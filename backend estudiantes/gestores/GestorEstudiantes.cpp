#include "GestorEstudiantes.hpp"
#include "../repositorios/RepositorioEstudiantes.hpp"
#include "../util/ValidadorRut.hpp"
#include <cctype>
#include <ctime>
#include <stdexcept>

namespace {
 bool vacio(const std::string& s) { return s.find_first_not_of(" \t\n\r") == std::string::npos; }
 int anioActual() { std::time_t t=std::time(nullptr); std::tm f{};
 #ifdef _WIN32
 localtime_s(&f,&t);
 #else
 localtime_r(&t,&f);
 #endif
 return f.tm_year+1900; }
 bool fechaISO(const std::string& s) {
   if (s.size() != 10 || s[4] != '-' || s[7] != '-') return false;
   for (std::size_t i = 0; i < s.size(); ++i)
     if (i != 4 && i != 7 && !std::isdigit(static_cast<unsigned char>(s[i]))) return false;
   const int y = std::stoi(s.substr(0, 4));
   const int m = std::stoi(s.substr(5, 2));
   const int d = std::stoi(s.substr(8, 2));
   if (y < 1 || m < 1 || m > 12) return false;
   const bool bisiesto = y % 4 == 0 && (y % 100 != 0 || y % 400 == 0);
   const int diasMes[12] = {31, bisiesto ? 29 : 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
   return d >= 1 && d <= diasMes[m-1];
 }
 bool datosValidos(const DatosEstudiante& d) {
   if(vacio(d.rut)||vacio(d.nombres)||vacio(d.apellidoPaterno)||vacio(d.apellidoMaterno)||
      vacio(d.fechaNacimiento)||vacio(d.direccion)||vacio(d.comuna)||vacio(d.correoInstitucional)||
      vacio(d.genero)||vacio(d.carreraId)||!fechaISO(d.fechaNacimiento)) return false;
   if(!d.fechaIngreso.empty()) return fechaISO(d.fechaIngreso) &&
       std::stoi(d.fechaIngreso.substr(0,4))>=1900 && std::stoi(d.fechaIngreso.substr(0,4))<=anioActual();
   return d.anioIngreso>=1900 && d.anioIngreso<=anioActual();
 }
}
GestorEstudiantes::GestorEstudiantes():repositorio_(std::make_shared<RepositorioEstudiantes>()){}
GestorEstudiantes::GestorEstudiantes(std::shared_ptr<RepositorioEstudiantes> r):repositorio_(std::move(r)) {
    if(!repositorio_) throw std::invalid_argument("Repositorio requerido");
}
bool GestorEstudiantes::existeRut(const std::string& rut) const {
    return repositorio_->existeRut(ValidadorRut::normalizar(rut));
}
bool GestorEstudiantes::existeCorreo(const std::string& correo,const std::string& excl) const {
    return repositorio_->existeCorreo(correo,ValidadorRut::normalizar(excl));
}
bool GestorEstudiantes::registrarEstudiante(const DatosEstudiante& datos) {
    if(!datosValidos(datos)||!ValidadorRut::validar(datos.rut)) return false;
    DatosEstudiante d=datos;d.rut=ValidadorRut::normalizar(datos.rut);
    if(existeRut(d.rut)||existeCorreo(d.correoInstitucional)) return false;
    return repositorio_->insertar(Estudiante(d,"EST-"+d.rut));
}
std::vector<Estudiante> GestorEstudiantes::buscarEstudiantes(const std::string& criterio) const {
    return repositorio_->buscarGeneral(criterio);
}
std::vector<Estudiante> GestorEstudiantes::buscarEstudiantes(const FiltrosEstudiante& f,std::size_t pagina) const {
    return repositorio_->buscarAvanzada(f,pagina);
}
bool GestorEstudiantes::editarEstudiante(const std::string& rut,const DatosEstudiante& datos) {
    const auto r=ValidadorRut::normalizar(rut);
    if(!ValidadorRut::validar(r)||ValidadorRut::normalizar(datos.rut)!=r||!datosValidos(datos)) return false;
    if(existeCorreo(datos.correoInstitucional,r)) return false;
    return repositorio_->actualizar(r,datos);
}
bool GestorEstudiantes::cambiarEstado(const std::string& rut,const std::string& estado,
    const std::string& motivo,const std::string& usuario) {
    if(!ValidadorRut::validar(rut)||vacio(motivo)||vacio(usuario)||
       (estado!="Activo"&&estado!="Inactivo")) return false;
    return repositorio_->cambiarEstado(ValidadorRut::normalizar(rut),estado,motivo,usuario);
}
std::vector<HistorialEstado> GestorEstudiantes::consultarHistorial(const std::string& rut) const {
    return repositorio_->historial(ValidadorRut::normalizar(rut));
}
