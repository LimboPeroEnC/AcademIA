
#ifndef MATRICULA_HPP
#define MATRICULA_HPP

#include <string>
#include <vector>

// ==========================================
// ESTADOS DE MATRICULA
// ==========================================

enum class EstadoMatricula {
    Vigente,
    Anulada
};

// ==========================================
// TIPOS DE MOVIMIENTO
// ==========================================

enum class TipoMovimientoMatricula {
    CambioSeccion,
    Anulacion
};

// ==========================================
// DATOS DE MATRICULA
// ==========================================

struct DatosMatricula {

    // Identificador unico de la matricula
    std::string codigo;

    // Estudiante matriculado
    std::string rutEstudiante;

    // Curso y seccion seleccionados
    std::string codigoCurso;

    // Asignatura correspondiente
    std::string codigoAsignatura;

    // Periodo academico
    std::string codigoPeriodo;
};

// ==========================================
// HISTORIAL DE MOVIMIENTOS
// ==========================================

struct MovimientoMatricula {

    TipoMovimientoMatricula tipo;

    std::string codigoCursoAnterior;

    std::string codigoCursoNuevo;

    std::string motivo;

    std::string responsable;

    std::string fechaHora;
};

// ==========================================
// CLASE MATRICULA
// ==========================================

class Matricula {

private:

    DatosMatricula datos;

    EstadoMatricula estado;

    std::string fechaRegistro;

    std::vector<MovimientoMatricula> historial;

    // Solo GestorMatriculas podra ejecutar
    // estas operaciones, despues de validar
    // cupos, periodos y prerrequisitos.

    friend class GestorMatriculas;

    bool cambiarSeccion(
        const std::string& nuevoCodigoCurso,
        const std::string& motivo,
        const std::string& responsable
    );

    bool anular(
        const std::string& motivo,
        const std::string& responsable
    );

public:

    // Constructor
    explicit Matricula(
        const DatosMatricula& datos
    );

    // ======================================
    // CONSULTAR DATOS
    // ======================================

    const DatosMatricula& obtenerDatos() const;

    EstadoMatricula obtenerEstado() const;

    std::string obtenerEstadoTexto() const;

    std::string obtenerFechaRegistro() const;

    const std::vector<MovimientoMatricula>&
    obtenerHistorial() const;

    // ======================================
    // CONSULTAR ESTADO
    // ======================================

    bool estaVigente() const;
};

#endif
