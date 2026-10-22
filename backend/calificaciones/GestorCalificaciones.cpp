
#include "GestorCalificaciones.hpp"
#include "../cursos/GestorCursos.hpp"
#include "../matriculas/GestorMatriculas.hpp"

#include <cmath>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace {
    constexpr double TOLERANCIA = 1e-7;

    std::string fechaHoraActual() {
        std::time_t instante = std::time(nullptr);
        std::tm* fecha = std::localtime(&instante);

        if (fecha == nullptr) {
            return "Fecha no disponible";
        }

        std::ostringstream salida;

        salida << std::put_time(
            fecha,
            "%Y-%m-%d %H:%M:%S"
        );

        return salida.str();
    }

    bool esBisiesto(int anio) {
        return anio % 400 == 0 ||
               (anio % 4 == 0 && anio % 100 != 0);
    }
}

// ==========================================
// CONSTRUCTOR
// ==========================================

GestorCalificaciones::GestorCalificaciones(
    GestorCursos& gestorCursos,
    const GestorMatriculas& gestorMatriculas
)
    : cursos(gestorCursos),
      matriculas(gestorMatriculas) {
}

// ==========================================
// VALIDAR TEXTO
// ==========================================

bool GestorCalificaciones::tieneTexto(
    const std::string& texto
) {
    return texto.find_first_not_of(" \t\r\n")
           != std::string::npos;
}

// ==========================================
// VALIDAR FECHAS
// ==========================================

bool GestorCalificaciones::fechaValida(
    const std::string& fecha
) {
    if (
        fecha.size() != 10 ||
        fecha[4] != '-' ||
        fecha[7] != '-'
    ) {
        return false;
    }

    for (std::size_t i = 0; i < fecha.size(); i++) {
        if (i == 4 || i == 7) {
            continue;
        }

        if (fecha[i] < '0' || fecha[i] > '9') {
            return false;
        }
    }

    const int anio = std::stoi(fecha.substr(0, 4));
    const int mes = std::stoi(fecha.substr(5, 2));
    const int dia = std::stoi(fecha.substr(8, 2));

    if (anio < 1 || mes < 1 || mes > 12) {
        return false;
    }

    const int diasMes[] = {
        31, 28, 31, 30, 31, 30,
        31, 31, 30, 31, 30, 31
    };

    int maxDias = diasMes[mes - 1];

    if (mes == 2 && esBisiesto(anio)) {
        maxDias = 29;
    }

    return dia >= 1 && dia <= maxDias;
}

// ==========================================
// VERIFICAR DOCENTE DEL CURSO
// ==========================================

bool GestorCalificaciones::docenteHabilitado(
    const std::string& codigoCurso,
    const std::string& codigoDocente
) const {
    if (!tieneTexto(codigoDocente)) {
        return false;
    }

    const Curso* curso =
        cursos.buscarCurso(codigoCurso);

    return curso != nullptr &&
           curso->obtenerEstado() ==
               EstadoCurso::Activo &&
           curso->obtenerDocente() ==
               codigoDocente;
}

// ==========================================
// VERIFICAR MATRICULA VIGENTE
// ==========================================

bool GestorCalificaciones::estaMatriculado(
    const std::string& rut,
    const std::string& codigoCurso
) const {
    const auto inscritos =
        matriculas.consultarInscritos(codigoCurso);

    for (const auto& matricula : inscritos) {
        if (
            matricula.obtenerDatos().rutEstudiante ==
            rut
        ) {
            return true;
        }
    }

    return false;
}

// ==========================================
// AC-66: BUSCAR EVALUACION
// ==========================================

const Evaluacion* GestorCalificaciones::buscarEvaluacion(
    const std::string& codigo
) const {
    for (const Evaluacion& evaluacion : evaluaciones) {
        if (evaluacion.obtenerDatos().codigo == codigo) {
            return &evaluacion;
        }
    }

    return nullptr;
}

// ==========================================
// AC-66: LISTAR EVALUACIONES
// ==========================================

std::vector<Evaluacion>
GestorCalificaciones::consultarEvaluaciones(
    const std::string& codigoCurso
) const {
    std::vector<Evaluacion> resultado;

    for (const Evaluacion& evaluacion : evaluaciones) {
        if (
            evaluacion.obtenerDatos().codigoCurso ==
            codigoCurso
        ) {
            resultado.push_back(evaluacion);
        }
    }

    return resultado;
}

// ==========================================
// AC-66: REGISTRAR EVALUACION
// ==========================================

