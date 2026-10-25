#ifndef GESTOR_ESTUDIANTES_H
#define GESTOR_ESTUDIANTES_H
#include <string>
#include <vector>
#include <ctime>
#include <cstddef>
#include <memory>
#include "../modelos/Estudiante.hpp"

struct FiltrosEstudiante {
    std::string rut,nombres,apellidoPaterno,apellidoMaterno,codigoBarra,estado,carrera;
};
struct HistorialEstado {
    std::string rutEstudiante,estadoAnterior,estadoNuevo,motivo,usuarioResponsable;
    std::time_t fechaCambio;
};
class RepositorioEstudiantes;
class GestorEstudiantes {
    std::shared_ptr<RepositorioEstudiantes> repositorio_;
    bool existeRut(const std::string& rut) const;
    bool existeCorreo(const std::string& correo, const std::string& rutExcluir = "") const;
public:
    GestorEstudiantes();
    explicit GestorEstudiantes(std::shared_ptr<RepositorioEstudiantes> repositorio);
    bool registrarEstudiante(const DatosEstudiante& datos);
    std::vector<Estudiante> buscarEstudiantes(const std::string& criterio) const;
    std::vector<Estudiante> buscarEstudiantes(const FiltrosEstudiante& filtros, std::size_t pagina = 1) const;
    bool editarEstudiante(const std::string& rut, const DatosEstudiante& nuevosDatos);
    bool cambiarEstado(const std::string& rut, const std::string& nuevoEstado,
        const std::string& motivo,const std::string& usuarioResponsable);
    std::vector<HistorialEstado> consultarHistorial(const std::string& rut) const;
};
#endif
