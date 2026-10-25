#pragma once
#include "RepositorioAcademico.hpp"
#include <optional>
#include <string>
#include <vector>

// Lógica de negocio/validación POO (sin SQL dentro del gestor).
class GestorAcademico {
    RepositorioAcademico repo_;
public:
    std::vector<Asignatura> buscarAsignaturas(const std::string& filtro) const;
    std::optional<Asignatura> asignatura(const std::string& id) const;
    bool registrarAsignatura(const Asignatura& a) const;
    bool modificarAsignatura(const std::string& id,const Asignatura& a) const;
    std::vector<PlanEstudios> planes(const std::string& carrera) const;
    bool registrarPlan(const PlanEstudios& p) const;
    std::vector<RamoMalla> visualizarMalla(const std::string& idPlan) const;
    std::optional<EstudiantePlan> consultarPlanEstudiante(const std::string& rut) const;
    bool asignarPlanEstudiante(const std::string& rut,const std::string& carrera,
                              const std::string& plan) const;
};
