
#ifndef GESTOR_CALIFICACIONES_HPP
#define GESTOR_CALIFICACIONES_HPP

#include <string>
#include <vector>
#include "Evaluacion.hpp"

class GestorCursos;
class GestorMatriculas;

// ==========================================
// HISTORIAL DE CAMBIOS DE NOTAS
// ==========================================

struct CambioNota {
    double notaAnterior;
    double notaNueva;
    std::string motivo;
    std::string responsable;
    std::string fechaHora;
};

// ==========================================
// REGISTRO DE CALIFICACION
// ==========================================

struct RegistroNota {
    std::string codigoEvaluacion;
    std::string rutEstudiante;
    double nota;
    bool publicada;
    std::vector<CambioNota> historial;
};

// ==========================================
// RESULTADO DEL PROMEDIO
// ==========================================

struct ResultadoPromedio {
    bool disponible = false;
    bool definitivo = false;
    double promedio = 0.0;
    double ponderacionEvaluada = 0.0;
    std::string mensaje;
};

// ==========================================
// CONDICION POR CALIFICACIONES
// ==========================================

enum class CondicionPorNotas {
    Pendiente,
    Aprobado,
    Reprobado
};

// ==========================================
// GESTOR DE CALIFICACIONES
// ==========================================

class GestorCalificaciones {

private:
    GestorCursos& cursos;
    const GestorMatriculas& matriculas;

    std::vector<Evaluacion> evaluaciones;
    std::vector<RegistroNota> notas;

    std::string ultimoError;

    // Funciones auxiliares

    static bool tieneTexto(
        const std::string& texto
    );

    static bool fechaValida(
        const std::string& fecha
    );

    bool docenteHabilitado(
        const std::string& codigoCurso,
        const std::string& codigoDocente
    ) const;

    bool estaMatriculado(
        const std::string& rut,
        const std::string& codigoCurso
    ) const;

    RegistroNota* buscarNotaEditable(
        const std::string& codigoEvaluacion,
        const std::string& rut
    );

public:

    // ======================================
    // CONSTRUCTOR
    // ======================================

    GestorCalificaciones(
        GestorCursos& gestorCursos,
        const GestorMatriculas& gestorMatriculas
    );

    // ======================================
    // AC-66: EVALUACIONES
    // ======================================

    bool registrarEvaluacion(
        const DatosEvaluacion& datos,
        const std::string& codigoDocente
    );

    const Evaluacion* buscarEvaluacion(
        const std::string& codigo
    ) const;

    std::vector<Evaluacion> consultarEvaluaciones(
        const std::string& codigoCurso
    ) const;

    // ======================================
    // AC-67: CALIFICACIONES
    // ======================================

    bool registrarNota(
        const std::string& codigoEvaluacion,
        const std::string& rutEstudiante,
        double nota,
        const std::string& codigoDocente
    );

    bool modificarNota(
        const std::string& codigoEvaluacion,
        const std::string& rutEstudiante,
        double nuevaNota,
        const std::string& motivo,
        const std::string& codigoDocente
    );

    bool publicarNota(
        const std::string& codigoEvaluacion,
        const std::string& rutEstudiante,
        const std::string& codigoDocente
    );

    // ======================================
    // AC-70: CONSULTAS
    // ======================================

    std::vector<RegistroNota> consultarNotas(
        const std::string& rutEstudiante,
        const std::string& codigoCurso,
        bool soloPublicadas = true
    ) const;

    const RegistroNota* buscarNota(
        const std::string& codigoEvaluacion,
        const std::string& rutEstudiante
    ) const;

    // ======================================
    // AC-68: PROMEDIOS
    // AC-69: APROBACION Y REPROBACION
    // ======================================

    ResultadoPromedio calcularPromedio(
        const std::string& rutEstudiante,
        const std::string& codigoCurso,
        bool soloPublicadas = false
    ) const;

    CondicionPorNotas determinarCondicionPorNotas(
        const std::string& rutEstudiante,
        const std::string& codigoCurso
    ) const;

    const std::string& obtenerUltimoError() const;
};

#endif
