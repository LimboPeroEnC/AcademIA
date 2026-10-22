
#ifndef GESTOR_ESTUDIANTES_H
#define GESTOR_ESTUDIANTES_H

#include <string>
#include <vector>
#include <ctime>
#include <cstddef>

#include "Estudiante.hpp"

// ==========================================
// FILTROS DE BUSQUEDA
// ==========================================

struct FiltrosEstudiante {
    std::string rut;
    std::string nombres;
    std::string apellidoPaterno;
    std::string apellidoMaterno;
    std::string codigoBarra;
    std::string estado;
    std::string carrera;
};

// ==========================================
// HISTORIAL DE CAMBIOS DE ESTADO
// ==========================================

struct HistorialEstado {
    std::string rutEstudiante;
    std::string estadoAnterior;
    std::string estadoNuevo;
    std::string motivo;
    std::string usuarioResponsable;
    std::time_t fechaCambio;
};

// ==========================================
// CLASE GESTOR DE ESTUDIANTES
// ==========================================

class GestorEstudiantes {

private:

    std::vector<Estudiante> estudiantes;

    std::vector<HistorialEstado> historialEstados;

    bool existeRut(
        const std::string& rut
    ) const;

    bool existeCorreo(
        const std::string& correo,
        const std::string& rutExcluir = ""
    ) const;

public:

    // AC-001: Registrar estudiante
    bool registrarEstudiante(
        const DatosEstudiante& datos
    );

    // AC-002: Busqueda general
    std::vector<Estudiante> buscarEstudiantes(
        const std::string& criterio
    ) const;

    // AC-002: Busqueda avanzada
    std::vector<Estudiante> buscarEstudiantes(
        const FiltrosEstudiante& filtros,
        std::size_t pagina = 1
    ) const;

    // AC-003: Editar estudiante
    bool editarEstudiante(
        const std::string& rut,
        const DatosEstudiante& nuevosDatos
    );

    // AC-004: Cambiar estado
    bool cambiarEstado(
        const std::string& rut,
        const std::string& nuevoEstado,
        const std::string& motivo,
        const std::string& usuarioResponsable
    );

    // AC-004: Consultar historial
    std::vector<HistorialEstado> consultarHistorial(
        const std::string& rut
    ) const;
};

#endif
