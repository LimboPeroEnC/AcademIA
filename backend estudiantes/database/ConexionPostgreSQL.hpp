#pragma once
#include <libpq-fe.h>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

class ErrorBaseDatos : public std::runtime_error {
public:
    std::string codigoSQL;
    ErrorBaseDatos(const std::string& mensaje, const std::string& codigo)
      : std::runtime_error(mensaje), codigoSQL(codigo) {}
};

class ConexionPostgreSQL {
    PGconn* conexion_ = nullptr;
public:
    using Resultado = std::unique_ptr<PGresult, decltype(&PQclear)>;
    ConexionPostgreSQL();
    ~ConexionPostgreSQL();
    ConexionPostgreSQL(const ConexionPostgreSQL&) = delete;
    ConexionPostgreSQL& operator=(const ConexionPostgreSQL&) = delete;
    Resultado ejecutar(const std::string& sql,
        const std::vector<std::optional<std::string>>& parametros = {});
};
