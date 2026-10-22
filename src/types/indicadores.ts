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