export interface Usuario {
  rut: string;
  nombres: string;
  apellidos: string;
  correo: string;
  rol: 'Administrador' | 'Académico' | 'Docente' | 'Estudiante';
  estado: 'Activo' | 'Inactivo';
}

export interface Estudiante extends Usuario {
  carrera_id: string;
  codigo_barra: string;
  año_ingreso: number;
}

export interface Ramo {
  codigo: string;
  nombre: string;
  creditos: number;
  estado: 'Activo' | 'Inactivo' | 'Anulado';
}