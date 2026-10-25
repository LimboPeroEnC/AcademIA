#pragma once
#include <optional>
#include <string>
#include <vector>

struct Asignatura {
    std::string idRamo;
    std::string nombre;
    std::string descripcion;
    int creditos = 0;
    int semestreReferencia = 1;
};
struct PlanEstudios {
    std::string idPlan;
    std::string idCarrera;
    std::string nombre;
    int anio = 0;
    bool vigente = true;
};
struct RamoMalla {
    std::string idRamo;
    std::string nombre;
    int creditos = 0;
    int semestre = 1;
};
struct EstudiantePlan {
    std::string rut;
    std::string idPlan;
    std::string nombrePlan;
    std::string idCarrera;
    std::string nombreCarrera;
    std::string fechaAsignacion;
};

// SQL y transacciones están aislados aquí; las clases de dominio no conocen libpq.
class RepositorioAcademico {
public:
    std::vector<Asignatura> listarAsignaturas(const std::string& busqueda) const;
    std::optional<Asignatura> obtenerAsignatura(const std::string& id) const;
    bool insertarAsignatura(const Asignatura& a) const;
    bool modificarAsignatura(const std::string& id, const Asignatura& a) const;
    std::vector<PlanEstudios> listarPlanes(const std::string& idCarrera) const;
    bool crearPlan(const PlanEstudios& plan) const;
    std::vector<RamoMalla> malla(const std::string& idPlan) const;
    std::optional<EstudiantePlan> planDeEstudiante(const std::string& rut) const;
    // AC-48 actualiza carrera y plan en UNA transacción atómica.
    bool asociarEstudiante(const std::string& rut, const std::string& carrera,
                           const std::string& plan) const;
};