bool GestorCalificaciones::registrarEvaluacion(
    const DatosEvaluacion& datos,
    const std::string& codigoDocente
) {
    ultimoError.clear();

    if (
        !tieneTexto(datos.codigo) ||
        !tieneTexto(datos.codigoCurso) ||
        !tieneTexto(datos.nombre) ||
        !tieneTexto(datos.tipo) ||
        !fechaValida(datos.fecha) ||
        !std::isfinite(datos.ponderacion) ||
        datos.ponderacion <= 0.0 ||
        datos.ponderacion > 100.0
    ) {
        ultimoError =
            "Datos de evaluacion invalidos.";

        return false;
    }

    if (
        !docenteHabilitado(
            datos.codigoCurso,
            codigoDocente
        )
    ) {
        ultimoError =
            "El docente no esta asignado a un "
            "curso activo.";

        return false;
    }

    if (buscarEvaluacion(datos.codigo) != nullptr) {
        ultimoError =
            "Ya existe una evaluacion con ese codigo.";

        return false;
    }

    double total = datos.ponderacion;

    for (const Evaluacion& anterior : evaluaciones) {
        const DatosEvaluacion& actual =
            anterior.obtenerDatos();

        if (actual.codigoCurso != datos.codigoCurso) {
            continue;
        }

        if (actual.nombre == datos.nombre) {
            ultimoError =
                "Ya existe una evaluacion con ese "
                "nombre en el curso.";

            return false;
        }

        total += actual.ponderacion;
    }

    if (total > 100.0 + TOLERANCIA) {
        ultimoError =
            "La suma de ponderaciones supera el 100%.";

        return false;
    }

    evaluaciones.emplace_back(datos);

    return true;
}

// ==========================================
// BUSCAR NOTA PARA MODIFICACION
// ==========================================

RegistroNota* GestorCalificaciones::buscarNotaEditable(
    const std::string& codigoEvaluacion,
    const std::string& rut
) {
    for (RegistroNota& registro : notas) {
        if (
            registro.codigoEvaluacion ==
                codigoEvaluacion &&
            registro.rutEstudiante == rut
        ) {
            return &registro;
        }
    }

    return nullptr;
}

// ==========================================
// AC-70: BUSCAR NOTA
// ==========================================

const RegistroNota* GestorCalificaciones::buscarNota(
    const std::string& codigoEvaluacion,
    const std::string& rut
) const {
    for (const RegistroNota& registro : notas) {
        if (
            registro.codigoEvaluacion ==
                codigoEvaluacion &&
            registro.rutEstudiante == rut
        ) {
            return &registro;
        }
    }

    return nullptr;
}

// ==========================================
// AC-67: REGISTRAR NOTA
// ==========================================

bool GestorCalificaciones::registrarNota(
    const std::string& codigoEvaluacion,
    const std::string& rutEstudiante,
    double nota,
    const std::string& codigoDocente
) {
    ultimoError.clear();

    const Evaluacion* evaluacion =
        buscarEvaluacion(codigoEvaluacion);

    if (!evaluacion) {
        ultimoError = "La evaluacion no existe.";
        return false;
    }

    const std::string& codigoCurso =
        evaluacion->obtenerDatos().codigoCurso;

    if (!docenteHabilitado(codigoCurso, codigoDocente)) {
        ultimoError =
            "Docente sin permisos sobre el curso.";

        return false;
    }

    if (!estaMatriculado(rutEstudiante, codigoCurso)) {
        ultimoError =
            "El estudiante no tiene matricula "
            "vigente en el curso.";

        return false;
    }

    if (
        !std::isfinite(nota) ||
        nota < 1.0 ||
        nota > 7.0
    ) {
        ultimoError =
            "La nota debe estar entre 1.0 y 7.0.";

        return false;
    }

    if (
        buscarNota(
            codigoEvaluacion,
            rutEstudiante
        ) != nullptr
    ) {
        ultimoError =
            "La nota ya existe; utilice modificar nota.";

        return false;
    }

    notas.push_back({
        codigoEvaluacion,
        rutEstudiante,
        nota,
        false,
        {}
    });

    return true;
}

// ==========================================
// AC-67: MODIFICAR NOTA
// ==========================================

bool GestorCalificaciones::modificarNota(
    const std::string& codigoEvaluacion,
    const std::string& rutEstudiante,
    double nuevaNota,
    const std::string& motivo,
    const std::string& codigoDocente
) {
    ultimoError.clear();

    const Evaluacion* evaluacion =
        buscarEvaluacion(codigoEvaluacion);

    if (
        !evaluacion ||
        !docenteHabilitado(
            evaluacion->obtenerDatos().codigoCurso,
            codigoDocente
        )
    ) {
        ultimoError =
            "Evaluacion inexistente o docente "
            "no autorizado.";

        return false;
    }

    if (
        !tieneTexto(motivo) ||
        !std::isfinite(nuevaNota) ||
        nuevaNota < 1.0 ||
        nuevaNota > 7.0
    ) {
        ultimoError =
            "Nota o motivo de modificacion invalidos.";

        return false;
    }

    RegistroNota* registro =
        buscarNotaEditable(
            codigoEvaluacion,
            rutEstudiante
        );

    if (!registro) {
        ultimoError =
            "La calificacion que desea modificar "
            "no existe.";

        return false;
    }

    if (
        std::abs(registro->nota - nuevaNota) <
        TOLERANCIA
    ) {
        ultimoError =
            "La nueva nota es igual a la anterior.";

        return false;
    }

    registro->historial.push_back({
        registro->nota,
        nuevaNota,
        motivo,
        codigoDocente,
        fechaHoraActual()
    });

    registro->nota = nuevaNota;

    return true;
}

