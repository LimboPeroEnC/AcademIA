
#ifndef EVALUACION_HPP
#define EVALUACION_HPP

#include <string>

struct DatosEvaluacion {
    std::string codigo;
    std::string codigoCurso;
    std::string nombre;
    std::string tipo;
    std::string fecha;       // AAAA-MM-DD
    double ponderacion;     // Porcentaje: 0 < p <= 100
};

class Evaluacion {
private:
    DatosEvaluacion datos;

public:
    explicit Evaluacion(const DatosEvaluacion& datos);
    const DatosEvaluacion& obtenerDatos() const;
};

#endif
