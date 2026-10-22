
#include "Evaluacion.hpp"

Evaluacion::Evaluacion(const DatosEvaluacion& datos)
    : datos(datos) {
}

const DatosEvaluacion& Evaluacion::obtenerDatos() const {
    return datos;
}
