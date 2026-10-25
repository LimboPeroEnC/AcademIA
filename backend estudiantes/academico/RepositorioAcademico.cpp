#include "RepositorioAcademico.hpp"
#include "../database/ConexionPostgreSQL.hpp"
#include <string>

namespace {
std::string campo(PGresult* r,int i,int j) {
    return PQgetisnull(r,i,j) ? "" : std::string(PQgetvalue(r,i,j));
}
int entero(PGresult* r,int i,int j) {return std::stoi(campo(r,i,j));}
Asignatura parsearAsignatura(PGresult* r,int i) {
    return {campo(r,i,0),campo(r,i,1),campo(r,i,2),entero(r,i,3),entero(r,i,4)};
}
PlanEstudios parsearPlan(PGresult* r,int i) {
    return {campo(r,i,0),campo(r,i,1),campo(r,i,2),entero(r,i,3),campo(r,i,4)=="t"};
}
}
std::vector<Asignatura> RepositorioAcademico::listarAsignaturas(const std::string& busqueda) const {
    ConexionPostgreSQL db;
    auto r=db.ejecutar(
        "SELECT id_ramo,nombre_ramo,descripcion,cantidad_creditos,numero_semestre "
        "FROM public.ramo WHERE ($1='' OR id_ramo ILIKE '%'||$1||'%' "
        "OR nombre_ramo ILIKE '%'||$1||'%') "
        "ORDER BY numero_semestre,nombre_ramo LIMIT 200",{busqueda});
    std::vector<Asignatura> out;
    for(int i=0;i<PQntuples(r.get());++i)out.push_back(parsearAsignatura(r.get(),i));
    return out;
}
std::optional<Asignatura> RepositorioAcademico::obtenerAsignatura(const std::string& id) const {
    ConexionPostgreSQL db;
    auto r=db.ejecutar(
        "SELECT id_ramo,nombre_ramo,descripcion,cantidad_creditos,numero_semestre "
        "FROM public.ramo WHERE id_ramo=$1",{id});
    if(!PQntuples(r.get()))return std::nullopt;
    return parsearAsignatura(r.get(),0);
}
bool RepositorioAcademico::insertarAsignatura(const Asignatura& a) const {
    ConexionPostgreSQL db;
    auto r=db.ejecutar(
        "INSERT INTO public.ramo(id_ramo,nombre_ramo,descripcion,cantidad_creditos,numero_semestre) "
        "VALUES($1,$2,$3,$4::integer,$5::integer) RETURNING id_ramo",
        {a.idRamo,a.nombre,a.descripcion,std::to_string(a.creditos),std::to_string(a.semestreReferencia)});
    return PQntuples(r.get())==1;
}
bool RepositorioAcademico::modificarAsignatura(const std::string& id,const Asignatura& a) const {
    ConexionPostgreSQL db;
    auto r=db.ejecutar(
        "UPDATE public.ramo SET nombre_ramo=$2,descripcion=$3,cantidad_creditos=$4::integer, "
        "numero_semestre=$5::integer WHERE id_ramo=$1 RETURNING id_ramo",
        {id,a.nombre,a.descripcion,std::to_string(a.creditos),std::to_string(a.semestreReferencia)});
    return PQntuples(r.get())==1;
}
std::vector<PlanEstudios> RepositorioAcademico::listarPlanes(const std::string& carrera) const {
    ConexionPostgreSQL db;
    auto r=db.ejecutar(
        "SELECT id_plan,id_carrera,nombre_plan,anio_plan,vigente FROM public.plan_estudios "
        "WHERE ($1='' OR id_carrera=$1) ORDER BY anio_plan DESC,nombre_plan",{carrera});
    std::vector<PlanEstudios> out;
    for(int i=0;i<PQntuples(r.get());++i)out.push_back(parsearPlan(r.get(),i));
    return out;
}
bool RepositorioAcademico::crearPlan(const PlanEstudios& p) const {
    ConexionPostgreSQL db;
    auto r=db.ejecutar(
        "INSERT INTO public.plan_estudios(id_plan,id_carrera,nombre_plan,anio_plan,vigente) "
        "VALUES($1,$2,$3,$4::integer,$5::boolean) RETURNING id_plan",
        {p.idPlan,p.idCarrera,p.nombre,std::to_string(p.anio),p.vigente?"true":"false"});
    return PQntuples(r.get())==1;
}
std::vector<RamoMalla> RepositorioAcademico::malla(const std::string& idPlan) const {
    ConexionPostgreSQL db;
    auto r=db.ejecutar(
        "SELECT r.id_ramo,r.nombre_ramo,r.cantidad_creditos,pa.semestre "
        "FROM public.plan_asignatura pa JOIN public.ramo r ON r.id_ramo=pa.id_ramo "
        "WHERE pa.id_plan=$1 ORDER BY pa.semestre,r.nombre_ramo",{idPlan});
    std::vector<RamoMalla> out;
    for(int i=0;i<PQntuples(r.get());++i)
        out.push_back({campo(r.get(),i,0),campo(r.get(),i,1),entero(r.get(),i,2),entero(r.get(),i,3)});
    return out;
}
std::optional<EstudiantePlan> RepositorioAcademico::planDeEstudiante(const std::string& rut) const {
    ConexionPostgreSQL db;
    auto r=db.ejecutar(
        "SELECT ep.rut,ep.id_plan,p.nombre_plan,ep.id_carrera,c.nombre_carrera, "
        "ep.fecha_asignacion::text FROM public.estudiante_plan ep "
        "JOIN public.plan_estudios p ON p.id_plan=ep.id_plan "
        "JOIN public.carrera c ON c.id_carrera=ep.id_carrera WHERE ep.rut=$1",{rut});
    if(!PQntuples(r.get()))return std::nullopt;
    return EstudiantePlan{campo(r.get(),0,0),campo(r.get(),0,1),campo(r.get(),0,2),
                          campo(r.get(),0,3),campo(r.get(),0,4),campo(r.get(),0,5)};
}
bool RepositorioAcademico::asociarEstudiante(const std::string& rut,
    const std::string& carrera,const std::string& plan) const {
    ConexionPostgreSQL db;
    db.ejecutar("BEGIN");
    try {
        // La comprobación del plan/carrera y la escritura comparten la misma transacción.
        auto existe=db.ejecutar("SELECT 1 FROM public.plan_estudios "
            "WHERE id_plan=$1 AND id_carrera=$2 AND vigente=TRUE FOR SHARE",{plan,carrera});
        if(!PQntuples(existe.get())) {db.ejecutar("ROLLBACK");return false;}
        auto estudiante=db.ejecutar("UPDATE public.estudiante SET id_carrera=$2 "
            "WHERE rut=$1 RETURNING rut",{rut,carrera});
        if(!PQntuples(estudiante.get())) {db.ejecutar("ROLLBACK");return false;}
        db.ejecutar(
            "INSERT INTO public.estudiante_plan(rut,id_plan,id_carrera) VALUES($1,$2,$3) "
            "ON CONFLICT (rut) DO UPDATE SET id_plan=EXCLUDED.id_plan, "
            "id_carrera=EXCLUDED.id_carrera,fecha_asignacion=NOW()",
            {rut,plan,carrera});
        db.ejecutar("COMMIT");
        return true;
    } catch(...) {
        try { db.ejecutar("ROLLBACK"); } catch(...) {}
        throw;
    }
}
