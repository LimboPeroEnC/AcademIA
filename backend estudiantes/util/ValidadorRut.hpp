#pragma once
#include <string>
class ValidadorRut {
public:
    static std::string normalizar(const std::string& rut);
    static bool validar(const std::string& rut);
};
