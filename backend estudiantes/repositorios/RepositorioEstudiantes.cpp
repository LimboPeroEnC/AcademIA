#include "RepositorioEstudiantes.hpp"
#include "../database/ConexionPostgreSQL.hpp"
#include <algorithm>
#include <ctime>
#include <iomanip>
#include <sstream>

using Param = std::optional<std::string>;
namespace {
  std::string campo(PGresult* r, int row, int col) {
    return PQgetisnull(r, row, col) ? "" : PQgetvalue(r, row, col);
  }
  const std::string campos =
    " e.rut, e.nombres, e.apellido_paterno, e.apellido_materno, "
    " e.fecha_nacimiento::text, e.direccion_domicilio, e.comuna_residencia, "
    " e.correo_institucional, e.anio_ingreso::text, e.genero, e.id_carrera, "
    " COALESCE(e.id_usuario, ''), COALESCE(e.estado, 'Activo') ";
  const std::string tablas = " FROM public.estudiante e JOIN public.carrera c ON c.id_carrera=e.id_carrera ";
  std::vector<Estudiante> parsear(PGresult* r) {
      std::vector<Estudiante> out;
      for (int i=0; i<PQntuples(r); i++) {
          DatosEstudiante d;
          d.rut=campo(r,i,0); d.nombres=campo(r,i,1);
          d.apellidoPaterno=campo(r,i,2); d.apellidoMaterno=campo(r,i,3);
          d.fechaNacimiento=campo(r,i,4); d.direccion=campo(r,i,5);
          d.comuna=campo(r,i,6); d.correoInstitucional=campo(r,i,7);
          d.fechaIngreso=campo(r,i,8); d.anioIngreso=std::stoi(d.fechaIngreso.substr(0,4));
          d.genero=campo(r,i,9); d.carreraId=campo(r,i,10); d.usuarioId=campo(r,i,11);
          Estudiante e(d, "EST-" + d.rut);
          e.establecerEstado(campo(r,i,12));
          out.push_back(e);
      }
      return out;
  }
  Param usuario(const std::string& id) { return id.empty() ? std::nullopt : Param(id); }
  std::string fecha(const DatosEstudiante& d) {
      if (!d.fechaIngreso.empty()) return d.fechaIngreso;
      // Para llamadas existentes que solo entregan el anioIngreso.
      return std::to_string(d.anioIngreso) + "-01-01";
  }
}

