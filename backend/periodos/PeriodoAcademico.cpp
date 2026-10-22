
#include "PeriodoAcademico.hpp"

// ==========================================
// CONSTRUCTOR
// ==========================================

PeriodoAcademico::PeriodoAcademico(
    const DatosPeriodoAcademico& datos
) : datos(datos), estado(EstadoPeriodo::Abierto) {
}

// ==========================================
// CONSULTAR DATOS
// ==========================================

const DatosPeriodoAcademico&
PeriodoAcademico::obtenerDatos() const {
    return datos;
}

// ==========================================
// OBTENER ESTADO
// ==========================================

EstadoPeriodo PeriodoAcademico::obtenerEstado() const {
    return estado;
}

// ==========================================
// OBTENER ESTADO EN TEXTO
// ==========================================

std::string PeriodoAcademico::obtenerEstadoTexto() const {

    if (estado == EstadoPeriodo::Abierto) {
        return "Abierto";
    }

    return "Cerrado";
}

// ==========================================
// MODIFICAR PERIODO ACADEMICO
// ==========================================

bool PeriodoAcademico::actualizarDatos(
    const DatosPeriodoAcademico& nuevosDatos
) {

    // Impedir modificar periodos cerrados
    if (estado == EstadoPeriodo::Cerrado) {
        return false;
    }

    // El codigo identificador no puede cambiar
    if (nuevosDatos.codigo != datos.codigo) {
        return false;
    }

    // Actualizar informacion
    datos = nuevosDatos;

    return true;
}

// ==========================================
// CERRAR PERIODO ACADEMICO
// ==========================================

bool PeriodoAcademico::cerrarPeriodo() {

    // Evitar cerrar un periodo ya cerrado
    if (estado == EstadoPeriodo::Cerrado) {
        return false;
    }

    estado = EstadoPeriodo::Cerrado;

    return true;
}
