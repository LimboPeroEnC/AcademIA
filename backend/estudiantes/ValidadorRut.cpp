
#include "ValidadorRut.hpp"

#include <algorithm>
#include <cctype>

std::string ValidadorRut::normalizar(
    const std::string& rut
) {
    std::string resultado;

    for (unsigned char caracter : rut) {
        if (caracter != '.' &&
            caracter != '-' &&
            !std::isspace(caracter)) {

            resultado += static_cast<char>(
                std::toupper(caracter)
            );
        }
    }

    return resultado;
}

bool ValidadorRut::validar(
    const std::string& rut
) {
    std::string limpio = normalizar(rut);

    if (limpio.length() < 2 ||
        limpio.length() > 9) {
        return false;
    }

    for (size_t i = 0; i < limpio.length() - 1; i++) {
        if (!std::isdigit(
            static_cast<unsigned char>(limpio[i])
        )) {
            return false;
        }
    }

    char verificador = limpio.back();

    if (!std::isdigit(
            static_cast<unsigned char>(verificador)
        ) && verificador != 'K') {
        return false;
    }

    int suma = 0;
    int multiplicador = 2;
    bool tieneDigitoDistintoDeCero = false;

    for (int i = static_cast<int>(limpio.length()) - 2;
         i >= 0;
         i--) {

        int digito = limpio[i] - '0';

        if (digito != 0) {
            tieneDigitoDistintoDeCero = true;
        }

        suma += digito * multiplicador;

        multiplicador++;

        if (multiplicador > 7) {
            multiplicador = 2;
        }
    }

    if (!tieneDigitoDistintoDeCero) {
        return false;
    }

    int resultado = 11 - (suma % 11);

    char digitoEsperado;

    if (resultado == 11) {
        digitoEsperado = '0';
    } else if (resultado == 10) {
        digitoEsperado = 'K';
    } else {
        digitoEsperado = static_cast<char>(
            '0' + resultado
        );
    }

    return verificador == digitoEsperado;
}