bool RepositorioEstudiantes::existeRut(const std::string& rut) const {
    ConexionPostgreSQL db;
    auto r=db.ejecutar("SELECT 1 FROM public.estudiante WHERE rut=$1 LIMIT 1", {rut});
    return PQntuples(r.get())>0;
}
bool RepositorioEstudiantes::existeCorreo(const std::string& correo,const std::string& excluir) const {
    ConexionPostgreSQL db;
    auto r=db.ejecutar("SELECT 1 FROM public.estudiante WHERE lower(correo_institucional)=lower($1) AND rut<>$2 LIMIT 1",{correo,excluir});
    return PQntuples(r.get())>0;
}
bool RepositorioEstudiantes::insertar(const Estudiante& e) const {
    auto d=e.obtenerDatos(); ConexionPostgreSQL db;
    auto r=db.ejecutar(
      "INSERT INTO public.estudiante (rut,nombres,apellido_paterno,apellido_materno,fecha_nacimiento,"
      "direccion_domicilio,comuna_residencia,correo_institucional,anio_ingreso,genero,id_carrera,id_usuario,estado)"
      " VALUES ($1,$2,$3,$4,$5::date,$6,$7,$8,$9::date,$10,$11,$12,$13) RETURNING rut",
      {d.rut,d.nombres,d.apellidoPaterno,d.apellidoMaterno,d.fechaNacimiento,d.direccion,d.comuna,
       d.correoInstitucional,fecha(d),d.genero,d.carreraId,usuario(d.usuarioId),e.obtenerEstado()});
    return PQntuples(r.get())==1;
}
std::optional<Estudiante> RepositorioEstudiantes::obtenerPorRut(const std::string& rut) const {
    ConexionPostgreSQL db;
    auto r = db.ejecutar("SELECT " + campos + tablas + " WHERE e.rut=$1", {rut});
    auto lista = parsear(r.get());
    if (lista.empty()) return std::nullopt;
    return lista.front();
}
std::vector<Estudiante> RepositorioEstudiantes::buscarGeneral(const std::string& criterio) const {
    ConexionPostgreSQL db;
    // Sin criterio se listan 50, para no entregar listas ilimitadas.
    auto r=db.ejecutar("SELECT " + campos + tablas +
      " WHERE ($1='' OR e.rut ILIKE '%'||$1||'%' OR e.nombres ILIKE '%'||$1||'%' "
      " OR e.apellido_paterno ILIKE '%'||$1||'%' OR e.apellido_materno ILIKE '%'||$1||'%' "
      " OR e.correo_institucional ILIKE '%'||$1||'%' OR e.id_carrera ILIKE '%'||$1||'%' "
      " OR c.nombre_carrera ILIKE '%'||$1||'%') "
      " ORDER BY e.apellido_paterno, e.nombres LIMIT 50", {criterio});
    return parsear(r.get());
}
std::vector<Estudiante> RepositorioEstudiantes::buscarAvanzada(const FiltrosEstudiante& f,std::size_t pagina) const {
    if (!pagina) return {};
    ConexionPostgreSQL db;
    auto r=db.ejecutar("SELECT " + campos + tablas +
      " WHERE ($1='' OR e.rut ILIKE '%'||$1||'%') "
      " AND ($2='' OR e.nombres ILIKE '%'||$2||'%' OR e.apellido_paterno ILIKE '%'||$2||'%' OR e.apellido_materno ILIKE '%'||$2||'%') "
      " AND ($3='' OR e.apellido_paterno ILIKE '%'||$3||'%') "
      " AND ($4='' OR e.apellido_materno ILIKE '%'||$4||'%') "
      " AND ($5='' OR ('EST-'||e.rut) ILIKE '%'||$5||'%') "
      " AND ($6='' OR e.estado=$6) "
      " AND ($7='' OR e.id_carrera ILIKE '%'||$7||'%' OR c.nombre_carrera ILIKE '%'||$7||'%') "
      " ORDER BY e.apellido_paterno, e.nombres LIMIT 50 OFFSET $8::bigint",
      {f.rut,f.nombres,f.apellidoPaterno,f.apellidoMaterno,f.codigoBarra,f.estado,f.carrera,
       std::to_string((pagina-1)*50)});
    return parsear(r.get());
}
bool RepositorioEstudiantes::actualizar(const std::string& rut,const DatosEstudiante& d) const {
    ConexionPostgreSQL db;
    auto r=db.ejecutar(
      "UPDATE public.estudiante SET nombres=$2,apellido_paterno=$3,apellido_materno=$4,"
      "fecha_nacimiento=$5::date,direccion_domicilio=$6,comuna_residencia=$7,"
      "correo_institucional=$8,anio_ingreso=$9::date,genero=$10,id_carrera=$11,id_usuario=$12 "
      "WHERE rut=$1 RETURNING rut",
      {rut,d.nombres,d.apellidoPaterno,d.apellidoMaterno,d.fechaNacimiento,d.direccion,d.comuna,
       d.correoInstitucional,fecha(d),d.genero,d.carreraId,usuario(d.usuarioId)});
    return PQntuples(r.get())==1;
}
bool RepositorioEstudiantes::cambiarEstado(const std::string& rut, const std::string& nuevoEstado,
    const std::string& motivo,const std::string& responsable) const {
    ConexionPostgreSQL db;
    db.ejecutar("BEGIN");
    try {
        auto antes = db.ejecutar("SELECT estado FROM public.estudiante WHERE rut=$1 FOR UPDATE", {rut});
        if (!PQntuples(antes.get())) { db.ejecutar("ROLLBACK"); return false; }
        auto viejo = campo(antes.get(),0,0);
        if (viejo==nuevoEstado) { db.ejecutar("ROLLBACK"); return false; }
        db.ejecutar("UPDATE public.estudiante SET estado=$2 WHERE rut=$1",{rut,nuevoEstado});
        db.ejecutar("INSERT INTO public.historial_estado_estudiante"
            " (rut_estudiante,estado_anterior,estado_nuevo,motivo,usuario_responsable)"
            " VALUES ($1,$2,$3,$4,$5)", {rut,viejo,nuevoEstado,motivo,responsable});
        db.ejecutar("COMMIT"); return true;
    } catch (...) { try { db.ejecutar("ROLLBACK"); } catch (...) {} throw; }
}
std::vector<HistorialEstado> RepositorioEstudiantes::historial(const std::string& rut) const {
    ConexionPostgreSQL db;
    auto r=db.ejecutar(
      "SELECT rut_estudiante,estado_anterior,estado_nuevo,motivo,usuario_responsable,"
      " EXTRACT(EPOCH FROM fecha_cambio)::bigint FROM public.historial_estado_estudiante"
      " WHERE rut_estudiante=$1 ORDER BY fecha_cambio DESC",{rut});
    std::vector<HistorialEstado> salida;
    for (int i=0;i<PQntuples(r.get());i++) {
        HistorialEstado h;
        h.rutEstudiante=campo(r.get(),i,0);h.estadoAnterior=campo(r.get(),i,1);
        h.estadoNuevo=campo(r.get(),i,2);h.motivo=campo(r.get(),i,3);
        h.usuarioResponsable=campo(r.get(),i,4);h.fechaCambio=(std::time_t)std::stoll(campo(r.get(),i,5));
        salida.push_back(h);
    }
    return salida;
}
std::vector<CarreraBasica> RepositorioEstudiantes::carreras() const {
    ConexionPostgreSQL db;
    auto r=db.ejecutar("SELECT id_carrera,nombre_carrera FROM public.carrera ORDER BY nombre_carrera");
    std::vector<CarreraBasica> out;
    for(int i=0;i<PQntuples(r.get());i++) out.push_back({campo(r.get(),i,0),campo(r.get(),i,1)});
    return out;
}
