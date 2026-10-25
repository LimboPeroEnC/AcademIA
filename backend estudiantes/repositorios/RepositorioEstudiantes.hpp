#pragma once
#include "../modelos/Estudiante.hpp"
#include "../gestores/GestorEstudiantes.hpp"
#include <vector>
#include <string>
#include <cstddef>
#include <optional>

struct CarreraBasica { std::string id; std::string nombre; };

// Capa de persistencia. Las consultas a PostgreSQL quedan fuera de las entidades POO.
class RepositorioEstudiantes {
public:
    bool existeRut(const std::string& rut) const;
    bool existeCorreo(const std::string& correo, const std::string& rutExcluir) const;
    bool insertar(const Estudiante& estudiante) const;
    std::optional<Estudiante> obtenerPorRut(const std::string& rut) const;
    std::vector<Estudiante> buscarGeneral(const std::string& criterio) const;
    std::vector<Estudiante> buscarAvanzada(const FiltrosEstudiante& filtros, std::size_t pagina) const;
    bool actualizar(const std::string& rut, const DatosEstudiante& datos) const;
    bool cambiarEstado(const std::string& rut, const std::string& nuevoEstado,
        const std::string& motivo, const std::string& responsable) const;
    std::vector<HistorialEstado> historial(const std::string& rut) const;
    std::vector<CarreraBasica> carreras() const;
};
