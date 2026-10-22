
#include "Matricula.hpp"

#include <ctime>
#include <iomanip>
#include <sstream>

// ==========================================
// FUNCIONES AUXILIARES
// ==========================================

namespace {

    bool tieneTexto(const std::string& texto) {

        return texto.find_first_not_of(" \t\n\r")
               != std::string::npos;
    }

    std::string fechaHoraActual() {

        std::time_t instante = std::time(nullptr);

        std::tm* fecha = std::localtime(&instante);

        if (fecha == nullptr) {
            return "Fecha no disponible";
        }

        std::ostringstream salida;

        salida << std::put_time(
            fecha,
            "%Y-%m-%d %H:%M:%S"
        );

        return salida.str();
    }
}

// ==========================================
// CONSTRUCTOR
// ==========================================

Matricula::Matricula(
    const DatosMatricula& datos
)
    : datos(datos),
      estado(EstadoMatricula::Vigente),
      fechaRegistro(fechaHoraActual()) {
}

// ==========================================
// OBTENER DATOS DE MATRICULA
// ==========================================

const DatosMatricula&
Matricula::obtenerDatos() const {

    return datos;
}

// ==========================================
// OBTENER ESTADO
// ==========================================

EstadoMatricula Matricula::obtenerEstado() const {

    return estado;
}

// ==========================================
// OBTENER ESTADO COMO TEXTO
// ==========================================

std::string Matricula::obtenerEstadoTexto() const {

    if (estado == EstadoMatricula::Vigente) {
        return "Vigente";
    }

    return "Anulada";
}

// ==========================================
// OBTENER FECHA DE REGISTRO
// ==========================================

std::string Matricula::obtenerFechaRegistro() const {

    return fechaRegistro;
}

// ==========================================
// OBTENER HISTORIAL
// ==========================================

const std::vector<MovimientoMatricula>&
Matricula::obtenerHistorial() const {

    return historial;
}

// ==========================================
// VERIFICAR MATRICULA VIGENTE
// ==========================================

bool Matricula::estaVigente() const {

    return estado == EstadoMatricula::Vigente;
}

// ==========================================
// AC-64: CAMBIAR SECCION
// ==========================================

bool Matricula::cambiarSeccion(
    const std::string& nuevoCodigoCurso,
    const std::string& motivo,
    const std::string& responsable
) {

    // No modificar matriculas anuladas.

    if (!estaVigente()) {
        return false;
    }

    // Verificar datos obligatorios.

    if (
        !tieneTexto(nuevoCodigoCurso) ||
        !tieneTexto(motivo) ||
        !tieneTexto(responsable)
    ) {
        return false;
    }

    // No realizar cambios innecesarios.

    if (nuevoCodigoCurso == datos.codigoCurso) {
        return false;
    }

    // Crear registro de movimiento.

    MovimientoMatricula movimiento;

    movimiento.tipo =
        TipoMovimientoMatricula::CambioSeccion;

    movimiento.codigoCursoAnterior =
        datos.codigoCurso;

    movimiento.codigoCursoNuevo =
        nuevoCodigoCurso;

    movimiento.motivo = motivo;

    movimiento.responsable = responsable;

    movimiento.fechaHora = fechaHoraActual();

    // Guardar movimiento en el historial.

    historial.push_back(movimiento);

    // Actualizar curso asociado a la matricula.

    datos.codigoCurso = nuevoCodigoCurso;

    return true;
}

// ==========================================
// AC-64: ANULAR MATRICULA
// ==========================================

bool Matricula::anular(
    const std::string& motivo,
    const std::string& responsable
) {

    // No anular una matricula ya anulada.

    if (!estaVigente()) {
        return false;
    }

    // Validar motivo y responsable.

    if (
        !tieneTexto(motivo) ||
        !tieneTexto(responsable)
    ) {
        return false;
    }

    // Registrar anulacion en el historial.

    MovimientoMatricula movimiento;

    movimiento.tipo =
        TipoMovimientoMatricula::Anulacion;

    movimiento.codigoCursoAnterior =
        datos.codigoCurso;

    movimiento.codigoCursoNuevo = "";

    movimiento.motivo = motivo;

    movimiento.responsable = responsable;

    movimiento.fechaHora = fechaHoraActual();

    historial.push_back(movimiento);

    // Cambiar el estado.
    // No eliminamos la matricula.

    estado = EstadoMatricula::Anulada;

    return true;
}
