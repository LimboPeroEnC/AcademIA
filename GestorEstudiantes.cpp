#include "GestorEstudiantes.hpp"
#include "ValidadorRut.hpp"

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <ctime>
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

    std::string convertirMinusculas(std::string texto) {
        std::transform(
            texto.begin(),
            texto.end(),
            texto.begin(),
            [](unsigned char caracter) {
                return static_cast<char>(
                    std::tolower(caracter)
                );
            }
        );

        return texto;
    }

    bool contiene(
        const std::string& texto,
        const std::string& criterio
    ) {
        return convertirMinusculas(texto).find(
            convertirMinusculas(criterio)
        ) != std::string::npos;
    }

    int obtenerAnioActual() {
        std::time_t tiempo = std::time(nullptr);
        std::tm fecha{};

#ifdef _WIN32
        localtime_s(&fecha, &tiempo);
#else
        localtime_r(&tiempo, &fecha);
#endif

        return fecha.tm_year + 1900;
    }

    bool datosValidos(const DatosEstudiante& datos) {
        return
            !estaVacio(datos.rut) &&
            !estaVacio(datos.nombres) &&
            !estaVacio(datos.apellidoPaterno) &&
            !estaVacio(datos.apellidoMaterno) &&
            !estaVacio(datos.fechaNacimiento) &&
            !estaVacio(datos.direccion) &&
            !estaVacio(datos.comuna) &&
            !estaVacio(datos.correoInstitucional) &&
            !estaVacio(datos.genero) &&
            !estaVacio(datos.carreraId) &&
            datos.anioIngreso >= 1900 &&
            datos.anioIngreso <= obtenerAnioActual();
    }

}

// ==========================================
// VERIFICAR RUT DUPLICADO
// ==========================================

bool GestorEstudiantes::existeRut(
    const std::string& rut
) const {

    std::string rutNormalizado =
        ValidadorRut::normalizar(rut);

    for (const Estudiante& estudiante : estudiantes) {

        if (
            ValidadorRut::normalizar(
                estudiante.obtenerRut()
            ) == rutNormalizado
        ) {
            return true;
        }
    }

    return false;
}

// ==========================================
// VERIFICAR CORREO DUPLICADO
// ==========================================

bool GestorEstudiantes::existeCorreo(
    const std::string& correo,
    const std::string& rutExcluir
) const {

    std::string rutExcluido =
        ValidadorRut::normalizar(rutExcluir);

    for (const Estudiante& estudiante : estudiantes) {

        if (
            ValidadorRut::normalizar(
                estudiante.obtenerRut()
            ) == rutExcluido
        ) {
            continue;
        }

        DatosEstudiante datos =
            estudiante.obtenerDatos();

        if (
            convertirMinusculas(datos.correoInstitucional)
            == convertirMinusculas(correo)
        ) {
            return true;
        }
    }

    return false;
}

// ==========================================
// AC-001: REGISTRAR ESTUDIANTE
// ==========================================

bool GestorEstudiantes::registrarEstudiante(
    const DatosEstudiante& datos
) {

    if (!datosValidos(datos)) {
        return false;
    }

    if (!ValidadorRut::validar(datos.rut)) {
        return false;
    }

    if (existeRut(datos.rut)) {
        return false;
    }

    if (existeCorreo(datos.correoInstitucional)) {
        return false;
    }

    DatosEstudiante datosNormalizados = datos;

    datosNormalizados.rut =
        ValidadorRut::normalizar(datos.rut);

    std::string codigoBarra =
        "EST-" + datosNormalizados.rut;

    Estudiante nuevoEstudiante(
        datosNormalizados,
        codigoBarra
    );

    estudiantes.push_back(nuevoEstudiante);

    return true;
}

// ==========================================
// AC-002: BUSQUEDA GENERAL
// ==========================================

std::vector<Estudiante> GestorEstudiantes::buscarEstudiantes(
    const std::string& criterio
) const {

    std::vector<Estudiante> resultados;

    if (estaVacio(criterio)) {
        return resultados;
    }

    for (const Estudiante& estudiante : estudiantes) {

        DatosEstudiante datos =
            estudiante.obtenerDatos();

        bool coincide =
            contiene(
                ValidadorRut::normalizar(datos.rut),
                ValidadorRut::normalizar(criterio)
            ) ||
            contiene(datos.nombres, criterio) ||
            contiene(datos.apellidoPaterno, criterio) ||
            contiene(datos.apellidoMaterno, criterio) ||
            contiene(datos.correoInstitucional, criterio) ||
            contiene(datos.carreraId, criterio) ||
            contiene(estudiante.obtenerEstado(), criterio) ||
            contiene(estudiante.obtenerCodigoBarra(), criterio);

        if (coincide) {
            resultados.push_back(estudiante);
        }
    }

    return resultados;
}

// ==========================================
// AC-002: BUSQUEDA AVANZADA
// ==========================================

