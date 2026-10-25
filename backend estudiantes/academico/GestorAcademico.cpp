#include "GestorAcademico.hpp"
#include "../util/ValidadorRut.hpp"
#include <ctime>
#include <stdexcept>

namespace {
bool texto(const std::string& s,std::size_t limite) {
    return !s.empty() && s.size()<=limite && s.find_first_not_of(" \t\n\r")!=std::string::npos;
}
bool asignaturaValida(const Asignatura& a) {
    return texto(a.idRamo,100)&&texto(a.nombre,100)&&texto(a.descripcion,100)
        &&a.creditos>0&&a.creditos<=100&&a.semestreReferencia>=1&&a.semestreReferencia<=20;
}
}
std::vector<Asignatura> GestorAcademico::buscarAsignaturas(const std::string& f) const {
    if(f.size()>100)throw std::invalid_argument("Filtro demasiado largo");
    return repo_.listarAsignaturas(f);
}
std::optional<Asignatura> GestorAcademico::asignatura(const std::string& id) const {
    if(!texto(id,100))throw std::invalid_argument("Codigo de ramo invalido");
    return repo_.obtenerAsignatura(id);
}
bool GestorAcademico::registrarAsignatura(const Asignatura& a) const {
    if(!asignaturaValida(a))throw std::invalid_argument("Campos de asignatura invalidos");
    return repo_.insertarAsignatura(a);
}
bool GestorAcademico::modificarAsignatura(const std::string& id,const Asignatura& a) const {
    if(!asignaturaValida(a)||id!=a.idRamo)throw std::invalid_argument("No se permite cambiar el codigo del ramo");
    return repo_.modificarAsignatura(id,a);
}
std::vector<PlanEstudios> GestorAcademico::planes(const std::string& carrera) const {
    if(carrera.size()>100)throw std::invalid_argument("Carrera invalida");
    return repo_.listarPlanes(carrera);
}
bool GestorAcademico::registrarPlan(const PlanEstudios& p) const {
    if(!texto(p.idPlan,50)||!texto(p.idCarrera,100)||!texto(p.nombre,150)||
       p.anio<1900||p.anio>2200)throw std::invalid_argument("Datos del plan invalidos");
    return repo_.crearPlan(p);
}
std::vector<RamoMalla> GestorAcademico::visualizarMalla(const std::string& idPlan) const {
    if(!texto(idPlan,50))throw std::invalid_argument("ID de plan requerido");
    return repo_.malla(idPlan);
}
std::optional<EstudiantePlan> GestorAcademico::consultarPlanEstudiante(const std::string& rut) const {
    if(!ValidadorRut::validar(rut))throw std::invalid_argument("RUT invalido");
    return repo_.planDeEstudiante(ValidadorRut::normalizar(rut));
}
bool GestorAcademico::asignarPlanEstudiante(const std::string& rut,const std::string& carrera,
    const std::string& plan) const {
    if(!ValidadorRut::validar(rut)||!texto(carrera,100)||!texto(plan,50))
        throw std::invalid_argument("RUT, carrera o plan invalidos");
    return repo_.asociarEstudiante(ValidadorRut::normalizar(rut),carrera,plan);
}
