
#include "GestorMatriculas.hpp"

#include "../cursos/GestorCursos.hpp"
#include "../periodos/GestorPeriodos.hpp"

#include <utility>

// ==========================================
// CONSTRUCTOR
// ==========================================

GestorMatriculas::GestorMatriculas(
    GestorCursos& gestorCursos,
    const GestorPeriodos& gestorPeriodos,
    ValidadorEstudiante validarEstudiante,
    ValidadorPrerequisitos validarPrerequisitos
)
    : cursos(gestorCursos),
      periodos(gestorPeriodos),
      estudianteActivo(
          std::move(validarEstudiante)
      ),
      prerrequisitosCumplidos(
          std::move(validarPrerequisitos)
      ) {
}

// ==========================================
// VERIFICAR TEXTO NO VACIO
// ==========================================

bool GestorMatriculas::tieneTexto(
    const std::string& texto
) {

    return texto.find_first_not_of(" \t\n\r")
           != std::string::npos;
}

// ==========================================
// VERIFICAR CONFLICTOS DE HORARIO
// ==========================================

bool GestorMatriculas::horariosSeCruzan(
    const Curso& primero,
    const Curso& segundo
) {

    const auto& horariosA =
        primero.obtenerDatos().horarios;

    const auto& horariosB =
        segundo.obtenerDatos().horarios;

    for (const auto& a : horariosA) {

        for (const auto& b : horariosB) {

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
// BUSCAR MATRICULA PARA MODIFICACION
// ==========================================

Matricula* GestorMatriculas::buscarEditable(
    const std::string& codigo
) {

    for (Matricula& matricula : matriculas) {

        if (
            matricula.obtenerDatos().codigo ==
            codigo
        ) {
            return &matricula;
        }
    }

    return nullptr;
}

// ==========================================
// BUSCAR MATRICULA POR CODIGO
// ==========================================

const Matricula* GestorMatriculas::buscarMatricula(
    const std::string& codigo
) const {

    for (const Matricula& matricula : matriculas) {

        if (
            matricula.obtenerDatos().codigo ==
            codigo
        ) {
            return &matricula;
        }
    }

    return nullptr;
}

// ==========================================
// AC-62: VALIDAR PRERREQUISITOS
// ==========================================

bool GestorMatriculas::verificarPrerequisitos(
    const std::string& rut,
    const std::string& codigoAsignatura
) const {

    // Si falta informacion o conexion
    // con el validador, no se autoriza.

    return
        tieneTexto(rut) &&
        tieneTexto(codigoAsignatura) &&
        prerrequisitosCumplidos &&
        prerrequisitosCumplidos(
            rut,
            codigoAsignatura
        );
}

// ==========================================
// VALIDAR INSCRIPCION A UN CURSO
// ==========================================

bool GestorMatriculas::validarInscripcion(
    const std::string& rut,
    const std::string& codigoCurso,
    const std::string& codigoMatriculaExcluir,
    std::string& motivo
) const {

    // Validar identificadores.

    if (
        !tieneTexto(rut) ||
        !tieneTexto(codigoCurso)
    ) {

        motivo =
            "Falta identificar al estudiante o curso.";

        return false;
    }

    // Buscar curso.

    const Curso* destino =
        cursos.buscarCurso(codigoCurso);

    if (destino == nullptr) {

        motivo = "El curso no existe.";
        return false;
    }

    // Verificar estado del curso.

    if (
        destino->obtenerEstado() !=
        EstadoCurso::Activo
    ) {

        motivo = "El curso esta cerrado.";
        return false;
    }

    const DatosCurso& datos =
        destino->obtenerDatos();

    // Verificar periodo academico.

    if (
        !periodos.permiteMatricula(
            datos.codigoPeriodo
        )
    ) {

        motivo =
            "El periodo no permite nuevas matriculas.";

        return false;
    }

    // Comprobar cupos.

    if (
        destino->obtenerCuposDisponibles() <= 0
    ) {

        motivo = "No hay cupos disponibles.";
        return false;
    }

    // Verificar estudiante activo.

    if (
        !estudianteActivo ||
        !estudianteActivo(rut)
    ) {

        motivo =
            "El estudiante no existe o no esta activo.";

        return false;
    }

    // AC-62: verificar prerrequisitos.

    if (
        !verificarPrerequisitos(
            rut,
            datos.codigoAsignatura
        )
    ) {

        motivo =
            "No se han acreditado los prerrequisitos "
            "de la asignatura.";

        return false;
    }

    // Verificar inscripciones existentes
    // durante el mismo periodo.

    for (const Matricula& matricula : matriculas) {

        const DatosMatricula& anterior =
            matricula.obtenerDatos();

        // Ignorar matriculas anuladas.

        if (!matricula.estaVigente()) {
            continue;
        }

        // Ignorar la matricula que se cambia.

        if (
            anterior.codigo ==
            codigoMatriculaExcluir
        ) {
            continue;
        }

        // Solamente considerar el estudiante.

        if (anterior.rutEstudiante != rut) {
            continue;
        }

        // Solamente comparar el mismo periodo.

        if (
            anterior.codigoPeriodo !=
            datos.codigoPeriodo
        ) {
            continue;
        }

        // Impedir matricular la misma
        // asignatura dos veces.

        if (
            anterior.codigoAsignatura ==
            datos.codigoAsignatura
        ) {

            motivo =
                "El estudiante ya tiene esta "
                "asignatura inscrita en el periodo.";

            return false;
        }

        // Buscar otro curso del estudiante.

        const Curso* otro =
            cursos.buscarCurso(
                anterior.codigoCurso
            );

        if (otro == nullptr) {

            motivo =
                "Una matricula vigente tiene un "
                "curso asociado inexistente.";

            return false;
        }

        // Verificar conflictos entre horarios.

        if (
            horariosSeCruzan(
                *otro,
                *destino
            )
        ) {

            motivo =
                "El horario coincide con otro "
                "curso del estudiante.";

            return false;
        }
    }

    return true;
}

// ==========================================
// AC-61: REGISTRAR MATRICULA
// ==========================================

bool GestorMatriculas::registrarMatricula(
    const DatosMatricula& datos
) {

    ultimoError.clear();

    // Validar codigo obligatorio.

    if (!tieneTexto(datos.codigo)) {

        ultimoError =
            "Debe ingresar el codigo de matricula.";

        return false;
    }

    // Impedir codigos duplicados.

    if (
        buscarMatricula(datos.codigo) != nullptr
    ) {

        ultimoError =
            "El codigo de matricula ya existe.";

        return false;
    }

    // Validaciones academicas.

    std::string motivo;

    if (
        !validarInscripcion(
            datos.rutEstudiante,
            datos.codigoCurso,
            "",
            motivo
        )
    ) {

        ultimoError = motivo;
        return false;
    }

    // Verificar coherencia entre
    // matricula y curso seleccionado.

    const Curso* curso =
        cursos.buscarCurso(datos.codigoCurso);

    const DatosCurso& cursoDatos =
        curso->obtenerDatos();

    if (
        datos.codigoAsignatura !=
            cursoDatos.codigoAsignatura ||
        datos.codigoPeriodo !=
            cursoDatos.codigoPeriodo
    ) {

        ultimoError =
            "La asignatura o periodo no "
            "corresponde al curso.";

        return false;
    }

    // Reservar un cupo en el curso.

    if (
        !cursos.ocuparCupo(
            datos.codigoCurso
        )
    ) {

        ultimoError =
            "No se pudo reservar el cupo del curso.";

        return false;
    }

    // Registrar matricula.

    try {

        matriculas.emplace_back(datos);

    } catch (...) {

        // Si no se logra registrar,
        // devolver el cupo reservado.

        cursos.liberarCupo(
            datos.codigoCurso
        );

        ultimoError =
            "No se pudo registrar la matricula.";

        return false;
    }

    return true;
}

// ==========================================
// AC-64: CAMBIAR SECCION
// ==========================================

bool GestorMatriculas::cambiarSeccion(
    const std::string& codigoMatricula,
    const std::string& nuevoCodigoCurso,
    const std::string& motivo,
    const std::string& responsable
) {

    ultimoError.clear();

    // Buscar la matricula existente.

    Matricula* matricula =
        buscarEditable(codigoMatricula);

    if (
        matricula == nullptr ||
        !matricula->estaVigente()
    ) {

        ultimoError =
            "La matricula no existe o no esta vigente.";

        return false;
    }

    // Validar motivo y responsable.

    if (
        !tieneTexto(motivo) ||
        !tieneTexto(responsable)
    ) {

        ultimoError =
            "Debe indicar motivo y responsable.";

        return false;
    }

    const DatosMatricula& actual =
        matricula->obtenerDatos();

    // Evitar cambios a la misma seccion.

    if (
        actual.codigoCurso ==
        nuevoCodigoCurso
    ) {

        ultimoError =
            "La matricula ya pertenece a ese curso.";

        return false;
    }

    // Recuperar cursos.

    const Curso* origen =
        cursos.buscarCurso(
            actual.codigoCurso
        );

    const Curso* destino =
        cursos.buscarCurso(
            nuevoCodigoCurso
        );

    if (
        origen == nullptr ||
        destino == nullptr ||
        origen->obtenerEstado() !=
            EstadoCurso::Activo
    ) {

        ultimoError =
            "No se puede modificar el curso "
            "de origen o destino.";

        return false;
    }

    const DatosCurso& nuevo =
        destino->obtenerDatos();

    // Solo cambiar entre secciones
    // de la misma asignatura y periodo.

    if (
        nuevo.codigoAsignatura !=
            actual.codigoAsignatura ||
        nuevo.codigoPeriodo !=
            actual.codigoPeriodo
    ) {

        ultimoError =
            "Solo se permite cambiar a otra "
            "seccion de la misma asignatura "
            "y periodo.";

        return false;
    }

    // Volver a verificar cupos,
    // conflictos horarios y prerrequisitos.

    std::string errorValidacion;

    if (
        !validarInscripcion(
            actual.rutEstudiante,
            nuevoCodigoCurso,
            codigoMatricula,
            errorValidacion
        )
    ) {

        ultimoError = errorValidacion;
        return false;
    }

    // Reservar cupo en la nueva seccion.

    if (
        !cursos.ocuparCupo(
            nuevoCodigoCurso
        )
    ) {

        ultimoError =
            "No se pudo reservar el cupo de destino.";

        return false;
    }

    // Liberar cupo de la seccion anterior.

    if (
        !cursos.liberarCupo(
            actual.codigoCurso
        )
    ) {

        // Revertir reserva si falla.

        cursos.liberarCupo(
            nuevoCodigoCurso
        );

        ultimoError =
            "No se pudo liberar el cupo de origen.";

        return false;
    }

    // Conservar identificador del curso anterior.

    const std::string codigoAnterior =
        actual.codigoCurso;

    // Registrar movimiento y cambiar seccion.

    try {

        if (
            matricula->cambiarSeccion(
                nuevoCodigoCurso,
                motivo,
                responsable
            )
        ) {
            return true;
        }

    } catch (...) {

        // Se intentara restaurar
        // la disponibilidad de cupos.
    }

    // Revertir la reserva si el cambio falla.

    cursos.ocuparCupo(codigoAnterior);

    cursos.liberarCupo(nuevoCodigoCurso);

    ultimoError =
        "No se pudo completar el cambio de seccion.";

    return false;
}

// ==========================================
// AC-64: ANULAR MATRICULA
// ==========================================

bool GestorMatriculas::anularMatricula(
    const std::string& codigoMatricula,
    const std::string& motivo,
    const std::string& responsable
) {

    ultimoError.clear();

    // Buscar matricula.

    Matricula* matricula =
        buscarEditable(codigoMatricula);

    if (
        matricula == nullptr ||
        !matricula->estaVigente()
    ) {

        ultimoError =
            "La matricula no existe o ya fue anulada.";

        return false;
    }

    // Verificar motivo y responsable.

    if (
        !tieneTexto(motivo) ||
        !tieneTexto(responsable)
    ) {

        ultimoError =
            "Debe indicar motivo y responsable.";

        return false;
    }

    const std::string codigoCurso =
        matricula->obtenerDatos().codigoCurso;

    const Curso* curso =
        cursos.buscarCurso(codigoCurso);

    // No permitir anulaciones cuando
    // el curso o periodo estan cerrados.

    if (
        curso == nullptr ||
        curso->obtenerEstado() !=
            EstadoCurso::Activo ||
        !periodos.permiteMatricula(
            curso->obtenerDatos().codigoPeriodo
        )
    ) {

        ultimoError =
            "No se puede anular una matricula "
            "de un curso o periodo cerrado.";

        return false;
    }

    // Liberar cupo.

    if (!cursos.liberarCupo(codigoCurso)) {

        ultimoError =
            "No se pudo liberar el cupo.";

        return false;
    }

    // Cambiar estado y guardar historial.

    try {

        if (
            matricula->anular(
                motivo,
                responsable
            )
        ) {
            return true;
        }

    } catch (...) {

        // Recuperar el cupo si
        // no se completa la operacion.
    }

    // Revertir la liberacion del cupo.

    cursos.ocuparCupo(codigoCurso);

    ultimoError =
        "No se pudo completar la anulacion.";

    return false;
}

// ==========================================
// LISTAR TODAS LAS MATRICULAS
// ==========================================

std::vector<Matricula>
GestorMatriculas::listarMatriculas() const {

    return matriculas;
}

// ==========================================
// AC-63: CONSULTAR INSCRITOS POR CURSO
// ==========================================

std::vector<Matricula>
GestorMatriculas::consultarInscritos(
    const std::string& codigoCurso
) const {

    std::vector<Matricula> resultado;

    for (const Matricula& matricula : matriculas) {

        if (
            matricula.estaVigente() &&
            matricula.obtenerDatos().codigoCurso ==
                codigoCurso
        ) {

            resultado.push_back(matricula);
        }
    }

    return resultado;
}

// ==========================================
// CONSULTAR MATRICULAS POR ESTUDIANTE
// ==========================================

std::vector<Matricula>
GestorMatriculas::consultarPorEstudiante(
    const std::string& rut,
    bool incluirAnuladas
) const {

    std::vector<Matricula> resultado;

    for (const Matricula& matricula : matriculas) {

        if (
            matricula.obtenerDatos().rutEstudiante !=
            rut
        ) {
            continue;
        }

        if (
            !incluirAnuladas &&
            !matricula.estaVigente()
        ) {
            continue;
        }

        resultado.push_back(matricula);
    }

    return resultado;
}

// ==========================================
// CONSULTAR ULTIMO ERROR
// ==========================================

const std::string&
GestorMatriculas::obtenerUltimoError() const {

    return ultimoError;
}
