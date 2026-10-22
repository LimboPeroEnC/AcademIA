
#include "Curso.hpp"

// ==========================================
// CONSTRUCTOR
// ==========================================

Curso::Curso(
    const DatosCurso& datos
)
    : datos(datos),
      estado(EstadoCurso::Activo),
      identificadorDocente(""),
      cantidadInscritos(0) {
}

// ==========================================
// OBTENER DATOS DEL CURSO
// ==========================================

const DatosCurso& Curso::obtenerDatos() const {
    return datos;
}

// ==========================================
// OBTENER ESTADO DEL CURSO
// ==========================================

EstadoCurso Curso::obtenerEstado() const {
    return estado;
}

// ==========================================
// OBTENER ESTADO COMO TEXTO
// ==========================================

std::string Curso::obtenerEstadoTexto() const {

    if (estado == EstadoCurso::Activo) {
        return "Activo";
    }

    return "Cerrado";
}

// ==========================================
// OBTENER DOCENTE ASIGNADO
// ==========================================

std::string Curso::obtenerDocente() const {
    return identificadorDocente;
}

// ==========================================
// OBTENER ESTUDIANTES INSCRITOS
// ==========================================

int Curso::obtenerCantidadInscritos() const {
    return cantidadInscritos;
}

// ==========================================
// OBTENER CUPOS DISPONIBLES
// ==========================================

int Curso::obtenerCuposDisponibles() const {

    return datos.capacidadMaxima -
           cantidadInscritos;
}

// ==========================================
// ACTUALIZAR DATOS DEL CURSO
// ==========================================

bool Curso::actualizarDatos(
    const DatosCurso& nuevosDatos
) {

    // No permitir cambios en cursos cerrados
    if (estado == EstadoCurso::Cerrado) {
        return false;
    }

    // El codigo del curso es inmutable
    if (nuevosDatos.codigo != datos.codigo) {
        return false;
    }

    // No reducir capacidad bajo los inscritos
    if (nuevosDatos.capacidadMaxima <
        cantidadInscritos) {
        return false;
    }

    datos = nuevosDatos;

    return true;
}

// ==========================================
// ASIGNAR DOCENTE
// ==========================================

bool Curso::asignarDocente(
    const std::string& identificador
) {

    if (estado == EstadoCurso::Cerrado) {
        return false;
    }

    if (identificador.empty()) {
        return false;
    }

    // No reemplazar una asignacion existente
    // sin retirarla previamente
    if (!identificadorDocente.empty()) {
        return false;
    }

    identificadorDocente = identificador;

    return true;
}

// ==========================================
// QUITAR DOCENTE ASIGNADO
// ==========================================

bool Curso::quitarDocente() {

    if (estado == EstadoCurso::Cerrado) {
        return false;
    }

    if (identificadorDocente.empty()) {
        return false;
    }

    identificadorDocente.clear();

    return true;
}

// ==========================================
// OCUPAR CUPO
// ==========================================

bool Curso::ocuparCupo() {

    if (estado == EstadoCurso::Cerrado) {
        return false;
    }

    if (cantidadInscritos >=
        datos.capacidadMaxima) {
        return false;
    }

    cantidadInscritos++;

    return true;
}

// ==========================================
// LIBERAR CUPO
// ==========================================

bool Curso::liberarCupo() {

    if (cantidadInscritos <= 0) {
        return false;
    }

    cantidadInscritos--;

    return true;
}

// ==========================================
// CERRAR CURSO
// ==========================================

bool Curso::cerrarCurso() {

    if (estado == EstadoCurso::Cerrado) {
        return false;
    }

    estado = EstadoCurso::Cerrado;

    return true;
}
