'use server';

import { pool } from '@/lib/db';

export interface AsignaturaSimulacion {
  codigo: string;
  nombre: string;
  semestre: number;
  creditos: number;
  estadoHabilitacion: 'Habilitada' | 'Bloqueada' | 'Ya Aprobada';
  prerrequisitos: string[];
  prerrequisitosFaltantes: string[];
}

export interface ProyeccionCurricular {
  creditosAprobadosActuales: number;
  creditosTotalesCarrera: number;
  porcentajeAvanceActual: number;
  creditosProyectados: number;
  porcentajeAvanceProyectado: number;
  semestresRestantesEstimados: number;
}

export interface ResultadoSimulacionBD {
  rut: string;
  nombreEstudiante: string;
  carrera: string;
  periodoSimulado: string;
  asignaturas: AsignaturaSimulacion[];
  proyeccion: ProyeccionCurricular;
  tieneDatosSuficientes: boolean;
  mensajeControl?: string;
}

export async function simularProgresionAcademica(rutEstudiante: string): Promise<ResultadoSimulacionBD | null> {
  const periodoSimulado = 'Período Académico Próximo (2026-2)';

  try {
    const client = await pool.connect();

    // 1. Obtener datos del estudiante
    const queryEstudiante = `
      SELECT e.rut, e.nombres, e.apellido_paterno, e.apellido_materno, c.nombre_carrera
      FROM public.estudiante e
      LEFT JOIN public.carrera c ON e.id_carrera = c.id_carrera
      WHERE e.rut = $1
      LIMIT 1;
    `;

    const resEst = await client.query(queryEstudiante, [rutEstudiante.trim()]);

    if (resEst.rows.length === 0) {
      client.release();
      return null;
    }

    const est = resEst.rows[0];

    // 2. Obtener la malla completa de ramos de PostgreSQL
    const queryRamos = `
      SELECT id_ramo AS codigo, nombre_ramo AS nombre, cantidad_creditos AS creditos, numero_semestre AS semestre
      FROM public.ramo
      ORDER BY numero_semestre, id_ramo;
    `;

    const resRamos = await client.query(queryRamos);
    client.release();

    if (resRamos.rows.length === 0) {
      return {
        rut: est.rut,
        nombreEstudiante: `${est.nombres} ${est.apellido_paterno} ${est.apellido_materno}`,
        carrera: est.nombre_carrera || 'Ingeniería Civil en Computación e Informática',
        periodoSimulado,
        asignaturas: [],
        proyeccion: {
          creditosAprobadosActuales: 0,
          creditosTotalesCarrera: 240,
          porcentajeAvanceActual: 0,
          creditosProyectados: 0,
          porcentajeAvanceProyectado: 0,
          semestresRestantesEstimados: 0,
        },
        tieneDatosSuficientes: false,
        mensajeControl: 'No existen asignaturas ni créditos parametrizados en la malla curricular de PostgreSQL.',
      };
    }

    // Conjunto vacio inicial de ramos cursados (se llena dinámicamente con las casillas en la interfaz)
    const ramosAprobadosIds = new Set<string>();

    // Mapeo explicito de prerrequisitos base
    const mapaPrerrequisitos: Record<string, { codigos: string[]; nombres: string[] }> = {
      'INF-201': { codigos: ['INF-101'], nombres: ['Programación I'] },
      'INF-301': { codigos: ['INF-201'], nombres: ['Estructura de Datos'] },
      'INF-401': { codigos: ['INF-301'], nombres: ['Bases de Datos'] },
      'ICCI23': { codigos: ['INF-101'], nombres: ['Programación I'] },
    };

    let creditosAprobadosActuales = 0;
    const creditosTotalesCarrera = 240;

    const asignaturasEvaluadas: AsignaturaSimulacion[] = resRamos.rows.map((ramo) => {
      const yaAprobado = ramosAprobadosIds.has(ramo.codigo);
      const reqInfo = mapaPrerrequisitos[ramo.codigo] || { codigos: [], nombres: [] };

      const faltantes = reqInfo.nombres.filter((_, idx) => !ramosAprobadosIds.has(reqInfo.codigos[idx]));
      const cumplido = reqInfo.codigos.every((reqId) => ramosAprobadosIds.has(reqId));

      let estado: 'Habilitada' | 'Bloqueada' | 'Ya Aprobada' = 'Habilitada';
      if (yaAprobado) {
        estado = 'Ya Aprobada';
        creditosAprobadosActuales += ramo.creditos || 6;
      } else if (!cumplido && reqInfo.codigos.length > 0) {
        estado = 'Bloqueada';
      }

      return {
        codigo: ramo.codigo,
        nombre: ramo.nombre,
        semestre: ramo.semestre || 1,
        creditos: ramo.creditos || 6,
        estadoHabilitacion: estado,
        prerrequisitos: reqInfo.nombres,
        prerrequisitosFaltantes: faltantes,
      };
    });

    const porcentajeAvanceActual = Math.round((creditosAprobadosActuales / creditosTotalesCarrera) * 100);
    const creditosPendientes = creditosTotalesCarrera - creditosAprobadosActuales;
    const semestresRestantesEstimados = Math.ceil(creditosPendientes / 30);

    return {
      rut: est.rut,
      nombreEstudiante: `${est.nombres} ${est.apellido_paterno} ${est.apellido_materno}`,
      carrera: est.nombre_carrera || 'Ingeniería Civil en Computación e Informática',
      periodoSimulado,
      asignaturas: asignaturasEvaluadas,
      proyeccion: {
        creditosAprobadosActuales,
        creditosTotalesCarrera,
        porcentajeAvanceActual,
        creditosProyectados: creditosAprobadosActuales,
        porcentajeAvanceProyectado: porcentajeAvanceActual,
        semestresRestantesEstimados,
      },
      tieneDatosSuficientes: true,
    };
  } catch (error) {
    console.error('Error al simular avance curricular:', error);
    return null;
  }
}