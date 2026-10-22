
#ifndef GESTOR_CURSOS_HPP
#define GESTOR_CURSOS_HPP

#include <functional>
#include <string>
#include <vector>

#include "Curso.hpp"

class GestorCursos {

public:

    using ValidadorCodigo =
        std::function<bool(const std::string&)>;

private:

    std::vector<Curso> cursos;

    // Validadores de modulos externos
    ValidadorCodigo periodoAbierto;
    ValidadorCodigo asignaturaActiva;
    ValidadorCodigo docenteActivo;

    // ======================================
    // VALIDACIONES INTERNAS
    // ======================================

    static bool tieneTexto(
        const std::string& texto
    );

    static bool horariosValidos(
        const std::vector<HorarioCurso>& horarios
    );

    static bool horariosSeSuperponen(
        const std::vector<HorarioCurso>& primero,
        const std::vector<HorarioCurso>& segundo
    );

    bool validarDatos(
        const DatosCurso& datos
    ) const;

    bool existeDuplicado(
        const DatosCurso& datos,
        const std::string& codigoExcluir = ""
    ) const;

    bool docenteTieneConflicto(
        const std::string& identificadorDocente,
        const std::string& codigoPeriodo,
        const std::vector<HorarioCurso>& horarios,
        const std::string& codigoExcluir = ""
    ) const;

    Curso* buscarCursoEditable(
        const std::string& codigo
    );

public:

    // ======================================
    // CONSTRUCTOR
    // ======================================

    GestorCursos(
        ValidadorCodigo validarPeriodoAbierto,
        ValidadorCodigo validarAsignaturaActiva,
        ValidadorCodigo validarDocenteActivo
    );

    // ======================================
    // AC-59: GESTION DE CURSOS Y SECCIONES
    // ======================================

    bool registrarCurso(
        const DatosCurso& datos
    );

    bool modificarCurso(
        const std::string& codigo,
        const DatosCurso& nuevosDatos
    );

    bool cerrarCurso(
        const std::string& codigo
    );

    const Curso* buscarCurso(
        const std::string& codigo
    ) const;

    std::vector<Curso> listarCursos() const;

    // ======================================
    // AC-60: ASIGNACION DE DOCENTES
    // ======================================

    bool asignarDocente(
        const std::string& codigoCurso,
        const std::string& identificadorDocente
    );

    bool quitarDocente(
        const std::string& codigoCurso
    );

    // ======================================
    // AC-65: CONSULTAR OFERTA ACADEMICA
    // ======================================

    std::vector<Curso> consultarOferta(
        const std::string& codigoPeriodo = "",
        const std::string& codigoAsignatura = "",
        const std::string& seccion = "",
        bool soloConCupos = false
    ) const;

    // ======================================
    // APOYO AL FUTURO MODULO DE MATRICULAS
    // ======================================

    bool ocuparCupo(
        const std::string& codigoCurso
    );

    bool liberarCupo(
        const std::string& codigoCurso
    );
};

#endif