std::vector<Estudiante> GestorEstudiantes::buscarEstudiantes(
    const FiltrosEstudiante& filtros,
    std::size_t pagina
) const {

    std::vector<Estudiante> resultados;

    bool sinFiltros =
        estaVacio(filtros.rut) &&
        estaVacio(filtros.nombres) &&
        estaVacio(filtros.apellidoPaterno) &&
        estaVacio(filtros.apellidoMaterno) &&
        estaVacio(filtros.codigoBarra) &&
        estaVacio(filtros.estado) &&
        estaVacio(filtros.carrera);

    if (sinFiltros || pagina == 0) {
        return resultados;
    }

    const std::size_t limite = 50;

    std::size_t coincidencias = 0;

    for (const Estudiante& estudiante : estudiantes) {

        DatosEstudiante datos =
            estudiante.obtenerDatos();

        if (
            !estaVacio(filtros.rut) &&
            !contiene(
                ValidadorRut::normalizar(datos.rut),
                ValidadorRut::normalizar(filtros.rut)
            )
        ) {
            continue;
        }

        if (
            !estaVacio(filtros.nombres) &&
            !contiene(datos.nombres, filtros.nombres)
        ) {
            continue;
        }

        if (
            !estaVacio(filtros.apellidoPaterno) &&
            !contiene(
                datos.apellidoPaterno,
                filtros.apellidoPaterno
            )
        ) {
            continue;
        }

        if (
            !estaVacio(filtros.apellidoMaterno) &&
            !contiene(
                datos.apellidoMaterno,
                filtros.apellidoMaterno
            )
        ) {
            continue;
        }

        if (
            !estaVacio(filtros.codigoBarra) &&
            !contiene(
                estudiante.obtenerCodigoBarra(),
                filtros.codigoBarra
            )
        ) {
            continue;
        }

        if (
            !estaVacio(filtros.estado) &&
            convertirMinusculas(estudiante.obtenerEstado())
                != convertirMinusculas(filtros.estado)
        ) {
            continue;
        }

        if (
            !estaVacio(filtros.carrera) &&
            !contiene(datos.carreraId, filtros.carrera)
        ) {
            continue;
        }

        // Paginacion de 50 estudiantes
        std::size_t paginaCoincidencia =
            coincidencias / limite + 1;

        if (paginaCoincidencia == pagina) {
            resultados.push_back(estudiante);
        }

        coincidencias++;

        if (resultados.size() == limite) {
            break;
        }

        if (paginaCoincidencia > pagina) {
            break;
        }
    }

    return resultados;
}

// ==========================================
// AC-003: EDITAR ESTUDIANTE
// ==========================================

bool GestorEstudiantes::editarEstudiante(
    const std::string& rut,
    const DatosEstudiante& nuevosDatos
) {

    std::string rutNormalizado =
        ValidadorRut::normalizar(rut);

    if (!ValidadorRut::validar(rutNormalizado)) {
        return false;
    }

    if (
        ValidadorRut::normalizar(nuevosDatos.rut)
            != rutNormalizado
    ) {
        return false;
    }

    if (!datosValidos(nuevosDatos)) {
        return false;
    }

    for (Estudiante& estudiante : estudiantes) {

        if (
            ValidadorRut::normalizar(
                estudiante.obtenerRut()
            ) == rutNormalizado
        ) {

            if (
                existeCorreo(
                    nuevosDatos.correoInstitucional,
                    estudiante.obtenerRut()
                )
            ) {
                return false;
            }

            DatosEstudiante datosActualizados =
                nuevosDatos;

            datosActualizados.rut =
                estudiante.obtenerRut();

            estudiante.actualizarDatos(
                datosActualizados
            );

            return true;
        }
    }

    return false;
}

// ==========================================
// AC-004: CAMBIAR ESTADO
// ==========================================

bool GestorEstudiantes::cambiarEstado(
    const std::string& rut,
    const std::string& nuevoEstado,
    const std::string& motivo,
    const std::string& usuarioResponsable
) {

    if (estaVacio(motivo)) {
        return false;
    }

    if (estaVacio(usuarioResponsable)) {
        return false;
    }

    if (
        nuevoEstado != "Activo" &&
        nuevoEstado != "Inactivo"
    ) {
        return false;
    }

    std::string rutNormalizado =
        ValidadorRut::normalizar(rut);

    for (Estudiante& estudiante : estudiantes) {

        if (
            ValidadorRut::normalizar(
                estudiante.obtenerRut()
            ) == rutNormalizado
        ) {

            std::string estadoAnterior =
                estudiante.obtenerEstado();

            if (estadoAnterior == nuevoEstado) {
                return false;
            }

            HistorialEstado registro = {
                estudiante.obtenerRut(),
                estadoAnterior,
                nuevoEstado,
                motivo,
                usuarioResponsable,
                std::time(nullptr)
            };

            historialEstados.push_back(registro);

            estudiante.establecerEstado(
                nuevoEstado
            );

            return true;
        }
    }

    return false;
}

// ==========================================
// AC-004: CONSULTAR HISTORIAL
// ==========================================

std::vector<HistorialEstado>
GestorEstudiantes::consultarHistorial(
    const std::string& rut
) const {

    std::vector<HistorialEstado> resultados;

    std::string rutNormalizado =
        ValidadorRut::normalizar(rut);

    for (const HistorialEstado& registro : historialEstados) {

        if (
            ValidadorRut::normalizar(
                registro.rutEstudiante
            ) == rutNormalizado
        ) {
            resultados.push_back(registro);
        }
    }

    return resultados;
}
