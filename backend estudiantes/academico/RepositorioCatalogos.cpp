#include "RepositorioCatalogos.hpp"
#include "../database/ConexionPostgreSQL.hpp"
#include <string>
namespace {
std::string c(PGresult* r,int i,int j) {return PQgetisnull(r,i,j)?"":PQgetvalue(r,i,j);}
int numero(PGresult* r,int i,int j) {return PQgetisnull(r,i,j)?0:std::stoi(c(r,i,j));}
}
std::vector<FacultadAcademica> RepositorioCatalogos::facultades() const {
    ConexionPostgreSQL db;
    auto r=db.ejecutar("SELECT id_facultad,nombre FROM public.facultad ORDER BY nombre");
    std::vector<FacultadAcademica> out;
    for(int i=0;i<PQntuples(r.get());++i) out.push_back({c(r.get(),i,0),c(r.get(),i,1)});
    return out;
}
bool RepositorioCatalogos::crearFacultad(const FacultadAcademica& f) const {
    ConexionPostgreSQL db;
    auto r=db.ejecutar("INSERT INTO public.facultad(id_facultad,nombre) VALUES($1,$2) RETURNING id_facultad",{f.id,f.nombre});
    return PQntuples(r.get())==1;
}
bool RepositorioCatalogos::editarFacultad(const FacultadAcademica& f) const {
    ConexionPostgreSQL db;
    auto r=db.ejecutar("UPDATE public.facultad SET nombre=$2 WHERE id_facultad=$1 RETURNING id_facultad",{f.id,f.nombre});
    return PQntuples(r.get())==1;
}
bool RepositorioCatalogos::eliminarFacultad(const std::string& id) const {
    ConexionPostgreSQL db;
    // Nunca elimina en cascada: solo se permite sin carreras asociadas.
    auto r=db.ejecutar("DELETE FROM public.facultad f WHERE f.id_facultad=$1 "
        "AND NOT EXISTS (SELECT 1 FROM public.carrera c WHERE c.id_facultad=f.id_facultad) "
        "RETURNING id_facultad",{id});
    return PQntuples(r.get())==1;
}
std::vector<CarreraAcademica> RepositorioCatalogos::carreras() const {
    ConexionPostgreSQL db;
    auto r=db.ejecutar("SELECT c.id_carrera,c.nombre_carrera,c.modalidad_carrera,c.id_facultad,"
        "f.nombre,c.duracion_semestres FROM public.carrera c JOIN public.facultad f ON f.id_facultad=c.id_facultad "
        "ORDER BY c.nombre_carrera");
    std::vector<CarreraAcademica> out;
    for(int i=0;i<PQntuples(r.get());++i)
        out.push_back({c(r.get(),i,0),c(r.get(),i,1),c(r.get(),i,2),c(r.get(),i,3),c(r.get(),i,4),numero(r.get(),i,5)});
    return out;
}
bool RepositorioCatalogos::crearCarrera(const CarreraAcademica& a) const {
    ConexionPostgreSQL db;
    auto r=db.ejecutar("INSERT INTO public.carrera(id_carrera,nombre_carrera,duracion_semestres,modalidad_carrera,id_facultad) "
        "VALUES($1,$2,$3::integer,$4,$5) RETURNING id_carrera",
        {a.id,a.nombre,std::to_string(a.semestres),a.modalidad,a.idFacultad});
    return PQntuples(r.get())==1;
}
bool RepositorioCatalogos::editarCarrera(const CarreraAcademica& a) const {
    ConexionPostgreSQL db;
    auto r=db.ejecutar("UPDATE public.carrera SET nombre_carrera=$2,duracion_semestres=$3::integer,"
        "modalidad_carrera=$4,id_facultad=$5 WHERE id_carrera=$1 RETURNING id_carrera",
        {a.id,a.nombre,std::to_string(a.semestres),a.modalidad,a.idFacultad});
    return PQntuples(r.get())==1;
}
bool RepositorioCatalogos::actualizarPlan(const std::string& id,const std::string& nombre,int anio,bool vigente) const {
    ConexionPostgreSQL db;
    auto r=db.ejecutar("UPDATE public.plan_estudios SET nombre_plan=$2,anio_plan=$3::integer,"
        "vigente=$4::boolean WHERE id_plan=$1 RETURNING id_plan",
        {id,nombre,std::to_string(anio),vigente?"true":"false"});
    return PQntuples(r.get())==1;
}
std::vector<MateriaPlan> RepositorioCatalogos::asignaturasPlan(const std::string& id) const {
    ConexionPostgreSQL db;
    auto r=db.ejecutar("SELECT pa.id_ramo,r.nombre_ramo,pa.semestre,r.cantidad_creditos "
        "FROM public.plan_asignatura pa JOIN public.ramo r ON r.id_ramo=pa.id_ramo "
        "WHERE pa.id_plan=$1 ORDER BY pa.semestre,r.nombre_ramo",{id});
    std::vector<MateriaPlan> out;
    for(int i=0;i<PQntuples(r.get());++i)
        out.push_back({c(r.get(),i,0),c(r.get(),i,1),numero(r.get(),i,2),numero(r.get(),i,3)});
    return out;
}
bool RepositorioCatalogos::asociarAsignatura(const std::string& id,const std::string& ramo,int semestre) const {
    ConexionPostgreSQL db;
    // Verificar duración real de la carrera además del rango de semestre.
    auto r=db.ejecutar("INSERT INTO public.plan_asignatura(id_plan,id_ramo,semestre) "
        "SELECT p.id_plan, r.id_ramo,$3::integer FROM public.plan_estudios p "
        "JOIN public.carrera c ON c.id_carrera=p.id_carrera "
        "CROSS JOIN public.ramo r WHERE p.id_plan=$1 AND r.id_ramo=$2 "
        "AND (c.duracion_semestres IS NULL OR $3::integer<=c.duracion_semestres) "
        "ON CONFLICT(id_plan,id_ramo) DO UPDATE SET semestre=EXCLUDED.semestre RETURNING id_ramo",
        {id,ramo,std::to_string(semestre)});
    return PQntuples(r.get())==1;
}
bool RepositorioCatalogos::quitarAsignatura(const std::string& id,const std::string& ramo) const {
    ConexionPostgreSQL db;
    auto r=db.ejecutar("DELETE FROM public.plan_asignatura WHERE id_plan=$1 AND id_ramo=$2 RETURNING id_ramo",{id,ramo});
    return PQntuples(r.get())==1;
}
std::vector<PrerrequisitoPlan> RepositorioCatalogos::prerrequisitos(const std::string& id) const {
    ConexionPostgreSQL db;
    auto r=db.ejecutar("SELECT pr.id_ramo,ra.nombre_ramo,pr.id_prerrequisito,rb.nombre_ramo "
        "FROM public.asignatura_prerrequisito pr "
        "JOIN public.ramo ra ON ra.id_ramo=pr.id_ramo "
        "JOIN public.ramo rb ON rb.id_ramo=pr.id_prerrequisito "
        "WHERE pr.id_plan=$1 ORDER BY ra.nombre_ramo,rb.nombre_ramo",{id});
    std::vector<PrerrequisitoPlan> out;
    for(int i=0;i<PQntuples(r.get());++i)
        out.push_back({c(r.get(),i,0),c(r.get(),i,1),c(r.get(),i,2),c(r.get(),i,3)});
    return out;
}
bool RepositorioCatalogos::crearPrerrequisito(const std::string& id,const std::string& ramo,const std::string& previo) const {
    ConexionPostgreSQL db;
    // Restricciones en la migración verifican pertenencia al plan y semestre anterior.
    auto r=db.ejecutar("INSERT INTO public.asignatura_prerrequisito(id_plan,id_ramo,id_prerrequisito) "
        "VALUES($1,$2,$3) ON CONFLICT DO NOTHING RETURNING id_ramo",{id,ramo,previo});
    return PQntuples(r.get())==1;
}
bool RepositorioCatalogos::eliminarPrerrequisito(const std::string& id,const std::string& ramo,const std::string& previo) const {
    ConexionPostgreSQL db;
    auto r=db.ejecutar("DELETE FROM public.asignatura_prerrequisito "
        "WHERE id_plan=$1 AND id_ramo=$2 AND id_prerrequisito=$3 RETURNING id_ramo",{id,ramo,previo});
    return PQntuples(r.get())==1;
}
bool RepositorioCatalogos::definirCreditos(const std::string& id,int n) const {
    ConexionPostgreSQL db;
    auto r=db.ejecutar("UPDATE public.ramo SET cantidad_creditos=$2::integer "
        "WHERE id_ramo=$1 RETURNING id_ramo",{id,std::to_string(n)});
    return PQntuples(r.get())==1;
}
std::optional<FichaAcademica> RepositorioCatalogos::ficha(const std::string& rut) const {
    ConexionPostgreSQL db;
    auto r=db.ejecutar("SELECT e.rut,e.nombres,e.apellido_paterno,e.apellido_materno,"
        "e.correo_institucional,e.estado,e.id_carrera,c.nombre_carrera,f.nombre,"
        "COALESCE(ep.id_plan,''),COALESCE(p.nombre_plan,''),e.anio_ingreso::text "
        "FROM public.estudiante e JOIN public.carrera c ON c.id_carrera=e.id_carrera "
        "JOIN public.facultad f ON f.id_facultad=c.id_facultad "
        "LEFT JOIN public.estudiante_plan ep ON ep.rut=e.rut "
        "LEFT JOIN public.plan_estudios p ON p.id_plan=ep.id_plan "
        "WHERE e.rut=$1",{rut});
    if(PQntuples(r.get())==0)return std::nullopt;
    FichaAcademica f;
    f.rut=c(r.get(),0,0);f.nombres=c(r.get(),0,1);f.apellidoPaterno=c(r.get(),0,2);
    f.apellidoMaterno=c(r.get(),0,3);f.correo=c(r.get(),0,4);f.estado=c(r.get(),0,5);
    f.carreraId=c(r.get(),0,6);f.carreraNombre=c(r.get(),0,7);f.facultadNombre=c(r.get(),0,8);
    f.planId=c(r.get(),0,9);f.planNombre=c(r.get(),0,10);f.fechaIngreso=c(r.get(),0,11);
    if(!f.planId.empty())f.asignaturasPlan=asignaturasPlan(f.planId);
    return f;
}
