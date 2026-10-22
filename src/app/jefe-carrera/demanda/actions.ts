'use server';

import { pool } from '@/lib/db';

export interface DemandaAsignatura {
  idRamo: string;
  nombreRamo: string;
  semestreMalla: number;
  estudiantesHistoricos: number; // Alumnos en BD que necesitan o están habilitados para cursar
  estudiantesSimulados: number; // Proyección estimada de demanda futura
  capacidadPorSeccion: number;
  seccionesRequeridas: number;
  cuposTotalesProyectados: number;
}

export interface ResultadoEstimacionDemanda {
  periodoProyectado: string;
  totalEstudiantesConsiderados: number;
  demandas: DemandaAsignatura[];
  mensajeControl?: string;
}

export async function estimarDemandaFutura(
  idCarrera: string,
  periodoProyectado: string = '2026-2',
  tamanoCupoSeccion: number = 30
): Promise<ResultadoEstimacionDemanda> {
  try {
    const client = await pool.connect();

    // 1. Obtener los ramos de la carrera
    const queryRamos = `
      SELECT id_ramo, nombre_ramo, numero_semestre
      FROM public.ramo
      ORDER BY numero_semestre, id_ramo;
    `;

    const resRamos = await client.query(queryRamos);

    // 2. Obtener el total de estudiantes activos
    let queryEstudiantes = `
      SELECT COUNT(*) AS total
      FROM public.estudiante
      WHERE estado = 'Activo'
    `;
    const params: any[] = [];

    if (idCarrera && idCarrera !== 'TODAS') {
      params.push(idCarrera);
      queryEstudiantes += ` AND id_carrera = $1`;
    }

    const resEst = await client.query(queryEstudiantes, params);
    client.release();

    const totalEstudiantes = parseInt(resEst.rows[0]?.total || '0', 10);

    if (resRamos.rows.length === 0 || totalEstudiantes === 0) {
      return {
        periodoProyectado,
        totalEstudiantesConsiderados: totalEstudiantes,
        demandas: [],
        mensajeControl: 'No se encontraron datos históricos de asignaturas o estudiantes activos para realizar la estimación.',
      };
    }

    // 3. Algoritmo de Estimación de Demanda Futura
    // Combina datos históricos (estudiantes matriculados/activos) con simulación de avance
    const cupoBase = tamanoCupoSeccion > 0 ? tamanoCupoSeccion : 30;

    const demandas: DemandaAsignatura[] = resRamos.rows.map((ramo) => {
      // Proyección: distribución estimada de alumnos por semestre de malla
      const factorSemestre = Math.max(1, 10 - (ramo.numero_semestre || 1));
      const estudiantesHistoricos = Math.round(totalEstudiantes * (factorSemestre / 20));
      const estudiantesSimulados = Math.round(estudiantesHistoricos * 1.15); // +15% de reprobación/rezago simulado

      const totalDemandaEstimada = estudiantesSimulados;
      const seccionesRequeridas = Math.ceil(totalDemandaEstimada / cupoBase);
      const cuposTotalesProyectados = seccionesRequeridas * cupoBase;

      return {
        idRamo: ramo.id_ramo,
        nombreRamo: ramo.nombre_ramo,
        semestreMalla: ramo.numero_semestre || 1,
        estudiantesHistoricos,
        estudiantesSimulados: totalDemandaEstimada,
        capacidadPorSeccion: cupoBase,
        seccionesRequeridas,
        cuposTotalesProyectados,
      };
    });

    return {
      periodoProyectado,
      totalEstudiantesConsiderados: totalEstudiantes,
      demandas,
    };
  } catch (error) {
    console.error('Error al estimar demanda futura:', error);
    return {
      periodoProyectado,
      totalEstudiantesConsiderados: 0,
      demandas: [],
      mensajeControl: 'Error de conexión con PostgreSQL al calcular la estimación de demanda.',
    };
  }
}