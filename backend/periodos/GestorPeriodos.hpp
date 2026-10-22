
#ifndef GESTOR_PERIODOS_HPP
#define GESTOR_PERIODOS_HPP

#include <string>
#include <vector>

#include "PeriodoAcademico.hpp"

class GestorPeriodos {

private:
    std::vector<PeriodoAcademico> periodos;

    // Verificar si existe un codigo
    bool existeCodigo(
        const std::string& codigo
    ) const;

    // Verificar duplicados por anio y semestre
    bool existePeriodo(
        int anio,
        int semestre,
        const std::string& codigoExcluir = ""
    ) const;

    // Validar los datos del periodo
    bool validarDatos(
        const DatosPeriodoAcademico& datos
    ) const;

public:
    // AC-58: Crear periodo academico
    bool crearPeriodo(
        const DatosPeriodoAcademico& datos
    );

    // Consultar todos los periodos
    std::vector<PeriodoAcademico> listarPeriodos() const;

    // Buscar un periodo por su codigo
    const PeriodoAcademico* buscarPeriodo(
        const std::string& codigo
    ) const;

    // Modificar un periodo existente
    bool modificarPeriodo(
        const std::string& codigo,
        const DatosPeriodoAcademico& nuevosDatos
    );

    // Cerrar un periodo academico
    bool cerrarPeriodo(
        const std::string& codigo
    );

    // Comprobar si un periodo permite matriculas
    bool permiteMatricula(
        const std::string& codigo
    ) const;
};

#endif
