#pragma once
#include <optional>
#include <string>
#include <vector>

struct FacultadAcademica { std::string id, nombre; };
struct CarreraAcademica { std::string id, nombre, modalidad, idFacultad, nombreFacultad; int semestres=0; };
struct MateriaPlan { std::string idRamo, nombre; int semestre=0, creditos=0; };
struct PrerrequisitoPlan { std::string idRamo, nombreRamo, idRequisito, nombreRequisito; };
struct FichaAcademica {
    std::string rut, nombres, apellidoPaterno, apellidoMaterno, correo, estado;
    std::string carreraId, carreraNombre, facultadNombre;
    std::string planId, planNombre, fechaIngreso;
    std::vector<MateriaPlan> asignaturasPlan;
};

class RepositorioCatalogos {
public:
    std::vector<FacultadAcademica> facultades() const;
    bool crearFacultad(const FacultadAcademica&) const;
    bool editarFacultad(const FacultadAcademica&) const;
    bool eliminarFacultad(const std::string&) const; // Sin carreras relacionadas
    std::vector<CarreraAcademica> carreras() const;
    bool crearCarrera(const CarreraAcademica&) const;
    bool editarCarrera(const CarreraAcademica&) const;
    bool actualizarPlan(const std::string& id,const std::string& nombre,int anio,bool vigente) const;
    std::vector<MateriaPlan> asignaturasPlan(const std::string& idPlan) const;
    bool asociarAsignatura(const std::string& plan,const std::string& ramo,int semestre) const;
    bool quitarAsignatura(const std::string& plan,const std::string& ramo) const;
    std::vector<PrerrequisitoPlan> prerrequisitos(const std::string& idPlan) const;
    bool crearPrerrequisito(const std::string& plan,const std::string& ramo,const std::string& requisito) const;
    bool eliminarPrerrequisito(const std::string& plan,const std::string& ramo,const std::string& requisito) const;
    bool definirCreditos(const std::string& ramo,int creditos) const;
    std::optional<FichaAcademica> ficha(const std::string& rut) const;
};
