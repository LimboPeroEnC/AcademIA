#include "../gestores/GestorEstudiantes.hpp"
#include "../academico/GestorAcademico.hpp"
#include "../academico/GestorCatalogos.hpp"
#include "../repositorios/RepositorioEstudiantes.hpp"
#include "../database/ConexionPostgreSQL.hpp"
#include "../util/ValidadorRut.hpp"
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/json.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <limits>
#include <stdexcept>
#include <map>

namespace asio=boost::asio;
namespace http=boost::beast::http;
namespace json=boost::json;
using tcp=asio::ip::tcp;

namespace {
std::string str(const json::object& o,const char* name) {
    if(auto* v=o.if_contains(name);v && v->is_string()) return std::string(v->as_string());
    return "";
}
json::object error(const std::string& msg) { return {{"error",msg}}; }
int numero(const json::object& o,const char* name) {
    auto* v=o.if_contains(name);
    if(!v || !v->is_int64())throw std::invalid_argument(std::string("Entero requerido: ")+name);
    auto n=v->as_int64();
    if(n<std::numeric_limits<int>::min()||n>std::numeric_limits<int>::max())
        throw std::invalid_argument("Numero fuera de rango");
    return static_cast<int>(n);
}
Asignatura extraerAsignatura(const json::object& o) {
    return {str(o,"id_ramo"),str(o,"nombre_ramo"),str(o,"descripcion"),
        numero(o,"cantidad_creditos"),numero(o,"numero_semestre")};
}
json::object serializar(const Asignatura& a) {
    return {{"id_ramo",a.idRamo},{"nombre_ramo",a.nombre},{"descripcion",a.descripcion},
        {"cantidad_creditos",a.creditos},{"numero_semestre",a.semestreReferencia}};
}
json::object serializar(const PlanEstudios& p) {
    return {{"id_plan",p.idPlan},{"id_carrera",p.idCarrera},{"nombre_plan",p.nombre},
        {"anio_plan",p.anio},{"vigente",p.vigente}};
}
json::object serializar(const EstudiantePlan& p) {
    return {{"rut",p.rut},{"id_plan",p.idPlan},{"nombre_plan",p.nombrePlan},
        {"id_carrera",p.idCarrera},{"nombre_carrera",p.nombreCarrera},
        {"fecha_asignacion",p.fechaAsignacion}};
}

json::object serializar(const FacultadAcademica& x) {
    return {{"id_facultad",x.id},{"nombre",x.nombre}};
}
json::object serializar(const CarreraAcademica& x) {
    return {{"id_carrera",x.id},{"nombre_carrera",x.nombre},
        {"modalidad_carrera",x.modalidad},{"id_facultad",x.idFacultad},
        {"nombre_facultad",x.nombreFacultad},{"duracion_semestres",x.semestres}};
}
json::object serializar(const MateriaPlan& x) {
    return {{"id_ramo",x.idRamo},{"nombre_ramo",x.nombre},{"semestre",x.semestre},{"cantidad_creditos",x.creditos}};
}
json::object serializar(const PrerrequisitoPlan& x) {
    return {{"id_ramo",x.idRamo},{"nombre_ramo",x.nombreRamo},
        {"id_prerrequisito",x.idRequisito},{"nombre_prerrequisito",x.nombreRequisito}};
}
json::object serializar(const FichaAcademica& x) {
    json::array materias;
    for(const auto& a:x.asignaturasPlan)materias.push_back(serializar(a));
    return {{"rut",x.rut},{"nombres",x.nombres},{"apellido_paterno",x.apellidoPaterno},
        {"apellido_materno",x.apellidoMaterno},{"correo_institucional",x.correo},
        {"estado",x.estado},{"id_carrera",x.carreraId},{"nombre_carrera",x.carreraNombre},
        {"nombre_facultad",x.facultadNombre},{"id_plan",x.planId},{"nombre_plan",x.planNombre},
        {"anio_ingreso",x.fechaIngreso},{"asignaturas_plan",std::move(materias)},
        {"nota","No hay matrículas, calificaciones o aprobaciones individuales en el esquema actual"}};
}
bool booleano(const json::object& o,const char* clave) {
    auto* v=o.if_contains(clave);
    if(!v||!v->is_bool())throw std::invalid_argument(std::string("Booleano requerido: ")+clave);
    return v->as_bool();
}

DatosEstudiante extraer(const json::object& o) {
    DatosEstudiante d;
    d.rut=str(o,"rut");d.nombres=str(o,"nombres");d.apellidoPaterno=str(o,"apellido_paterno");
    d.apellidoMaterno=str(o,"apellido_materno");d.fechaNacimiento=str(o,"fecha_nacimiento");
    d.direccion=str(o,"direccion_domicilio");d.comuna=str(o,"comuna_residencia");
    d.correoInstitucional=str(o,"correo_institucional");d.fechaIngreso=str(o,"anio_ingreso");
    d.genero=str(o,"genero");d.carreraId=str(o,"id_carrera");d.usuarioId=str(o,"id_usuario");
    if(d.fechaIngreso.size()>=4) d.anioIngreso=std::stoi(d.fechaIngreso.substr(0,4));
    return d;
}
json::object aJson(const Estudiante& e,const std::map<std::string,std::string>& carreras) {
    auto d=e.obtenerDatos();
    auto it=carreras.find(d.carreraId);
    return {{"rut",d.rut},{"nombres",d.nombres},{"apellido_paterno",d.apellidoPaterno},
        {"apellido_materno",d.apellidoMaterno},{"fecha_nacimiento",d.fechaNacimiento},
        {"direccion_domicilio",d.direccion},{"comuna_residencia",d.comuna},
        {"correo_institucional",d.correoInstitucional},{"anio_ingreso",d.fechaIngreso},
        {"genero",d.genero},{"id_carrera",d.carreraId},{"id_usuario",d.usuarioId},
        {"estado",e.obtenerEstado()},{"codigo_barra",e.obtenerCodigoBarra()},
        {"nombre_carrera",it==carreras.end()?d.carreraId:it->second}};
}
std::map<std::string,std::string> catalogo(const RepositorioEstudiantes& repo) {
    std::map<std::string,std::string> m;
    for(const auto& c:repo.carreras()) m[c.id]=c.nombre;
    return m;
}
std::string decode(const std::string& input) {
    std::string out;
    for(std::size_t i=0;i<input.size();i++) {
        if(input[i]=='+') out+=' ';
        else if(input[i]=='%' && i+2<input.size()) {
            auto h=[](char c)->int { if(c>='0'&&c<='9')return c-'0';if(c>='a'&&c<='f')return c-'a'+10;if(c>='A'&&c<='F')return c-'A'+10;return -1;};
            auto a=h(input[i+1]),b=h(input[i+2]);
            if(a>=0&&b>=0){out+=char(a*16+b);i+=2;} else out+=input[i];
        } else out+=input[i];
    }
    return out;
}
std::map<std::string,std::string> query(const std::string& target) {
    std::map<std::string,std::string> p;
    auto pos=target.find('?');if(pos==std::string::npos)return p;
    std::istringstream s(target.substr(pos+1));std::string pair;
    while(std::getline(s,pair,'&')) {
        auto eq=pair.find('=');
        p[decode(pair.substr(0,eq))]=eq==std::string::npos ? "" : decode(pair.substr(eq+1));
    }
    return p;
}
std::string param(const std::map<std::string,std::string>& m,const std::string& k) {
    auto it=m.find(k);return it==m.end()?"":it->second;
}
http::response<http::string_body> respuesta(http::status status,const json::value& data) {
    http::response<http::string_body> r{status,11};
    r.set(http::field::content_type,"application/json; charset=utf-8");
    r.set(http::field::cache_control,"no-store");
    r.body()=json::serialize(data);r.prepare_payload();return r;
}
http::response<http::string_body> archivo(const std::string& target) {
    static const std::map<std::string,std::string> permitidos = {
        {"/","index.html"},{"/index.html","index.html"},{"/registrar.html","registrar.html"},
        {"/buscar.html","buscar.html"},{"/modificar.html","modificar.html"},
        {"/historial.html","historial.html"},{"/css/styles.css","css/styles.css"},
        {"/js/estudiantes.js","js/estudiantes.js"},
        {"/js/academico.js","js/academico.js"},
        {"/asignaturas.html","asignaturas.html"},
        {"/malla.html","malla.html"},
        {"/asociar-plan.html","asociar-plan.html"},
        {"/estado.html","estado.html"},
        {"/facultades.html","facultades.html"},
        {"/carreras.html","carreras.html"},
        {"/planes.html","planes.html"},
        {"/asociar-asignaturas.html","asociar-asignaturas.html"},
        {"/prerrequisitos.html","prerrequisitos.html"},
        {"/creditos.html","creditos.html"},
        {"/ficha.html","ficha.html"},
        {"/js/restantes.js","js/restantes.js"}
    };
    auto it=permitidos.find(target);
    if(it==permitidos.end())return respuesta(http::status::not_found,error("Archivo no encontrado"));
    std::filesystem::path ruta=std::filesystem::path("frontend")/it->second;
    std::ifstream f(ruta,std::ios::binary);
    if(!f)return respuesta(http::status::internal_server_error,error("No se encontro el frontend. Ejecute el servidor desde la carpeta raiz del proyecto."));
    std::ostringstream contenido;contenido<<f.rdbuf();
    http::response<http::string_body> r{http::status::ok,11};
    r.set(http::field::content_type,target.find(".css")!=std::string::npos ? "text/css; charset=utf-8" :
       target.find(".js")!=std::string::npos ? "text/javascript; charset=utf-8" : "text/html; charset=utf-8");
    r.body()=contenido.str();r.prepare_payload();return r;
}
http::response<http::string_body> manejar(const http::request<http::string_body>& req) {
    auto target=std::string(req.target());
    auto p=target.substr(0,target.find('?'));
    try {
        RepositorioEstudiantes repo;
        GestorEstudiantes gestor;
        GestorAcademico academico;
        GestorCatalogos catalogos;
        if(req.method()==http::verb::get && p=="/api/estado") {
            ConexionPostgreSQL db;db.ejecutar("SELECT 1");
            return respuesta(http::status::ok,{{"conexion","correcta"},{"base_de_datos","disponible"}});
        }
        // AC-50: administracion de facultades
        if(p=="/api/facultades") {
            if(req.method()==http::verb::get) {
                json::array a;for(const auto& f:catalogos.facultades())a.push_back(serializar(f));
                return respuesta(http::status::ok,a);
            }
            if(req.method()==http::verb::post) {
                auto v=json::parse(req.body());if(!v.is_object())throw std::invalid_argument("JSON invalido");
                FacultadAcademica f{str(v.as_object(),"id_facultad"),str(v.as_object(),"nombre")};
                catalogos.crearFacultad(f);
                return respuesta(http::status::created,{{"mensaje","Facultad registrada"}});
            }
        }
        if(p.rfind("/api/facultades/",0)==0) {
            std::string id=decode(p.substr(16));
            if(req.method()==http::verb::put) {
                auto v=json::parse(req.body());if(!v.is_object())throw std::invalid_argument("JSON invalido");
                return catalogos.editarFacultad({id,str(v.as_object(),"nombre")})?
                    respuesta(http::status::ok,{{"mensaje","Facultad modificada"}}):
                    respuesta(http::status::not_found,error("Facultad no encontrada"));
            }
            if(req.method()==http::verb::delete_)return catalogos.eliminarFacultad(id)?
                respuesta(http::status::ok,{{"mensaje","Facultad eliminada"}}):
                respuesta(http::status::conflict,error("Facultad inexistente o con carreras relacionadas"));
        }
        // AC-51: carreras asociadas a facultades
        if(p=="/api/carreras") {
            if(req.method()==http::verb::get) {
                json::array a;for(const auto& c:catalogos.carreras()) a.push_back(serializar(c));
                return respuesta(http::status::ok,a);
            }
            if(req.method()==http::verb::post) {
                auto v=json::parse(req.body());if(!v.is_object())throw std::invalid_argument("JSON invalido");
                auto& o=v.as_object();
                CarreraAcademica c{str(o,"id_carrera"),str(o,"nombre_carrera"),str(o,"modalidad_carrera"),str(o,"id_facultad"),"",numero(o,"duracion_semestres")};
                catalogos.crearCarrera(c);
                return respuesta(http::status::created,{{"mensaje","Carrera registrada"}});
            }
        }
        if(p.rfind("/api/carreras/",0)==0 && req.method()==http::verb::put) {
            auto v=json::parse(req.body());if(!v.is_object())throw std::invalid_argument("JSON invalido");
            auto& o=v.as_object();
            CarreraAcademica c{decode(p.substr(14)),str(o,"nombre_carrera"),str(o,"modalidad_carrera"),str(o,"id_facultad"),"",numero(o,"duracion_semestres")};
            return catalogos.editarCarrera(c)?respuesta(http::status::ok,{{"mensaje","Carrera modificada"}}):
                respuesta(http::status::not_found,error("Carrera no encontrada"));
        }
        // AC-53: consulta, registro y edición de asignaturas (public.ramo).
        if(p=="/api/asignaturas") {
            if(req.method()==http::verb::get) {
                json::array a;
                for(const auto& ramo:academico.buscarAsignaturas(param(query(target),"q")))
                    a.push_back(serializar(ramo));
                return respuesta(http::status::ok,a);
            }
            if(req.method()==http::verb::post) {
                auto v=json::parse(req.body());if(!v.is_object())throw std::invalid_argument("JSON invalido");
                auto a=extraerAsignatura(v.as_object());
                academico.registrarAsignatura(a);
                return respuesta(http::status::created,{{"mensaje","Asignatura registrada"},{"id_ramo",a.idRamo}});
            }
        }
        // AC-56: editar creditos sin reemplazar los demas atributos del ramo.
        const std::string creditoFinal="/creditos";
        if(req.method()==http::verb::patch && p.rfind("/api/asignaturas/",0)==0 &&
           p.size()>creditoFinal.size() && p.substr(p.size()-creditoFinal.size())==creditoFinal) {
            auto id=decode(p.substr(17,p.size()-17-creditoFinal.size()));
            auto v=json::parse(req.body());if(!v.is_object())throw std::invalid_argument("JSON invalido");
            return catalogos.definirCreditos(id,numero(v.as_object(),"cantidad_creditos"))?
                respuesta(http::status::ok,{{"mensaje","Creditos actualizados"}}):
                respuesta(http::status::not_found,error("Asignatura no encontrada"));
        }
        const std::string rutaRamo="/api/asignaturas/";
        if(p.rfind(rutaRamo,0)==0) {
            const auto id=decode(p.substr(rutaRamo.size()));
            if(req.method()==http::verb::get) {
                auto a=academico.asignatura(id);
                return a?respuesta(http::status::ok,serializar(*a)):
                    respuesta(http::status::not_found,error("Asignatura no encontrada"));
            }
            if(req.method()==http::verb::put) {
                auto v=json::parse(req.body());if(!v.is_object())throw std::invalid_argument("JSON invalido");
                auto a=extraerAsignatura(v.as_object());
                return academico.modificarAsignatura(id,a)?
                    respuesta(http::status::ok,{{"mensaje","Asignatura modificada"}}):
                    respuesta(http::status::not_found,error("Asignatura no encontrada"));
            }
        }
        // AC-48 y AC-57: planes y malla curricular.
        if(p=="/api/planes") {
            if(req.method()==http::verb::get) {
                json::array a;
                for(const auto& plan:academico.planes(param(query(target),"carrera")))a.push_back(serializar(plan));
                return respuesta(http::status::ok,a);
            }
            if(req.method()==http::verb::post) {
                auto v=json::parse(req.body());if(!v.is_object())throw std::invalid_argument("JSON invalido");
                auto& o=v.as_object();
                PlanEstudios plan{str(o,"id_plan"),str(o,"id_carrera"),str(o,"nombre_plan"),
                    numero(o,"anio_plan"),true};
                academico.registrarPlan(plan);
                return respuesta(http::status::created,{{"mensaje","Plan registrado"},{"id_plan",plan.idPlan}});
            }
        }
        // AC-52: actualizar plan (no se modifica su carrera/ID con estudiantes asociados).
        if(p.rfind("/api/planes/",0)==0 && req.method()==http::verb::put &&
           p.find('/',12)==std::string::npos) {
            auto v=json::parse(req.body());if(!v.is_object())throw std::invalid_argument("JSON invalido");
            auto& o=v.as_object();
            return catalogos.actualizarPlan(decode(p.substr(12)),str(o,"nombre_plan"),numero(o,"anio_plan"),booleano(o,"vigente"))?
                respuesta(http::status::ok,{{"mensaje","Plan actualizado"}}):
                respuesta(http::status::not_found,error("Plan no encontrado"));
        }
        // AC-55/AC-54: las asignaturas y los prerrequisitos pertenecen al PLAN.
        if(p.rfind("/api/planes/",0)==0 && p.size()>12) {
            const auto resto=p.substr(12);
            const auto barra=resto.find('/');
            if(barra!=std::string::npos) {
                const std::string plan=decode(resto.substr(0,barra));
                const std::string sufijo=resto.substr(barra);
                if(sufijo=="/asignaturas") {
                    if(req.method()==http::verb::get) {
                        json::array a;for(const auto& x:catalogos.asignaturasPlan(plan))a.push_back(serializar(x));
                        return respuesta(http::status::ok,a);
                    }
                    if(req.method()==http::verb::post) {
                        auto v=json::parse(req.body());if(!v.is_object())throw std::invalid_argument("JSON invalido");
                        auto& o=v.as_object();
                        if(!catalogos.asociarAsignatura(plan,str(o,"id_ramo"),numero(o,"semestre")))
                            return respuesta(http::status::bad_request,error("Plan, ramo o semestre no valido para esta carrera"));
                        return respuesta(http::status::ok,{{"mensaje","Asignatura asociada al plan"}});
                    }
                }
                if(sufijo.rfind("/asignaturas/",0)==0 && req.method()==http::verb::delete_) {
                    return catalogos.quitarAsignatura(plan,decode(sufijo.substr(13)))?
                        respuesta(http::status::ok,{{"mensaje","Asignatura desasociada"}}):
                        respuesta(http::status::not_found,error("Asociacion no encontrada"));
                }
                if(sufijo=="/prerrequisitos") {
                    if(req.method()==http::verb::get) {
                        json::array a;for(const auto& x:catalogos.prerrequisitos(plan))a.push_back(serializar(x));
                        return respuesta(http::status::ok,a);
                    }
                    if(req.method()==http::verb::post) {
                        auto v=json::parse(req.body());if(!v.is_object())throw std::invalid_argument("JSON invalido");
                        auto& o=v.as_object();
                        return catalogos.crearPrerrequisito(plan,str(o,"id_ramo"),str(o,"id_prerrequisito"))?
                            respuesta(http::status::created,{{"mensaje","Prerrequisito registrado"}}):
                            respuesta(http::status::conflict,error("Prerrequisito ya existente"));
                    }
                }
                if(sufijo.rfind("/prerrequisitos/",0)==0 && req.method()==http::verb::delete_) {
                    std::string refs=sufijo.substr(16);auto sep=refs.find('/');
                    if(sep==std::string::npos)throw std::invalid_argument("Debe indicar ramo y prerrequisito");
                    return catalogos.eliminarPrerrequisito(plan,decode(refs.substr(0,sep)),decode(refs.substr(sep+1)))?
                        respuesta(http::status::ok,{{"mensaje","Prerrequisito eliminado"}}):
                        respuesta(http::status::not_found,error("Prerrequisito no encontrado"));
                }
            }
        }
        const std::string rutaMalla="/api/planes/";
        if(req.method()==http::verb::get && p.rfind(rutaMalla,0)==0 &&
           p.size()>rutaMalla.size()+6 && p.substr(p.size()-6)=="/malla") {
            auto id=decode(p.substr(rutaMalla.size(),p.size()-rutaMalla.size()-6));
            bool existe=false;
            for(const auto& plan:academico.planes(""))if(plan.idPlan==id){existe=true;break;}
            if(!existe)return respuesta(http::status::not_found,error("Plan no encontrado"));
            json::array lista;
            for(const auto& ramo:academico.visualizarMalla(id))lista.push_back({
                {"id_ramo",ramo.idRamo},{"nombre_ramo",ramo.nombre},
                {"cantidad_creditos",ramo.creditos},{"semestre",ramo.semestre}});
            return respuesta(http::status::ok,lista);
        }
        if(req.method()==http::verb::get && p=="/api/estudiantes") {
            auto q=query(target);
            FiltrosEstudiante f;f.rut=param(q,"rut");f.nombres=param(q,"nombre");f.carrera=param(q,"carrera");
            std::size_t pagina=1;
            if(!param(q,"pagina").empty()) {
                try { pagina=std::stoul(param(q,"pagina")); } catch(...) {return respuesta(http::status::bad_request,error("Pagina invalida"));}
                if(pagina<1||pagina>100000)return respuesta(http::status::bad_request,error("Pagina invalida"));
            }
            auto lista=gestor.buscarEstudiantes(f,pagina);auto nombres=catalogo(repo);json::array out;
            for(const auto& e:lista)out.push_back(aJson(e,nombres));
            return respuesta(http::status::ok,out);
        }
        constexpr const char* prefijo="/api/estudiantes/";
        if(p.rfind(prefijo,0)==0) {
            std::string resto=decode(p.substr(std::char_traits<char>::length(prefijo)));
            auto barra=resto.find('/');std::string rut=barra==std::string::npos?resto:resto.substr(0,barra);
            auto accion=barra==std::string::npos?"":resto.substr(barra);
            if(!ValidadorRut::validar(rut))return respuesta(http::status::bad_request,error("RUT invalido"));
            rut=ValidadorRut::normalizar(rut);
            if(req.method()==http::verb::get && accion=="/ficha") {
                auto ficha=catalogos.ficha(rut);
                return ficha?respuesta(http::status::ok,serializar(*ficha)):
                    respuesta(http::status::not_found,error("Estudiante no encontrado"));
            }
            if(req.method()==http::verb::get && accion=="/plan") {
                auto relacion=academico.consultarPlanEstudiante(rut);
                return relacion?respuesta(http::status::ok,serializar(*relacion)):
                    respuesta(http::status::not_found,error("Estudiante sin plan asociado"));
            }
            if(req.method()==http::verb::put && accion=="/plan") {
                auto v=json::parse(req.body());if(!v.is_object())throw std::invalid_argument("JSON invalido");
                const auto carrera=str(v.as_object(),"id_carrera");
                const auto plan=str(v.as_object(),"id_plan");
                return academico.asignarPlanEstudiante(rut,carrera,plan)?
                    respuesta(http::status::ok,{{"mensaje","Carrera y plan asociados"}}):
                    respuesta(http::status::bad_request,error("Estudiante inexistente, plan no vigente o plan no pertenece a la carrera"));
            }
            if(req.method()==http::verb::get && accion=="/historial") {
                json::array out;
                for(const auto& h:gestor.consultarHistorial(rut))out.push_back({
                    {"rut_estudiante",h.rutEstudiante},{"estado_anterior",h.estadoAnterior},
                    {"estado_nuevo",h.estadoNuevo},{"motivo",h.motivo},
                    {"usuario_responsable",h.usuarioResponsable},{"fecha_cambio",(std::int64_t)h.fechaCambio}});
                return respuesta(http::status::ok,out);
            }
            if(req.method()==http::verb::get && accion.empty()) {
                auto encontrado=repo.obtenerPorRut(rut);
                if(encontrado)return respuesta(http::status::ok,aJson(*encontrado,catalogo(repo)));
                return respuesta(http::status::not_found,error("Estudiante no encontrado"));
            }
            if(req.method()==http::verb::put && accion.empty()) {
                auto v=json::parse(req.body()); if(!v.is_object())return respuesta(http::status::bad_request,error("JSON invalido"));
                auto d=extraer(v.as_object());
                if(gestor.editarEstudiante(rut,d))return respuesta(http::status::ok,{{"mensaje","Estudiante actualizado"}});
                return respuesta(http::status::bad_request,error("Datos invalidos, duplicados o estudiante inexistente"));
            }
            if(req.method()==http::verb::post && accion=="/estado") {
                auto v=json::parse(req.body()); if(!v.is_object())return respuesta(http::status::bad_request,error("JSON invalido"));
                auto& o=v.as_object();
                if(gestor.cambiarEstado(rut,str(o,"estado"),str(o,"motivo"),str(o,"usuario_responsable")))
                    return respuesta(http::status::ok,{{"mensaje","Cambio registrado en historial"}});
                return respuesta(http::status::bad_request,error("Estado igual, estudiante inexistente o motivo/responsable invalidos"));
            }
        }
        if(req.method()==http::verb::post && p=="/api/estudiantes") {
            auto v=json::parse(req.body());if(!v.is_object())return respuesta(http::status::bad_request,error("JSON invalido"));
            auto& o=v.as_object();
            if(str(o,"estado")!="" && str(o,"estado")!="Activo")
                return respuesta(http::status::bad_request,error("El registro inicial debe ser Activo; use Cambiar estado posteriormente."));
            auto d=extraer(o);
            if(gestor.registrarEstudiante(d))return respuesta(http::status::created,{{"mensaje","Estudiante registrado"},{"rut",ValidadorRut::normalizar(d.rut)}});
            return respuesta(http::status::bad_request,error("Verifique RUT, campos obligatorios y duplicados"));
        }
        if(req.method()==http::verb::get) return archivo(p);
        return respuesta(http::status::not_found,error("Ruta desconocida"));
    } catch(const ErrorBaseDatos& e) {
        std::cerr<<"PostgreSQL: "<<e.what()<<" ("<<e.codigoSQL<<")\n";
        auto httpCode=e.codigoSQL.rfind("08",0)==0?http::status::service_unavailable:
          e.codigoSQL=="23505"?http::status::conflict:
          (e.codigoSQL=="23503"||e.codigoSQL=="23514"||e.codigoSQL.rfind("22",0)==0)?http::status::bad_request:http::status::internal_server_error;
        return respuesta(httpCode,error(e.codigoSQL=="08006"?"No se pudo conectar a PostgreSQL. Verifica host, puerto y contrasena":
            (httpCode==http::status::internal_server_error?"Error de base de datos. Revisa la consola del servidor.":e.what())));
    } catch(const std::exception& e) {
        std::cerr<<"Error: "<<e.what()<<"\n";
        return respuesta(http::status::bad_request,error("Solicitud invalida o datos incorrectos"));
    }
}
}
int main() {
    try {
        asio::io_context ioc;
        int port=8080;
        if(auto p=std::getenv("API_PORT"))port=std::stoi(p);
        if(port<1||port>65535)throw std::runtime_error("Puerto invalido");
        tcp::acceptor acceptor(ioc, {asio::ip::make_address("127.0.0.1"), static_cast<unsigned short>(port)});
        std::cout<<"Servidor C++ listo en http://127.0.0.1:"<<port<<"\n";
        std::cout<<"Inicie desde la carpeta raiz (debe contener frontend/).\n";
        for(;;) {
            tcp::socket socket(ioc);acceptor.accept(socket);
            try {
                boost::beast::flat_buffer buffer;
                http::request_parser<http::string_body> parser;
                parser.body_limit(32768);
                http::read(socket,buffer,parser);
                auto response=manejar(parser.get());
                response.keep_alive(false);
                http::write(socket,response);
                boost::system::error_code ec;socket.shutdown(tcp::socket::shutdown_both,ec);
            } catch(const std::exception& e) {std::cerr<<"HTTP: "<<e.what()<<"\n";}
        }
    }catch(const std::exception& e){std::cerr<<"No se pudo iniciar servidor: "<<e.what()<<"\n";return 1;}
}
