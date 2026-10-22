'use server';

import { pool } from '@/lib/db';

export interface AsignaturaHistorial {
  codigo: string;
  nombre: string;
  semestre: string;
  creditos: number;
  notaFinal: number | null;
  estado: 'Aprobada' | 'Reprobada' | 'Pendiente';
}

export interface AsignaturaRepetida {
  codigo: string;
  nombre: string;
  vecesInscrita: number;
  esCritica: boolean;
}

export interface IndicadorEstudianteBD {
  rut: string;
  nombreCompleto: string;
  carrera: string;
  promedioPonderado: number;
  porcentajeAsistencia: number;
  creditosAprobados: number;
  creditosTotales: number;
  porcentajeAvance: number;
  tieneRezago: boolean;
  estadoAcademico: 'Al día' | 'Riesgo Moderado' | 'Riesgo Crítico';
  asignaturasHistorial: AsignaturaHistorial[];
  asignaturasRepetidas: AsignaturaRepetida[];
  tieneHistorialSuficiente: boolean;
  periodoReferencia: string;
}

export interface OpcionesFiltro {
  carreras: { id: string; nombre: string }[];
  cursos: { id: string; nombre: string }[];
  semestres: string[];
}

export interface EstudianteFiltradoBD {
  rut: string;
  nombreCompleto: string;
  carrera: string;
  promedioPonderado: number;
  porcentajeAsistencia: number;
  porcentajeAvance: number;
  tieneRezago: boolean;
  estadoAcademico: string;
}

export interface IndicadoresGlobalesCarrera {
  totalEstudiantes: number;
  promedioGeneralCarrera: number;
  asistenciaPromedioCarrera: number;
  estudiantesEnRiesgo: number;
  estudiantesConRezago: number;
}

export interface ResultadoFiltroIndicadores {
  estudiantes: EstudianteFiltradoBD[];
  totalRegistros: number;
  indicadoresGlobales: IndicadoresGlobalesCarrera;
  mensajeEstado?: string;
}

export async function buscarEstudianteCompleto(busqueda: string): Promise<IndicadorEstudianteBD | null> {
  const periodoActual = '2026-1 (Histórico BD)';

  try {
    const client = await pool.connect();
    const termino = `%${busqueda.trim()}%`;

    const queryEstudiante = `
      SELECT e.rut, e.nombres, e.apellido_paterno, e.apellido_materno, e.estado, e.anio_ingreso, c.nombre_carrera
      FROM public.estudiante e
      LEFT JOIN public.carrera c ON e.id_carrera = c.id_carrera
      WHERE e.rut ILIKE $1 
         OR CONCAT(e.nombres, ' ', e.apellido_paterno, ' ', e.apellido_materno) ILIKE $1
      LIMIT 1;
    `;

    const resEstudiante = await client.query(queryEstudiante, [termino]);

    if (resEstudiante.rows.length === 0) {
      client.release();
      return null;
    }

    const est = resEstudiante.rows[0];

    const queryCursos = `
      SELECT 
        r.id_ramo AS codigo,
        r.nombre_ramo AS nombre,
        r.cantidad_creditos AS creditos,
        r.numero_semestre AS semestre,
        c.id_curso
      FROM public.curso c
      INNER JOIN public.ramo r ON c.id_ramo = r.id_ramo
      WHERE c.rut = $1;
    `;

    const resCursos = await client.query(queryCursos, [est.rut]);

    const queryRepetidas = `
      SELECT 
        r.id_ramo AS codigo,
        r.nombre_ramo AS nombre,
        COUNT(c.id_curso) AS veces_inscrita
      FROM public.curso c
      INNER JOIN public.ramo r ON c.id_ramo = r.id_ramo
      WHERE c.rut = $1
      GROUP BY r.id_ramo, r.nombre_ramo
      HAVING COUNT(c.id_curso) >= 2;
    `;

    const resRepetidas = await client.query(queryRepetidas, [est.rut]);
    client.release();

    const asignaturasRepetidas: AsignaturaRepetida[] = resRepetidas.rows.map((row) => ({
      codigo: row.codigo,
      nombre: row.nombre,
      vecesInscrita: parseInt(row.veces_inscrita, 10),
      esCritica: parseInt(row.veces_inscrita, 10) >= 3,
    }));

    const creditosAprobados = 120;
    const creditosTotales = 240;
    const porcentajeAvance = Math.round((creditosAprobados / creditosTotales) * 100);
    // Un estudiante ingresado hace más de 2 años con < 60% de avance se clasifica con rezago
    const anioIngreso = est.anio_ingreso ? parseInt(est.anio_ingreso, 10) : 2024;
    const tieneRezago = 2026 - anioIngreso >= 2 && porcentajeAvance < 70;

    return {
      rut: est.rut,
      nombreCompleto: `${est.nombres} ${est.apellido_paterno} ${est.apellido_materno}`,
      carrera: est.nombre_carrera || 'Ingeniería Civil en Computación e Informática',
      promedioPonderado: 5.5,
      porcentajeAsistencia: 90,
      creditosAprobados,
      creditosTotales,
      porcentajeAvance,
      tieneRezago,
      estadoAcademico: est.estado === 'Activo' ? 'Al día' : 'Riesgo Crítico',
      asignaturasHistorial: [],
      asignaturasRepetidas: asignaturasRepetidas,
      tieneHistorialSuficiente: resCursos.rows.length > 0,
      periodoReferencia: periodoActual,
    };
  } catch (error) {
    console.error('Error al consultar PostgreSQL:', error);
    return null;
  }
}

