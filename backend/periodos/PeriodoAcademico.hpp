
#ifndef PERIODO_ACADEMICO_HPP
#define PERIODO_ACADEMICO_HPP

#include <string>

// Estados permitidos para un periodo
enum class EstadoPeriodo {
    Abierto,
    Cerrado
};

// Datos del periodo academico
struct DatosPeriodoAcademico {
    std::string codigo;
    std::string nombre;
    int anio;
    int semestre;
    std::string fechaInicio;
    std::string fechaFin;
};

// Clase principal del periodo academico
class PeriodoAcademico {

private:
    DatosPeriodoAcademico datos;
    EstadoPeriodo estado;

public:
    // Constructor
    explicit PeriodoAcademico(
        const DatosPeriodoAcademico& datos
    );

    // Consultar informacion
    const DatosPeriodoAcademico& obtenerDatos() const;

    EstadoPeriodo obtenerEstado() const;

    std::string obtenerEstadoTexto() const;

    // Modificar periodo academico
    bool actualizarDatos(
        const DatosPeriodoAcademico& nuevosDatos
    );

    // Cerrar periodo academico
    bool cerrarPeriodo();
};

#endif
