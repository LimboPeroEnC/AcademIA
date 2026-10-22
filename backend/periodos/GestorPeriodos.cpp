
#include "GestorPeriodos.hpp"

#include <cctype>
#include <string>
#include <vector>

// ==========================================
// FUNCIONES AUXILIARES
// ==========================================

namespace {

    bool estaVacio(const std::string& texto) {
        return texto.find_first_not_of(" \t\n\r")
            == std::string::npos;
    }

    bool esBisiesto(int anio) {
        return (anio % 400 == 0) ||
               (anio % 4 == 0 && anio % 100 != 0);
    }

    bool fechaValida(const std::string& fecha) {

        // Formato obligatorio: AAAA-MM-DD
        if (fecha.length() != 10 ||
            fecha[4] != '-' ||
            fecha[7] != '-') {
            return false;
        }

        for (std::size_t i = 0; i < fecha.length(); i++) {
            if (i == 4 || i == 7) {
                continue;
            }

            if (!std::isdigit(
                    static_cast<unsigned char>(fecha[i])
                )) {
                return false;
            }
        }

        int anio = std::stoi(fecha.substr(0, 4));
        int mes = std::stoi(fecha.substr(5, 2));
        int dia = std::stoi(fecha.substr(8, 2));

        if (anio < 1 || mes < 1 || mes > 12) {
            return false;
        }

        const int diasPorMes[12] = {
            31, 28, 31, 30, 31, 30,
            31, 31, 30, 31, 30, 31
        };

        int maxDias = diasPorMes[mes - 1];

        if (mes == 2 && esBisiesto(anio)) {
            maxDias = 29;
        }

        return dia >= 1 && dia <= maxDias;
    }
}

// ==========================================
// VERIFICAR CODIGO DUPLICADO
// ==========================================

bool GestorPeriodos::existeCodigo(
    const std::string& codigo
) const {

    for (const PeriodoAcademico& periodo : periodos) {

        if (periodo.obtenerDatos().codigo == codigo) {
            return true;
        }
    }

    return false;
}

// ==========================================
// VERIFICAR PERIODO DUPLICADO
// ==========================================

bool GestorPeriodos::existePeriodo(
    int anio,
    int semestre,
    const std::string& codigoExcluir
) const {

    for (const PeriodoAcademico& periodo : periodos) {

        const DatosPeriodoAcademico& datos =
            periodo.obtenerDatos();

        if (
            datos.codigo != codigoExcluir &&
            datos.anio == anio &&
            datos.semestre == semestre
        ) {
            return true;
        }
    }

    return false;
}

// ==========================================
// VALIDAR DATOS
// ==========================================

bool GestorPeriodos::validarDatos(
    const DatosPeriodoAcademico& datos
) const {

    if (estaVacio(datos.codigo) ||
        estaVacio(datos.nombre)) {
        return false;
    }

    if (datos.anio < 1 || datos.anio > 9999) {
        return false;
    }

    // Por ahora consideramos dos semestres al anio
    if (datos.semestre != 1 && datos.semestre != 2) {
        return false;
    }

    if (!fechaValida(datos.fechaInicio) ||
        !fechaValida(datos.fechaFin)) {
        return false;
    }

    // Formato ISO: permite comparar fechas como texto
    if (datos.fechaInicio >= datos.fechaFin) {
        return false;
    }

    return true;
}

// ==========================================
// AC-58: CREAR PERIODO ACADEMICO
// ==========================================

bool GestorPeriodos::crearPeriodo(
    const DatosPeriodoAcademico& datos
) {

    if (!validarDatos(datos)) {
        return false;
    }

    if (existeCodigo(datos.codigo)) {
        return false;
    }

    if (existePeriodo(datos.anio, datos.semestre)) {
        return false;
    }

    PeriodoAcademico nuevoPeriodo(datos);

    periodos.push_back(nuevoPeriodo);

    return true;
}

// ==========================================
// LISTAR PERIODOS ACADEMICOS
// ==========================================

std::vector<PeriodoAcademico>
GestorPeriodos::listarPeriodos() const {

    return periodos;
}

// ==========================================
// BUSCAR PERIODO POR CODIGO
// ==========================================

const PeriodoAcademico* GestorPeriodos::buscarPeriodo(
    const std::string& codigo
) const {

    for (const PeriodoAcademico& periodo : periodos) {

        if (periodo.obtenerDatos().codigo == codigo) {
            return &periodo;
        }
    }

    return nullptr;
}

// ==========================================
// MODIFICAR PERIODO ACADEMICO
// ==========================================

bool GestorPeriodos::modificarPeriodo(
    const std::string& codigo,
    const DatosPeriodoAcademico& nuevosDatos
) {

    if (!validarDatos(nuevosDatos)) {
        return false;
    }

    // El codigo original no puede modificarse
    if (codigo != nuevosDatos.codigo) {
        return false;
    }

    if (existePeriodo(
            nuevosDatos.anio,
            nuevosDatos.semestre,
            codigo
        )) {
        return false;
    }

    for (PeriodoAcademico& periodo : periodos) {

        if (periodo.obtenerDatos().codigo == codigo) {

            if (periodo.obtenerEstado() ==
                EstadoPeriodo::Cerrado) {
                return false;
            }

            return periodo.actualizarDatos(nuevosDatos);
        }
    }

    return false;
}

// ==========================================
// CERRAR PERIODO ACADEMICO
// ==========================================

bool GestorPeriodos::cerrarPeriodo(
    const std::string& codigo
) {

    for (PeriodoAcademico& periodo : periodos) {

        if (periodo.obtenerDatos().codigo == codigo) {
            return periodo.cerrarPeriodo();
        }
    }

    return false;
}

// ==========================================
// VERIFICAR SI PERMITE MATRICULAS
// ==========================================

bool GestorPeriodos::permiteMatricula(
    const std::string& codigo
) const {

    const PeriodoAcademico* periodo =
        buscarPeriodo(codigo);

    if (periodo == nullptr) {
        return false;
    }

    return periodo->obtenerEstado() ==
           EstadoPeriodo::Abierto;
}
