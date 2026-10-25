#include "ValidadorRut.hpp"
#include <algorithm>
#include <cctype>
std::string ValidadorRut::normalizar(const std::string& rut) {
    std::string out;
    for (unsigned char c : rut) {
        if (c != '.' && c != '-' && !std::isspace(c)) out += (char)std::toupper(c);
    }
    if (out.size() < 2) return out;
    return out.substr(0, out.size()-1) + "-" + out.substr(out.size()-1);
}
bool ValidadorRut::validar(const std::string& rut) {
    auto n = normalizar(rut);
    auto guion = n.find('-');
    if (guion == std::string::npos || guion < 1 || guion > 8 || n.size() != guion + 2) return false;
    int suma = 0, factor = 2;
    for (std::size_t i = guion; i-- > 0;) {
        if (!std::isdigit(static_cast<unsigned char>(n[i]))) return false;
        suma += (n[i] - '0') * factor;
        factor = factor == 7 ? 2 : factor+1;
    }
    int resto = 11 - (suma % 11);
    char digito = resto == 11 ? '0' : resto == 10 ? 'K' : char('0'+resto);
    return n.back() == digito;
}