// ==========================================
// PUBLICAR NOTA
// ==========================================

bool GestorCalificaciones::publicarNota(
    const std::string& codigoEvaluacion,
    const std::string& rutEstudiante,
    const std::string& codigoDocente
) {
    ultimoError.clear();

    const Evaluacion* evaluacion =
        buscarEvaluacion(codigoEvaluacion);

    if (
        !evaluacion ||
        !docenteHabilitado(
            evaluacion->obtenerDatos().codigoCurso,
            codigoDocente
        )
    ) {
        ultimoError =
            "Evaluacion inexistente o docente "
            "no autorizado.";

        return false;
    }

    RegistroNota* registro =
        buscarNotaEditable(
            codigoEvaluacion,
            rutEstudiante
        );

    if (!registro || registro->publicada) {
        ultimoError =
            "Nota inexistente o ya publicada.";

        return false;
    }

    registro->publicada = true;

    return true;
}

// ==========================================
// AC-70: CONSULTAR NOTAS
// ==========================================

std::vector<RegistroNota>
GestorCalificaciones::consultarNotas(
    const std::string& rutEstudiante,
    const std::string& codigoCurso,
    bool soloPublicadas
) const {
    std::vector<RegistroNota> resultado;

    for (const RegistroNota& nota : notas) {
        if (nota.rutEstudiante != rutEstudiante) {
            continue;
        }

        if (soloPublicadas && !nota.publicada) {
            continue;
        }

        const Evaluacion* evaluacion =
            buscarEvaluacion(
                nota.codigoEvaluacion
            );

        if (
            evaluacion &&
            evaluacion->obtenerDatos().codigoCurso ==
                codigoCurso
        ) {
            resultado.push_back(nota);
        }
    }

    return resultado;
}

// ==========================================
// AC-68: CALCULAR PROMEDIO PONDERADO
// ==========================================

ResultadoPromedio
GestorCalificaciones::calcularPromedio(
    const std::string& rutEstudiante,
    const std::string& codigoCurso,
    bool soloPublicadas
) const {
    ResultadoPromedio resultado;

    if (!estaMatriculado(rutEstudiante, codigoCurso)) {
        resultado.mensaje =
            "El estudiante no tiene matricula vigente.";

        return resultado;
    }

    const auto lista =
        consultarEvaluaciones(codigoCurso);

    if (lista.empty()) {
        resultado.mensaje =
            "No existen evaluaciones registradas.";

        return resultado;
    }

    double sumaProgramada = 0.0;
    double ponderacionConNota = 0.0;
    double sumaPonderada = 0.0;

    for (const Evaluacion& evaluacion : lista) {
        const auto& datos =
            evaluacion.obtenerDatos();

        sumaProgramada += datos.ponderacion;

        const RegistroNota* nota =
            buscarNota(
                datos.codigo,
                rutEstudiante
            );

        if (
            !nota ||
            (soloPublicadas && !nota->publicada)
        ) {
            continue;
        }

        ponderacionConNota +=
            datos.ponderacion;

        sumaPonderada +=
            nota->nota * datos.ponderacion;
    }

    resultado.ponderacionEvaluada =
        ponderacionConNota;

    if (ponderacionConNota <= TOLERANCIA) {
        resultado.mensaje =
            "No hay calificaciones disponibles.";

        return resultado;
    }

    resultado.disponible = true;

    // El promedio parcial considera solamente
    // las evaluaciones que tienen calificacion.
    // Las notas pendientes NO cuentan como cero.

    resultado.promedio =
        sumaPonderada / ponderacionConNota;

    resultado.definitivo =
        std::abs(sumaProgramada - 100.0) <
            TOLERANCIA &&
        std::abs(ponderacionConNota - 100.0) <
            TOLERANCIA;

    resultado.mensaje = resultado.definitivo
        ? "Promedio definitivo por notas."
        : "Promedio parcial: existen evaluaciones "
          "o notas pendientes.";

    return resultado;
}

// ==========================================
// AC-69: DETERMINAR CONDICION POR NOTAS
// ==========================================

CondicionPorNotas
GestorCalificaciones::determinarCondicionPorNotas(
    const std::string& rutEstudiante,
    const std::string& codigoCurso
) const {
    const ResultadoPromedio promedio =
        calcularPromedio(
            rutEstudiante,
            codigoCurso
        );

    if (
        !promedio.disponible ||
        !promedio.definitivo
    ) {
        return CondicionPorNotas::Pendiente;
    }

    return promedio.promedio >= 4.0
        ? CondicionPorNotas::Aprobado
        : CondicionPorNotas::Reprobado;
}

// ==========================================
// CONSULTAR ULTIMO ERROR
// ==========================================

const std::string&
GestorCalificaciones::obtenerUltimoError() const {
    return ultimoError;
}
