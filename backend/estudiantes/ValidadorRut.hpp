
#ifndef VALIDADOR_RUT_H
#define VALIDADOR_RUT_H

#include <string>

class ValidadorRut {
public:
    static bool validar(const std::string& rut);
    static std::string normalizar(const std::string& rut);
};

#endif
