
#ifndef GESTOR_MATRICULAS_HPP
#define GESTOR_MATRICULAS_HPP

#include <functional>
#include <string>
#include <vector>

#include "Matricula.hpp"

// Declaraciones de otros modulos
class GestorCursos;
class GestorPeriodos;
class Curso;

// ==========================================
// CLASE GESTOR DE MATRICULAS
// ==========================================

class GestorMatriculas {

public:

    // Validador de estudiantes activos
    using ValidadorEstudiante =
        std::function<bool(const std::string&)>;

    // Validador de prerrequisitos
    // Recibe RUT y codigo de asignatura
    using ValidadorPrerequisitos =
        std::function<bool(
            const std::string&,
            const std::string&
        )>;

private:

    // Almacenamiento temporal
    std::vector<Matricula> matriculas;

    // Modulos existentes
    GestorCursos& cursos;
    const GestorPeriodos& periodos;

    // Validaciones externas
    ValidadorEstudiante estudianteActivo;
    ValidadorPrerequisitos prerrequisitosCumplidos;

    // Ultimo mensaje de error
    std::string ultimoError;

    // ======================================
    // FUNCIONES AUXILIARES
    // ======================================

    static bool tieneTexto(
        const std::string& texto
    );

    static bool horariosSeCruzan(
        const Curso& primero,
        const Curso& segundo
    );

    Matricula* buscarEditable(
        const std::string& codigo
    );

    bool validarInscripcion(
        const std::string& rut,
        const std::string& codigoCurso,
        const std::string& codigoMatriculaExcluir,
        std::string& motivo
    ) const;

public:

    // ======================================
    // CONSTRUCTOR
    // ======================================

    GestorMatriculas(
        GestorCursos& gestorCursos,
        const GestorPeriodos& gestorPeriodos,
        ValidadorEstudiante validarEstudiante,
        ValidadorPrerequisitos validarPrerequisitos
    );

    // ======================================
    // AC-61: REGISTRAR MATRICULA
    // ======================================

    bool registrarMatricula(
        const DatosMatricula& datos
    );

    // ======================================
    // AC-62: VERIFICAR PRERREQUISITOS
    // ======================================

    bool verificarPrerequisitos(
        const std::string& rut,
        const std::string& codigoAsignatura
    ) const;

    // ======================================
    // AC-63: CONSULTAR INSCRITOS
    // ======================================

    const Matricula* buscarMatricula(
        const std::string& codigo
    ) const;

    std::vector<Matricula> listarMatriculas() const;

    std::vector<Matricula> consultarInscritos(
        const std::string& codigoCurso
    ) const;

    std::vector<Matricula> consultarPorEstudiante(
        const std::string& rut,
        bool incluirAnuladas = true
    ) const;

    // ======================================
    // AC-64: MODIFICAR O ANULAR
    // ======================================

    bool cambiarSeccion(
        const std::string& codigoMatricula,
        const std::string& nuevoCodigoCurso,
        const std::string& motivo,
        const std::string& responsable
    );

    bool anularMatricula(
        const std::string& codigoMatricula,
        const std::string& motivo,
        const std::string& responsable
    );

    // ======================================
    // CONSULTAR ERRORES
    // ======================================

    const std::string& obtenerUltimoError() const;
};

#endif
