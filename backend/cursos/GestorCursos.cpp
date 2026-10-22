
#include "GestorCursos.hpp"

#include <utility>

// ==========================================
// CONSTRUCTOR
// ==========================================

GestorCursos::GestorCursos(
    ValidadorCodigo validarPeriodoAbierto,
    ValidadorCodigo validarAsignaturaActiva,
    ValidadorCodigo validarDocenteActivo
)
    : periodoAbierto(std::move(validarPeriodoAbierto)),
      asignaturaActiva(std::move(validarAsignaturaActiva)),
      docenteActivo(std::move(validarDocenteActivo)) {
}

// ==========================================
// VERIFICAR TEXTO NO VACIO
// ==========================================

bool GestorCursos::tieneTexto(
    const std::string& texto
) {

    return texto.find_first_not_of(" \t\n\r")
           != std::string::npos;
}

// ==========================================
// DETECTAR SUPERPOSICION DE HORARIOS
// ==========================================

bool GestorCursos::horariosSeSuperponen(
    const std::vector<HorarioCurso>& primero,
    const std::vector<HorarioCurso>& segundo
) {

    for (const HorarioCurso& a : primero) {

        for (const HorarioCurso& b : segundo) {

            if (
                a.dia == b.dia &&
                a.horaInicio < b.horaFin &&
                b.horaInicio < a.horaFin
            ) {
                return true;
            }
        }
    }

    return false;
}

// ==========================================
// VALIDAR HORARIOS DEL CURSO
// ==========================================

bool GestorCursos::horariosValidos(
    const std::vector<HorarioCurso>& horarios
) {

    if (horarios.empty()) {
        return false;
    }

    for (const HorarioCurso& horario : horarios) {

        if (
            horario.dia < 1 ||
            horario.dia > 7 ||
            horario.horaInicio < 0 ||
            horario.horaInicio >= 1440 ||
            horario.horaFin <= horario.horaInicio ||
            horario.horaFin > 1440
        ) {
            return false;
        }
    }

    // Impedir bloques superpuestos dentro
    // del mismo curso.

    for (std::size_t i = 0; i < horarios.size(); i++) {

        for (
            std::size_t j = i + 1;
            j < horarios.size();
            j++
        ) {

            if (
                horarios[i].dia == horarios[j].dia &&
                horarios[i].horaInicio < horarios[j].horaFin &&
                horarios[j].horaInicio < horarios[i].horaFin
            ) {
                return false;
            }
        }
    }

    return true;
}

// ==========================================
// VALIDAR DATOS DEL CURSO
// ==========================================

bool GestorCursos::validarDatos(
    const DatosCurso& datos
) const {

    // Verificar campos obligatorios.

    if (
        !tieneTexto(datos.codigo) ||
        !tieneTexto(datos.codigoAsignatura) ||
        !tieneTexto(datos.codigoPeriodo) ||
        !tieneTexto(datos.seccion) ||
        !tieneTexto(datos.modalidad)
    ) {
        return false;
    }

    // La capacidad debe ser positiva.

    if (datos.capacidadMaxima <= 0) {
        return false;
    }

    // Verificar horarios.

    if (!horariosValidos(datos.horarios)) {
        return false;
    }

    // Es necesario disponer de validadores
    // para comprobar asignaturas y periodos.

    if (!periodoAbierto || !asignaturaActiva) {
        return false;
    }

    // Verificar periodo habilitado y
    // asignatura existente y activa.

    if (!periodoAbierto(datos.codigoPeriodo)) {
        return false;
    }

    if (!asignaturaActiva(datos.codigoAsignatura)) {
        return false;
    }

    return true;
}

// ==========================================
// VERIFICAR CURSOS DUPLICADOS
// ==========================================

bool GestorCursos::existeDuplicado(
    const DatosCurso& datos,
    const std::string& codigoExcluir
) const {

    for (const Curso& curso : cursos) {

        const DatosCurso& actual =
            curso.obtenerDatos();

        if (actual.codigo == codigoExcluir) {
            continue;
        }

        // No repetir codigo de curso.

        if (actual.codigo == datos.codigo) {
            return true;
        }

        // No repetir asignatura, periodo
        // y seccion simultaneamente.

        if (
            actual.codigoAsignatura ==
                datos.codigoAsignatura &&
            actual.codigoPeriodo ==
                datos.codigoPeriodo &&
            actual.seccion == datos.seccion
        ) {
            return true;
        }
    }

    return false;
}

