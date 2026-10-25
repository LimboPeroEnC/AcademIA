#pragma once
#include "RepositorioCatalogos.hpp"

// Regla de negocio separada de SQL (POO): validar antes de persistir.
class GestorCatalogos {
    RepositorioCatalogos repo_;
public:
    std::vector<FacultadAcademica> facultades() const;
    bool crearFacultad(const FacultadAcademica&) const;
    bool editarFacultad(const FacultadAcademica&) const;
    bool eliminarFacultad(const std::string&) const;
    std::vector<CarreraAcademica> carreras() const;
    bool crearCarrera(const CarreraAcademica&) const;
    bool editarCarrera(const CarreraAcademica&) const;
    bool actualizarPlan(const std::string&,const std::string&,int,bool) const;
    std::vector<MateriaPlan> asignaturasPlan(const std::string&) const;
    bool asociarAsignatura(const std::string&,const std::string&,int) const;
    bool quitarAsignatura(const std::string&,const std::string&) const;
    std::vector<PrerrequisitoPlan> prerrequisitos(const std::string&) const;
    bool crearPrerrequisito(const std::string&,const std::string&,const std::string&) const;
    bool eliminarPrerrequisito(const std::string&,const std::string&,const std::string&) const;
    bool definirCreditos(const std::string&,int) const;
    std::optional<FichaAcademica> ficha(const std::string&) const;
};
