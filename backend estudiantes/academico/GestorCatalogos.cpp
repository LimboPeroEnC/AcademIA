#include "GestorCatalogos.hpp"
#include "../util/ValidadorRut.hpp"
#include <stdexcept>
namespace {
void requerido(const std::string& s,std::size_t n) {
    if(s.size()>n || s.find_first_not_of(" \t\n\r")==std::string::npos)
        throw std::invalid_argument("Campo obligatorio o demasiado largo");
}
void enteroValido(int n,int min,int max) {
    if(n<min||n>max) throw std::invalid_argument("Valor numerico fuera de rango");
}
}
std::vector<FacultadAcademica> GestorCatalogos::facultades() const {return repo_.facultades();}
bool GestorCatalogos::crearFacultad(const FacultadAcademica& f) const {
    requerido(f.id,100);requerido(f.nombre,100);return repo_.crearFacultad(f);
}
bool GestorCatalogos::editarFacultad(const FacultadAcademica& f) const {
    requerido(f.id,100);requerido(f.nombre,100);return repo_.editarFacultad(f);
}
bool GestorCatalogos::eliminarFacultad(const std::string& id) const {requerido(id,100);return repo_.eliminarFacultad(id);}
std::vector<CarreraAcademica> GestorCatalogos::carreras() const {return repo_.carreras();}
bool GestorCatalogos::crearCarrera(const CarreraAcademica& c) const {
    requerido(c.id,100);requerido(c.nombre,100);requerido(c.modalidad,100);requerido(c.idFacultad,100);
    enteroValido(c.semestres,1,20);return repo_.crearCarrera(c);
}
bool GestorCatalogos::editarCarrera(const CarreraAcademica& c) const {
    requerido(c.id,100);requerido(c.nombre,100);requerido(c.modalidad,100);requerido(c.idFacultad,100);
    enteroValido(c.semestres,1,20);return repo_.editarCarrera(c);
}
bool GestorCatalogos::actualizarPlan(const std::string& id,const std::string& nombre,int anio,bool vigente) const {
    requerido(id,50);requerido(nombre,150);enteroValido(anio,1900,2200);
    return repo_.actualizarPlan(id,nombre,anio,vigente);
}
std::vector<MateriaPlan> GestorCatalogos::asignaturasPlan(const std::string& plan) const {requerido(plan,50);return repo_.asignaturasPlan(plan);}
bool GestorCatalogos::asociarAsignatura(const std::string& p,const std::string& r,int s) const {
    requerido(p,50);requerido(r,100);enteroValido(s,1,20);return repo_.asociarAsignatura(p,r,s);
}
bool GestorCatalogos::quitarAsignatura(const std::string& p,const std::string& r) const {requerido(p,50);requerido(r,100);return repo_.quitarAsignatura(p,r);}
std::vector<PrerrequisitoPlan> GestorCatalogos::prerrequisitos(const std::string& p) const {requerido(p,50);return repo_.prerrequisitos(p);}
bool GestorCatalogos::crearPrerrequisito(const std::string& p,const std::string& r,const std::string& req) const {
    requerido(p,50);requerido(r,100);requerido(req,100);
    if(r==req)throw std::invalid_argument("Un ramo no puede ser prerrequisito de si mismo");
    return repo_.crearPrerrequisito(p,r,req);
}
bool GestorCatalogos::eliminarPrerrequisito(const std::string& p,const std::string& r,const std::string& req) const {
    requerido(p,50);requerido(r,100);requerido(req,100);return repo_.eliminarPrerrequisito(p,r,req);
}
bool GestorCatalogos::definirCreditos(const std::string& id,int c) const {
    requerido(id,100);enteroValido(c,1,100);return repo_.definirCreditos(id,c);
}
std::optional<FichaAcademica> GestorCatalogos::ficha(const std::string& rut) const {
    if(!ValidadorRut::validar(rut)) throw std::invalid_argument("RUT invalido");
    return repo_.ficha(ValidadorRut::normalizar(rut));
}