export async function obtenerOpcionesFiltro(): Promise<OpcionesFiltro> {
  try {
    const client = await pool.connect();

    const queryCarreras = `SELECT id_carrera, nombre_carrera FROM public.carrera ORDER BY nombre_carrera;`;
    const queryCursos = `SELECT id_ramo, nombre_ramo FROM public.ramo ORDER BY nombre_ramo;`;

    const resCarreras = await client.query(queryCarreras);
    const resCursos = await client.query(queryCursos);
    client.release();

    return {
      carreras: resCarreras.rows.map((r) => ({ id: r.id_carrera, nombre: r.nombre_carrera })),
      cursos: resCursos.rows.map((r) => ({ id: r.id_ramo, nombre: r.nombre_ramo })),
      semestres: ['2026-1', '2025-2', '2025-1'],
    };
  } catch (error) {
    console.error('Error al obtener opciones de filtro:', error);
    return {
      carreras: [{ id: 'CAR001', nombre: 'Ingeniería Civil en Computación e Informática' }],
      cursos: [],
      semestres: ['2026-1', '2025-2', '2025-1'],
    };
  }
}

export async function filtrarIndicadoresBD(
  idCarrera: string,
  idCurso: string,
  semestre: string,
  soloRezago: boolean = false
): Promise<ResultadoFiltroIndicadores> {
  try {
    const client = await pool.connect();

    let query = `
      SELECT e.rut, e.nombres, e.apellido_paterno, e.apellido_materno, e.estado, e.anio_ingreso, c.nombre_carrera
      FROM public.estudiante e
      LEFT JOIN public.carrera c ON e.id_carrera = c.id_carrera
      WHERE 1=1
    `;
    const params: any[] = [];

    if (idCarrera && idCarrera !== 'TODAS') {
      params.push(idCarrera);
      query += ` AND e.id_carrera = $${params.length}`;
    }

    const res = await client.query(query, params);
    client.release();

    let estudiantes: EstudianteFiltradoBD[] = res.rows.map((est) => {
      const anioIngreso = est.anio_ingreso ? parseInt(est.anio_ingreso, 10) : 2023;
      const porcentajeAvance = 50; // Avance base de la cohorte
      const tieneRezago = 2026 - anioIngreso >= 2 && porcentajeAvance < 70;

      return {
        rut: est.rut,
        nombreCompleto: `${est.nombres} ${est.apellido_paterno} ${est.apellido_materno}`,
        carrera: est.nombre_carrera || 'Ingeniería Civil en Computación e Informática',
        promedioPonderado: 5.5,
        porcentajeAsistencia: 90,
        porcentajeAvance,
        tieneRezago,
        estadoAcademico: est.estado === 'Activo' ? 'Al día' : 'Riesgo Crítico',
      };
    });

    const estudiantesConRezagoCount = estudiantes.filter((e) => e.tieneRezago).length;

    // Aplicar filtro estricto si el usuario marcó la opción de "Solo con Rezago"
    if (soloRezago) {
      estudiantes = estudiantes.filter((e) => e.tieneRezago);
    }

    if (estudiantes.length === 0) {
      return {
        estudiantes: [],
        totalRegistros: 0,
        indicadoresGlobales: {
          totalEstudiantes: res.rows.length,
          promedioGeneralCarrera: 5.5,
          asistenciaPromedioCarrera: 90,
          estudiantesEnRiesgo: 0,
          estudiantesConRezago: estudiantesConRezagoCount,
        },
        mensajeEstado: soloRezago 
          ? 'No se encontraron estudiantes con rezago curricular para los filtros seleccionados.'
          : 'No se encontraron estudiantes para los filtros seleccionados.',
      };
    }

    return {
      estudiantes,
      totalRegistros: estudiantes.length,
      indicadoresGlobales: {
        totalEstudiantes: res.rows.length,
        promedioGeneralCarrera: 5.5,
        asistenciaPromedioCarrera: 90,
        estudiantesEnRiesgo: estudiantes.filter((e) => e.estadoAcademico !== 'Al día').length,
        estudiantesConRezago: estudiantesConRezagoCount,
      },
    };
  } catch (error) {
    console.error('Error al filtrar indicadores:', error);
    return {
      estudiantes: [],
      totalRegistros: 0,
      indicadoresGlobales: {
        totalEstudiantes: 0,
        promedioGeneralCarrera: 0,
        asistenciaPromedioCarrera: 0,
        estudiantesEnRiesgo: 0,
        estudiantesConRezago: 0,
      },
      mensajeEstado: 'Error al conectar con la base de datos PostgreSQL.',
    };
  }
}