// ==========================================
// VERIFICAR CONFLICTO DE HORARIO DOCENTE
// ==========================================

bool GestorCursos::docenteTieneConflicto(
    const std::string& identificadorDocente,
    const std::string& codigoPeriodo,
    const std::vector<HorarioCurso>& horarios,
    const std::string& codigoExcluir
) const {

    for (const Curso& curso : cursos) {

        const DatosCurso& actual =
            curso.obtenerDatos();

        // Ignorar el curso que se esta editando.

        if (actual.codigo == codigoExcluir) {
            continue;
        }

        // Solo comparar cursos del mismo periodo.

        if (actual.codigoPeriodo != codigoPeriodo) {
            continue;
        }

        // Ignorar cursos cerrados.

        if (
            curso.obtenerEstado() !=
            EstadoCurso::Activo
        ) {
            continue;
        }

        // Verificar que sea el mismo docente.

        if (
            curso.obtenerDocente() !=
            identificadorDocente
        ) {
            continue;
        }

        // Comparar horarios.

        if (
            horariosSeSuperponen(
                actual.horarios,
                horarios
            )
        ) {
            return true;
        }
    }

    return false;
}

// ==========================================
// BUSCAR CURSO PARA MODIFICACION
// ==========================================

Curso* GestorCursos::buscarCursoEditable(
    const std::string& codigo
) {

    for (Curso& curso : cursos) {

        if (curso.obtenerDatos().codigo == codigo) {
            return &curso;
        }
    }

    return nullptr;
}

// ==========================================
// BUSCAR CURSO POR CODIGO
// ==========================================

const Curso* GestorCursos::buscarCurso(
    const std::string& codigo
) const {

    for (const Curso& curso : cursos) {

        if (curso.obtenerDatos().codigo == codigo) {
            return &curso;
        }
    }

    return nullptr;
}

// ==========================================
// LISTAR TODOS LOS CURSOS
// ==========================================

std::vector<Curso> GestorCursos::listarCursos() const {

    return cursos;
}

// ==========================================
// AC-59: REGISTRAR CURSO
// ==========================================

bool GestorCursos::registrarCurso(
    const DatosCurso& datos
) {

    // Validar informacion ingresada.

    if (!validarDatos(datos)) {
        return false;
    }

    // No permitir registros duplicados.

    if (existeDuplicado(datos)) {
        return false;
    }

    // Crear y almacenar curso.

    cursos.emplace_back(datos);

    return true;
}

// ==========================================
// AC-59: MODIFICAR CURSO
// ==========================================

bool GestorCursos::modificarCurso(
    const std::string& codigo,
    const DatosCurso& nuevosDatos
) {

    Curso* curso = buscarCursoEditable(codigo);

    if (curso == nullptr) {
        return false;
    }

    // No modificar cursos cerrados.

    if (
        curso->obtenerEstado() !=
        EstadoCurso::Activo
    ) {
        return false;
    }

    const DatosCurso& anteriores =
        curso->obtenerDatos();

    // Los identificadores de la oferta
    // academica no pueden modificarse.

    if (
        nuevosDatos.codigo != anteriores.codigo ||
        nuevosDatos.codigoAsignatura !=
            anteriores.codigoAsignatura ||
        nuevosDatos.codigoPeriodo !=
            anteriores.codigoPeriodo ||
        nuevosDatos.seccion != anteriores.seccion
    ) {
        return false;
    }

    if (!validarDatos(nuevosDatos)) {
        return false;
    }

    if (
        existeDuplicado(
            nuevosDatos,
            codigo
        )
    ) {
        return false;
    }

    // Si tiene docente asignado,
    // verificar posibles conflictos
    // antes de cambiar el horario.

    const std::string docente =
        curso->obtenerDocente();

    if (!docente.empty()) {

        if (
            docenteTieneConflicto(
                docente,
                nuevosDatos.codigoPeriodo,
                nuevosDatos.horarios,
                codigo
            )
        ) {
            return false;
        }
    }

    return curso->actualizarDatos(nuevosDatos);
}

// ==========================================
// CERRAR CURSO
// ==========================================

