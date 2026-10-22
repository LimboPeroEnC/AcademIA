export interface IndicadorEstudiante {
  rut: string;
  nombreCompleto: string;
  carrera: string;
  promedioPonderado: number;
  porcentajeAsistencia: number;
  creditosAprobados: number;
  creditosTotales: number;
  estadoAcademico: 'Al día' | 'Riesgo Moderado' | 'Riesgo Crítico';
}

export interface AsignaturaEstudiante {
  codigo: string;
  nombre: string;
  creditos: number;
  estado: 'Aprobada' | 'Reprobada' | 'Pendiente' | 'En Curso';
  notaFinal?: number;
  semestre: string;
}

export interface IndicadorEstudiante {
  rut: string;
  nombreCompleto: string;
  carrera: string;
  promedioPonderado: number;
  porcentajeAsistencia: number;
  creditosAprobados: number;
  creditosTotales: number;
  estadoAcademico: 'Al día' | 'Riesgo Moderado' | 'Riesgo Crítico';
  asignaturas?: AsignaturaEstudiante[];
  }