#include "ConexionPostgreSQL.hpp"
#include <cstdlib>

namespace {
  const char* envOr(const char* name, const char* fallback) {
      const char* v = std::getenv(name);
      return v && *v ? v : fallback;
  }
}
ConexionPostgreSQL::ConexionPostgreSQL() {
    const char* keys[] = {"host", "port", "dbname", "user", "password", "connect_timeout", nullptr};
    const char* values[] = {envOr("DB_HOST", "127.0.0.1"), envOr("DB_PORT", "5432"),
        envOr("DB_NAME", "DBAcademiaUltimate"), envOr("DB_USER", "postgres"),
        envOr("DB_PASSWORD", ""), "5", nullptr};
    conexion_ = PQconnectdbParams(keys, values, 0);
    if (!conexion_ || PQstatus(conexion_) != CONNECTION_OK) {
        std::string error = conexion_ ? PQerrorMessage(conexion_) : "Sin memoria para abrir conexion";
        if (conexion_) PQfinish(conexion_);
        conexion_ = nullptr;
        throw ErrorBaseDatos("No se pudo conectar a PostgreSQL: " + error, "08006");
    }
}
ConexionPostgreSQL::~ConexionPostgreSQL() { if (conexion_) PQfinish(conexion_); }
ConexionPostgreSQL::Resultado ConexionPostgreSQL::ejecutar(
    const std::string& sql, const std::vector<std::optional<std::string>>& parametros) {
    std::vector<const char*> valores;
    valores.reserve(parametros.size());
    for (const auto& p : parametros) valores.push_back(p ? p->c_str() : nullptr);
    Resultado resultado(PQexecParams(conexion_, sql.c_str(), (int)valores.size(), nullptr,
        valores.empty() ? nullptr : valores.data(), nullptr, nullptr, 0), &PQclear);
    if (!resultado) throw ErrorBaseDatos(PQerrorMessage(conexion_), "XX000");
    auto status = PQresultStatus(resultado.get());
    if (status != PGRES_TUPLES_OK && status != PGRES_COMMAND_OK) {
        const char* code = PQresultErrorField(resultado.get(), PG_DIAG_SQLSTATE);
        std::string codigo = code ? code : "XX000";
        std::string mensaje;
        if (codigo == "23505") mensaje = "Ya existe un registro con la misma clave o valor unico.";
        else if (codigo == "23503") mensaje = "No existe un registro relacionado (carrera, estudiante, plan o ramo).";
        else if (codigo == "23514") mensaje = "Restriccion invalida (revisa que el prerrequisito pertenezca al plan y sea de un semestre anterior).";
        else if (codigo == "22007" || codigo == "22008") mensaje = "Formato de fecha incorrecto.";
        else mensaje = PQresultErrorMessage(resultado.get());
        throw ErrorBaseDatos(mensaje, codigo);
    }
    return resultado;
}