bool GestorCursos::cerrarCurso(
    const std::string& codigo
) {

    Curso* curso = buscarCursoEditable(codigo);

    if (curso == nullptr) {
        return false;
    }

    return curso->cerrarCurso();
}

// ==========================================
// AC-60: ASIGNAR DOCENTE A CURSO
// ==========================================

bool GestorCursos::asignarDocente(
    const std::string& codigoCurso,
    const std::string& identificadorDocente
) {

    Curso* curso =
        buscarCursoEditable(codigoCurso);

    if (curso == nullptr) {
        return false;
    }

    // El curso debe estar activo.

    if (
        curso->obtenerEstado() !=
        EstadoCurso::Activo
    ) {
        return false;
    }

    // Verificar identificador del docente.

    if (!tieneTexto(identificadorDocente)) {
        return false;
    }

    // Verificar docente existente y activo.

    if (!docenteActivo) {
        return false;
    }

    if (!docenteActivo(identificadorDocente)) {
        return false;
    }

    // Verificar periodo habilitado.

    if (!periodoAbierto) {
        return false;
    }

    if (
        !periodoAbierto(
            curso->obtenerDatos().codigoPeriodo
        )
    ) {
        return false;
    }

    const DatosCurso& datos =
        curso->obtenerDatos();

    // Impedir que el docente tenga
    // clases superpuestas.

    if (
        docenteTieneConflicto(
            identificadorDocente,
            datos.codigoPeriodo,
            datos.horarios,
            codigoCurso
        )
    ) {
        return false;
    }

    return curso->asignarDocente(
        identificadorDocente
    );
}

// ==========================================
// QUITAR DOCENTE DE UN CURSO
// ==========================================

bool GestorCursos::quitarDocente(
    const std::string& codigoCurso
) {

    Curso* curso =
        buscarCursoEditable(codigoCurso);

    if (curso == nullptr) {
        return false;
    }

    return curso->quitarDocente();
}

// ==========================================
// AC-65: CONSULTAR OFERTA ACADEMICA
// ==========================================

std::vector<Curso> GestorCursos::consultarOferta(
    const std::string& codigoPeriodo,
    const std::string& codigoAsignatura,
    const std::string& seccion,
    bool soloConCupos
) const {

    std::vector<Curso> resultado;

    for (const Curso& curso : cursos) {

        const DatosCurso& datos =
            curso.obtenerDatos();

        // Filtrar por periodo academico.

        if (
            !codigoPeriodo.empty() &&
            datos.codigoPeriodo != codigoPeriodo
        ) {
            continue;
        }

        // Filtrar por asignatura.

        if (
            !codigoAsignatura.empty() &&
            datos.codigoAsignatura !=
                codigoAsignatura
        ) {
            continue;
        }

        // Filtrar por seccion.

        if (
            !seccion.empty() &&
            datos.seccion != seccion
        ) {
            continue;
        }

        // Mostrar solo cursos con
        // cupos disponibles y habilitados.

        if (soloConCupos) {

            if (
                curso.obtenerEstado() !=
                EstadoCurso::Activo
            ) {
                continue;
            }

            if (
                curso.obtenerCuposDisponibles() <= 0
            ) {
                continue;
            }

            if (
                !periodoAbierto ||
                !periodoAbierto(datos.codigoPeriodo)
            ) {
                continue;
            }
        }

        resultado.push_back(curso);
    }

    return resultado;
}

// ==========================================
// OCUPAR CUPO
// APOYO AL FUTURO MODULO DE MATRICULAS
// ==========================================

bool GestorCursos::ocuparCupo(
    const std::string& codigoCurso
) {

    Curso* curso =
        buscarCursoEditable(codigoCurso);

    if (curso == nullptr) {
        return false;
    }

    if (!periodoAbierto) {
        return false;
    }

    if (
        !periodoAbierto(
            curso->obtenerDatos().codigoPeriodo
        )
    ) {
        return false;
    }

    return curso->ocuparCupo();
}

// ==========================================
// LIBERAR CUPO
// APOYO AL FUTURO MODULO DE MATRICULAS
// ==========================================

bool GestorCursos::liberarCupo(
    const std::string& codigoCurso
) {

    Curso* curso =
        buscarCursoEditable(codigoCurso);

    if (curso == nullptr) {
        return false;
    }

    return curso->liberarCupo();
}
