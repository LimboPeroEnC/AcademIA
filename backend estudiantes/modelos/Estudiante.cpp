
#include "Estudiante.hpp"

#include <stdexcept>

Estudiante::Estudiante(
    const DatosEstudiante& datos,
    const std::string& codigoBarra
) : datos(datos),
    codigoBarra(codigoBarra),
    estado("Activo") {
}

std::string Estudiante::obtenerRut() const {
    return datos.rut;
}

std::string Estudiante::obtenerNombreCompleto() const {
    return datos.nombres + " " +
           datos.apellidoPaterno + " " +
           datos.apellidoMaterno;
}

std::string Estudiante::obtenerEstado() const {
    return estado;
}

std::string Estudiante::obtenerCodigoBarra() const {
    return codigoBarra;
}

DatosEstudiante Estudiante::obtenerDatos() const {
    return datos;
}

void Estudiante::actualizarDatos(
    const DatosEstudiante& nuevosDatos
) {
    DatosEstudiante actualizados = nuevosDatos;

    // El RUT no puede modificarse.
    actualizados.rut = datos.rut;

    datos = actualizados;
}

void Estudiante::establecerEstado(
    const std::string& nuevoEstado
) {
    if (
        nuevoEstado != "Activo" &&
        nuevoEstado != "Inactivo"
    ) {
        throw std::invalid_argument(
            "Estado del estudiante no valido."
        );
    }

    estado = nuevoEstado;
}
