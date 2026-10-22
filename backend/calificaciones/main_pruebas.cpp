
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <cmath>
#include <limits>
#include <cstdlib>

#include "GestorCalificaciones.hpp"
#include "../cursos/GestorCursos.hpp"
#include "../periodos/GestorPeriodos.hpp"
#include "../matriculas/GestorMatriculas.hpp"

using namespace std;

// ==========================================
// DATOS FICTICIOS PARA PRUEBAS
// ==========================================

const string RUT1 = "12.345.678-5";
const string RUT2 = "11.111.111-1";

// ==========================================
// ENTORNO DE PRUEBAS
// ==========================================

struct EntornoPruebas {

    GestorPeriodos periodos;

    set<string> asignaturas{
        "INF101",
        "MAT101"
    };

    set<string> docentes{
        "DOC001",
        "DOC002"
    };

    set<string> estudiantes{
        RUT1,
        RUT2
    };

    GestorCursos cursos;
    GestorMatriculas matriculas;
    GestorCalificaciones calificaciones;

    bool inicializado = false;

    EntornoPruebas()
        : cursos(
            [this](const string& periodo) {
                return periodos.permiteMatricula(periodo);
            },

            [this](const string& asignatura) {
                return asignaturas.count(asignatura) > 0;
            },

            [this](const string& docente) {
                return docentes.count(docente) > 0;
            }
        ),

        matriculas(
            cursos,
            periodos,

            [this](const string& rut) {
                return estudiantes.count(rut) > 0;
            },

            [this](const string& rut,
                   const string& asignatura) {

                // Simulacion explicita.
                // Estas asignaturas no exigen
                // prerrequisitos en las pruebas.

                return estudiantes.count(rut) > 0 &&
                       asignaturas.count(asignatura) > 0;
            }
        ),

        calificaciones(cursos, matriculas) {

        bool correcto = true;

        correcto = periodos.crearPeriodo({
            "PA-2026-2",
            "Segundo semestre 2026",
            2026,
            2,
            "2026-08-03",
            "2026-12-18"
        }) && correcto;

        correcto = cursos.registrarCurso({
            "CUR-INF101-A",
            "INF101",
            "PA-2026-2",
            "A",
            30,
            "Presencial",
            {{1, 510, 600}}
        }) && correcto;

        correcto = cursos.registrarCurso({
            "CUR-MAT101-A",
            "MAT101",
            "PA-2026-2",
            "A",
            30,
            "Presencial",
            {{2, 510, 600}}
        }) && correcto;

        correcto = cursos.asignarDocente(
            "CUR-INF101-A",
            "DOC001"
        ) && correcto;

        correcto = cursos.asignarDocente(
            "CUR-MAT101-A",
            "DOC002"
        ) && correcto;

        correcto = matriculas.registrarMatricula({
            "MAT-001",
            RUT1,
            "CUR-INF101-A",
            "INF101",
            "PA-2026-2"
        }) && correcto;

        correcto = matriculas.registrarMatricula({
            "MAT-002",
            RUT2,
            "CUR-INF101-A",
            "INF101",
            "PA-2026-2"
        }) && correcto;

        inicializado = correcto;
    }
};

// ==========================================
// CONTADOR DINAMICO DE PRUEBAS
// ==========================================

struct ResumenPruebas {

    int total = 0;
    int correctas = 0;

    int totalGrupo = 0;
    int correctasGrupo = 0;

    void iniciarGrupo(const string& nombre) {

        if (totalGrupo > 0) {
            finalizarGrupo();
        }

        totalGrupo = 0;
        correctasGrupo = 0;

        cout << "\n========================================\n";
        cout << nombre << "\n";
        cout << "========================================\n";
    }

    void comprobar(
        bool condicion,
        const string& descripcion
    ) {

        total++;
        totalGrupo++;

        if (condicion) {

            correctas++;
            correctasGrupo++;

            cout << "[OK] ";

        } else {

            cout << "[ERROR] ";
        }

        cout << descripcion << "\n";
    }

    void finalizarGrupo() {

        cout << "\nResultado del grupo: "
             << correctasGrupo
             << "/"
             << totalGrupo
             << "\n";

        totalGrupo = 0;
        correctasGrupo = 0;
    }

    void finalizar() {

        finalizarGrupo();

        cout << "\n========================================\n";
        cout << "        RESULTADO GENERAL\n";
        cout << "========================================\n";

        cout << "Pruebas ejecutadas: "
             << total << "\n";

        cout << "Pruebas correctas: "
             << correctas << "\n";

        cout << "Pruebas fallidas: "
             << total - correctas << "\n";

        cout << "\n";

        if (total == correctas && total > 0) {

            cout << "TODAS LAS PRUEBAS PASARON\n";

        } else {

            cout << "EXISTEN PRUEBAS FALLIDAS\n";
            cout << "Debemos revisar los casos "
                 << "marcados con [ERROR].\n";
        }

        cout << "========================================\n";
    }
};

// ==========================================
// FUNCIONES AUXILIARES DE PRUEBAS
// ==========================================

bool sonIguales(
    double primero,
    double segundo
) {
    return std::abs(primero - segundo) < 1e-6;
}

bool cargarEvaluaciones(EntornoPruebas& e) {

    bool primera =
        e.calificaciones.registrarEvaluacion({
            "EV-001",
            "CUR-INF101-A",
            "Control 1",
            "Control",
            "2026-09-10",
            40.0
        }, "DOC001");

    bool segunda =
        e.calificaciones.registrarEvaluacion({
            "EV-002",
            "CUR-INF101-A",
            "Prueba final",
            "Prueba",
            "2026-11-10",
            60.0
        }, "DOC001");

    return primera && segunda;
}

// ==========================================
// GRUPO 1: EVALUACIONES
// ==========================================

void probarEvaluaciones(ResumenPruebas& r) {

    r.iniciarGrupo(
        "AC-66: REGISTRO Y VALIDACION DE EVALUACIONES"
    );

    EntornoPruebas e;

    auto& g = e.calificaciones;

    r.comprobar(
        e.inicializado,
        "Inicializar datos academicos"
    );

    if (!e.inicializado) {
        return;
    }

    r.comprobar(
        g.consultarEvaluaciones(
            "CUR-INF101-A"
        ).empty(),
        "Curso inicialmente sin evaluaciones"
    );

    DatosEvaluacion control{
        "EV-001",
        "CUR-INF101-A",
        "Control 1",
        "Control",
        "2026-09-10",
        40.0
    };

    r.comprobar(
        g.registrarEvaluacion(control, "DOC001"),
        "Registrar evaluacion con 40%"
    );

    r.comprobar(
        g.buscarEvaluacion("EV-001") != nullptr,
        "Buscar evaluacion existente"
    );

    r.comprobar(
        g.buscarEvaluacion("NO-EXISTE") == nullptr,
        "Buscar evaluacion inexistente"
    );

    r.comprobar(
        !g.registrarEvaluacion(control, "DOC001"),
        "Rechazar codigo duplicado"
    );

    DatosEvaluacion otra = control;

    otra.codigo = "EV-OTRA";

    r.comprobar(
        !g.registrarEvaluacion(otra, "DOC001"),
        "Rechazar nombre duplicado en el mismo curso"
    );

    otra.nombre = "Control 2";

    r.comprobar(
        !g.registrarEvaluacion(otra, "DOC002"),
        "Rechazar docente no asignado"
    );

    otra.codigoCurso = "CUR-INEXISTENTE";

    r.comprobar(
        !g.registrarEvaluacion(otra, "DOC001"),
        "Rechazar curso inexistente"
    );

    otra = control;
    otra.codigo = "";

    r.comprobar(
        !g.registrarEvaluacion(otra, "DOC001"),
        "Rechazar codigo vacio"
    );

    otra = control;
    otra.codigo = "EV-SIN-NOMBRE";
    otra.nombre = "";

    r.comprobar(
        !g.registrarEvaluacion(otra, "DOC001"),
        "Rechazar nombre vacio"
    );

    otra = control;
    otra.codigo = "EV-SIN-TIPO";
    otra.tipo = "";

    r.comprobar(
        !g.registrarEvaluacion(otra, "DOC001"),
        "Rechazar tipo vacio"
    );

    otra = control;
    otra.codigo = "EV-FECHA-1";
    otra.fecha = "10/09/2026";

    r.comprobar(
        !g.registrarEvaluacion(otra, "DOC001"),
        "Rechazar formato incorrecto de fecha"
    );

    otra.fecha = "2026-02-30";

    r.comprobar(
        !g.registrarEvaluacion(otra, "DOC001"),
        "Rechazar fecha inexistente"
    );

    otra.fecha = "2027-02-29";

    r.comprobar(
        !g.registrarEvaluacion(otra, "DOC001"),
        "Rechazar 29 de febrero en anio no bisiesto"
    );

    otra = control;
    otra.codigo = "EV-PESO-0";
    otra.nombre = "Peso cero";
    otra.ponderacion = 0.0;

    r.comprobar(
        !g.registrarEvaluacion(otra, "DOC001"),
        "Rechazar ponderacion cero"
    );

    otra.ponderacion = -10.0;

    r.comprobar(
        !g.registrarEvaluacion(otra, "DOC001"),
        "Rechazar ponderacion negativa"
    );

    otra.ponderacion = 101.0;

    r.comprobar(
        !g.registrarEvaluacion(otra, "DOC001"),
        "Rechazar ponderacion mayor al 100%"
    );

    otra.ponderacion =
        numeric_limits<double>::infinity();

    r.comprobar(
        !g.registrarEvaluacion(otra, "DOC001"),
        "Rechazar ponderacion infinita"
    );

    otra.ponderacion =
        numeric_limits<double>::quiet_NaN();

    r.comprobar(
        !g.registrarEvaluacion(otra, "DOC001"),
        "Rechazar ponderacion NaN"
    );

    DatosEvaluacion final{
        "EV-002",
        "CUR-INF101-A",
        "Prueba final",
        "Prueba",
        "2026-11-10",
        60.0
    };

    r.comprobar(
        g.registrarEvaluacion(final, "DOC001"),
        "Registrar evaluacion de 60%"
    );

    r.comprobar(
        g.consultarEvaluaciones(
            "CUR-INF101-A"
        ).size() == 2,
        "Consultar dos evaluaciones registradas"
    );

    DatosEvaluacion excedida{
        "EV-EXCESO",
        "CUR-INF101-A",
        "Evaluacion adicional",
        "Control",
        "2026-12-01",
        5.0
    };

    r.comprobar(
        !g.registrarEvaluacion(excedida, "DOC001"),
        "Rechazar suma superior al 100%"
    );

    DatosEvaluacion otroCurso{
        "EV-MAT",
        "CUR-MAT101-A",
        "Control 1",
        "Control",
        "2026-09-10",
        100.0
    };

    r.comprobar(
        g.registrarEvaluacion(otroCurso, "DOC002"),
        "Permitir mismo nombre en otra asignatura"
    );

    r.comprobar(
        g.consultarEvaluaciones(
            "CUR-MAT101-A"
        ).size() == 1,
        "Mantener evaluaciones separadas por curso"
    );
}

// ==========================================
// GRUPO 2: REGISTRO DE CALIFICACIONES
// ==========================================

void probarRegistroNotas(ResumenPruebas& r) {

    r.iniciarGrupo(
        "AC-67: REGISTRO Y VALIDACION DE NOTAS"
    );

    EntornoPruebas e;

    auto& g = e.calificaciones;

    r.comprobar(
        e.inicializado && cargarEvaluaciones(e),
        "Preparar evaluaciones de prueba"
    );

    r.comprobar(
        !g.registrarNota(
            "NO-EXISTE", RUT1, 5.0, "DOC001"
        ),
        "Rechazar evaluacion inexistente"
    );

    r.comprobar(
        !g.registrarNota(
            "EV-001", RUT1, 5.0, "DOC002"
        ),
        "Rechazar docente incorrecto"
    );

    r.comprobar(
        !g.registrarNota(
            "EV-001", "99.999.999-9", 5.0, "DOC001"
        ),
        "Rechazar estudiante no matriculado"
    );

    r.comprobar(
        !g.registrarNota(
            "EV-001", RUT1, 0.9, "DOC001"
        ),
        "Rechazar nota inferior a 1.0"
    );

    r.comprobar(
        !g.registrarNota(
            "EV-001", RUT1, 7.1, "DOC001"
        ),
        "Rechazar nota superior a 7.0"
    );

    r.comprobar(
        !g.registrarNota(
            "EV-001",
            RUT1,
            numeric_limits<double>::infinity(),
            "DOC001"
        ),
        "Rechazar nota infinita"
    );

    r.comprobar(
        !g.registrarNota(
            "EV-001",
            RUT1,
            numeric_limits<double>::quiet_NaN(),
            "DOC001"
        ),
        "Rechazar nota NaN"
    );

    r.comprobar(
        g.registrarNota(
            "EV-001", RUT1, 1.0, "DOC001"
        ),
        "Aceptar nota minima 1.0"
    );

    r.comprobar(
        g.registrarNota(
            "EV-001", RUT2, 7.0, "DOC001"
        ),
        "Aceptar nota maxima 7.0"
    );

    r.comprobar(
        !g.registrarNota(
            "EV-001", RUT1, 5.0, "DOC001"
        ),
        "Rechazar calificacion duplicada"
    );

    const RegistroNota* n1 =
        g.buscarNota("EV-001", RUT1);

    const RegistroNota* n2 =
        g.buscarNota("EV-001", RUT2);

    r.comprobar(
        n1 != nullptr && sonIguales(n1->nota, 1.0),
        "Comprobar nota minima almacenada"
    );

    r.comprobar(
        n2 != nullptr && sonIguales(n2->nota, 7.0),
        "Comprobar nota maxima almacenada"
    );

    r.comprobar(
        n1 != nullptr && !n1->publicada,
        "Nota nueva permanece sin publicar"
    );

    r.comprobar(
        g.buscarNota("EV-002", RUT1) == nullptr,
        "Consulta de nota aun no registrada"
    );

    r.comprobar(
        g.consultarNotas(
            RUT1, "CUR-INF101-A", true
        ).empty(),
        "Ocultar notas no publicadas"
    );
}

// ==========================================
// GRUPO 3: EDICION Y PUBLICACION
// ==========================================

void probarEdicionPublicacion(ResumenPruebas& r) {

    r.iniciarGrupo(
        "AC-67 Y AC-70: EDICION, AUDITORIA Y PUBLICACION"
    );

    EntornoPruebas e;

    auto& g = e.calificaciones;

    r.comprobar(
        e.inicializado && cargarEvaluaciones(e),
        "Preparar evaluaciones"
    );

    r.comprobar(
        g.registrarNota(
            "EV-001", RUT1, 3.0, "DOC001"
        ),
        "Registrar nota original 3.0"
    );

    r.comprobar(
        !g.modificarNota(
            "EV-001", RUT1, 4.0, "", "DOC001"
        ),
        "Rechazar modificacion sin motivo"
    );

    r.comprobar(
        !g.modificarNota(
            "EV-001", RUT1, 0.5,
            "Correccion", "DOC001"
        ),
        "Rechazar nueva nota fuera de escala"
    );

    r.comprobar(
        !g.modificarNota(
            "EV-001", RUT1, 3.0,
            "Misma nota", "DOC001"
        ),
        "Rechazar modificacion sin cambios"
    );

    r.comprobar(
        !g.modificarNota(
            "EV-001", RUT1, 4.0,
            "Correccion", "DOC002"
        ),
        "Rechazar docente no autorizado"
    );

    r.comprobar(
        !g.modificarNota(
            "EV-002", RUT1, 4.0,
            "Correccion", "DOC001"
        ),
        "Rechazar modificacion de nota inexistente"
    );

    r.comprobar(
        !g.modificarNota(
            "NO-EXISTE", RUT1, 4.0,
            "Correccion", "DOC001"
        ),
        "Rechazar evaluacion inexistente"
    );

    r.comprobar(
        g.modificarNota(
            "EV-001", RUT1, 4.0,
            "Correccion de acta", "DOC001"
        ),
        "Modificar nota correctamente"
    );

    const RegistroNota* nota =
        g.buscarNota("EV-001", RUT1);

    r.comprobar(
        nota != nullptr &&
        sonIguales(nota->nota, 4.0),
        "Actualizar nota a 4.0"
    );

    r.comprobar(
        nota != nullptr &&
        nota->historial.size() == 1,
        "Registrar un movimiento en historial"
    );

    r.comprobar(
        nota != nullptr &&
        nota->historial.size() == 1 &&
        sonIguales(
            nota->historial[0].notaAnterior, 3.0
        ) &&
        sonIguales(
            nota->historial[0].notaNueva, 4.0
        ),
        "Conservar nota anterior y nueva"
    );

    r.comprobar(
        nota != nullptr &&
        nota->historial.size() == 1 &&
        nota->historial[0].responsable == "DOC001" &&
        nota->historial[0].motivo ==
            "Correccion de acta",
        "Conservar responsable y motivo"
    );

    r.comprobar(
        !g.publicarNota(
            "EV-002", RUT1, "DOC001"
        ),
        "Rechazar publicacion de nota inexistente"
    );

    r.comprobar(
        !g.publicarNota(
            "EV-001", RUT1, "DOC002"
        ),
        "Rechazar publicacion por docente incorrecto"
    );

    r.comprobar(
        g.publicarNota(
            "EV-001", RUT1, "DOC001"
        ),
        "Publicar nota correctamente"
    );

    r.comprobar(
        !g.publicarNota(
            "EV-001", RUT1, "DOC001"
        ),
        "Impedir doble publicacion"
    );

    r.comprobar(
        g.consultarNotas(
            RUT1, "CUR-INF101-A", true
        ).size() == 1,
        "Estudiante visualiza nota publicada"
    );

    r.comprobar(
        g.consultarNotas(
            RUT2, "CUR-INF101-A", true
        ).empty(),
        "No mostrar notas de otro estudiante"
    );

    r.comprobar(
        g.modificarNota(
            "EV-001", RUT1, 4.5,
            "Segunda correccion", "DOC001"
        ),
        "Modificar nuevamente una nota publicada"
    );

    nota = g.buscarNota("EV-001", RUT1);

    r.comprobar(
        nota != nullptr &&
        nota->publicada &&
        nota->historial.size() == 2,
        "Conservar publicacion e historial completo"
    );
}

// ==========================================
// GRUPO 4: PROMEDIOS Y CONDICIONES
// ==========================================

void probarPromedios(ResumenPruebas& r) {

    r.iniciarGrupo(
        "AC-68 Y AC-69: PROMEDIOS Y APROBACION"
    );

    EntornoPruebas e;

    auto& g = e.calificaciones;

    r.comprobar(
        e.inicializado,
        "Inicializar entorno"
    );

    ResultadoPromedio p =
        g.calcularPromedio(
            RUT1, "CUR-INF101-A"
        );

    r.comprobar(
        !p.disponible,
        "Sin evaluaciones no existe promedio"
    );

    r.comprobar(
        g.determinarCondicionPorNotas(
            RUT1, "CUR-INF101-A"
        ) == CondicionPorNotas::Pendiente,
        "Sin evaluaciones la condicion es Pendiente"
    );

    r.comprobar(
        cargarEvaluaciones(e),
        "Registrar ponderaciones 40% y 60%"
    );

    p = g.calcularPromedio(
        RUT1, "CUR-INF101-A"
    );

    r.comprobar(
        !p.disponible,
        "Sin notas no hay promedio calculable"
    );

    r.comprobar(
        g.registrarNota(
            "EV-001", RUT1, 3.0, "DOC001"
        ),
        "Registrar primera nota 3.0"
    );

    p = g.calcularPromedio(
        RUT1, "CUR-INF101-A"
    );

    r.comprobar(
        p.disponible &&
        !p.definitivo &&
        sonIguales(p.promedio, 3.0),
        "Promedio parcial 3.0"
    );

    r.comprobar(
        sonIguales(
            p.ponderacionEvaluada, 40.0
        ),
        "Ponderacion evaluada es 40%"
    );

    r.comprobar(
        g.determinarCondicionPorNotas(
            RUT1, "CUR-INF101-A"
        ) == CondicionPorNotas::Pendiente,
        "No reprobar con evaluaciones pendientes"
    );

    p = g.calcularPromedio(
        RUT1, "CUR-INF101-A", true
    );

    r.comprobar(
        !p.disponible,
        "Vista publicada no incluye notas ocultas"
    );

    r.comprobar(
        g.registrarNota(
            "EV-002", RUT1, 5.0, "DOC001"
        ),
        "Registrar segunda nota 5.0"
    );

    p = g.calcularPromedio(
        RUT1, "CUR-INF101-A"
    );

    r.comprobar(
        p.disponible &&
        p.definitivo &&
        sonIguales(p.promedio, 4.2),
        "Promedio definitivo 4.2"
    );

    r.comprobar(
        sonIguales(
            p.ponderacionEvaluada, 100.0
        ),
        "Promedio definitivo utiliza el 100%"
    );

    r.comprobar(
        g.determinarCondicionPorNotas(
            RUT1, "CUR-INF101-A"
        ) == CondicionPorNotas::Aprobado,
        "Promedio 4.2 aprueba por notas"
    );

    r.comprobar(
        g.publicarNota(
            "EV-001", RUT1, "DOC001"
        ),
        "Publicar primera nota"
    );

    p = g.calcularPromedio(
        RUT1, "CUR-INF101-A", true
    );

    r.comprobar(
        p.disponible &&
        !p.definitivo &&
        sonIguales(p.promedio, 3.0),
        "Promedio publico sigue parcial"
    );

    r.comprobar(
        g.publicarNota(
            "EV-002", RUT1, "DOC001"
        ),
        "Publicar segunda nota"
    );

    p = g.calcularPromedio(
        RUT1, "CUR-INF101-A", true
    );

    r.comprobar(
        p.disponible &&
        p.definitivo &&
        sonIguales(p.promedio, 4.2),
        "Promedio publico definitivo 4.2"
    );

    r.comprobar(
        g.modificarNota(
            "EV-001", RUT1, 4.0,
            "Correccion", "DOC001"
        ),
        "Modificar primera calificacion"
    );

    p = g.calcularPromedio(
        RUT1, "CUR-INF101-A"
    );

    r.comprobar(
        p.definitivo &&
        sonIguales(p.promedio, 4.6),
        "Recalcular promedio despues de modificar: 4.6"
    );

    r.comprobar(
        g.registrarNota(
            "EV-001", RUT2, 2.0, "DOC001"
        ),
        "Registrar primera nota reprobatoria"
    );

    r.comprobar(
        g.registrarNota(
            "EV-002", RUT2, 3.0, "DOC001"
        ),
        "Registrar segunda nota reprobatoria"
    );

    p = g.calcularPromedio(
        RUT2, "CUR-INF101-A"
    );

    r.comprobar(
        p.definitivo &&
        sonIguales(p.promedio, 2.6),
        "Calcular promedio 2.6"
    );

    r.comprobar(
        g.determinarCondicionPorNotas(
            RUT2, "CUR-INF101-A"
        ) == CondicionPorNotas::Reprobado,
        "Promedio inferior a 4.0 reprueba por notas"
    );

    p = g.calcularPromedio(
        "99.999.999-9",
        "CUR-INF101-A"
    );

    r.comprobar(
        !p.disponible,
        "No calcular promedio sin matricula vigente"
    );
}

// ==========================================
// GRUPO 5: NOTA EXACTA DE APROBACION
// ==========================================

void probarLimitesAprobacion(ResumenPruebas& r) {

    r.iniciarGrupo(
        "AC-69: LIMITES DE APROBACION"
    );

    EntornoPruebas e;

    auto& g = e.calificaciones;

    r.comprobar(
        e.inicializado,
        "Preparar entorno de limites"
    );

    r.comprobar(
        g.registrarEvaluacion({
            "EV-UNICA",
            "CUR-INF101-A",
            "Evaluacion unica",
            "Prueba",
            "2026-10-10",
            100.0
        }, "DOC001"),
        "Registrar evaluacion ponderada al 100%"
    );

    r.comprobar(
        g.registrarNota(
            "EV-UNICA", RUT1, 4.0, "DOC001"
        ),
        "Registrar nota exacta 4.0"
    );

    ResultadoPromedio p =
        g.calcularPromedio(
            RUT1, "CUR-INF101-A"
        );

    r.comprobar(
        p.definitivo &&
        sonIguales(p.promedio, 4.0),
        "Calcular promedio exacto 4.0"
    );

    r.comprobar(
        g.determinarCondicionPorNotas(
            RUT1, "CUR-INF101-A"
        ) == CondicionPorNotas::Aprobado,
        "Nota 4.0 es aprobatoria"
    );

    r.comprobar(
        g.registrarNota(
            "EV-UNICA", RUT2, 3.99, "DOC001"
        ),
        "Registrar nota 3.99 para comprobar limite"
    );

    r.comprobar(
        g.determinarCondicionPorNotas(
            RUT2, "CUR-INF101-A"
        ) == CondicionPorNotas::Reprobado,
        "Nota 3.99 es reprobatoria sin redondeo"
    );

    r.comprobar(
        g.modificarNota(
            "EV-UNICA",
            RUT2,
            4.0,
            "Correccion de prueba",
            "DOC001"
        ),
        "Corregir nota 3.99 a 4.0"
    );

    r.comprobar(
        g.determinarCondicionPorNotas(
            RUT2, "CUR-INF101-A"
        ) == CondicionPorNotas::Aprobado,
        "Actualizar condicion luego de correccion"
    );
}

// ==========================================
// GRUPO 6: INTEGRACION ENTRE MODULOS
// ==========================================

void probarIntegracion(ResumenPruebas& r) {

    r.iniciarGrupo(
        "INTEGRACION: CURSOS, MATRICULAS Y CALIFICACIONES"
    );

    EntornoPruebas e;

    auto& g = e.calificaciones;

    r.comprobar(
        e.inicializado,
        "Inicializar modulos relacionados"
    );

    r.comprobar(
        g.registrarEvaluacion({
            "EV-MAT",
            "CUR-MAT101-A",
            "Control matematicas",
            "Control",
            "2026-10-01",
            100.0
        }, "DOC002"),
        "Registrar evaluacion en segunda asignatura"
    );

    r.comprobar(
        !g.registrarNota(
            "EV-MAT", RUT1, 6.0, "DOC002"
        ),
        "Rechazar nota de estudiante no matriculado"
    );

    r.comprobar(
        e.matriculas.registrarMatricula({
            "MAT-MAT-001",
            RUT1,
            "CUR-MAT101-A",
            "MAT101",
            "PA-2026-2"
        }),
        "Matricular estudiante en segundo curso"
    );

    r.comprobar(
        g.registrarNota(
            "EV-MAT", RUT1, 6.0, "DOC002"
        ),
        "Permitir nota tras matricula vigente"
    );

    ResultadoPromedio p =
        g.calcularPromedio(
            RUT1, "CUR-MAT101-A"
        );

    r.comprobar(
        p.definitivo &&
        sonIguales(p.promedio, 6.0),
        "Calcular promedio de segunda asignatura"
    );

    r.comprobar(
        g.consultarNotas(
            RUT1, "CUR-INF101-A", false
        ).empty(),
        "No mezclar notas de distintas asignaturas"
    );

    r.comprobar(
        e.matriculas.anularMatricula(
            "MAT-002",
            "Solicitud de retiro",
            "ADMIN001"
        ),
        "Anular matricula de estudiante 2"
    );

    r.comprobar(
        !g.registrarNota(
            "EV-MAT", RUT2, 5.0, "DOC002"
        ),
        "Impedir nota en otro curso sin matricula"
    );

    r.comprobar(
        e.cursos.cerrarCurso("CUR-MAT101-A"),
        "Cerrar segundo curso"
    );

    r.comprobar(
        !g.registrarEvaluacion({
            "EV-CERRADA",
            "CUR-MAT101-A",
            "Evaluacion tardia",
            "Control",
            "2026-12-01",
            10.0
        }, "DOC002"),
        "Impedir crear evaluaciones en curso cerrado"
    );
}

// ==========================================
// EJECUTAR TODAS LAS PRUEBAS
// ==========================================

void ejecutarPruebasAutomaticas() {

    cout << "\n========================================\n";
    cout << "    ACADEMIA - PRUEBAS DE CALIFICACIONES\n";
    cout << "========================================\n";

    ResumenPruebas resultado;

    probarEvaluaciones(resultado);
    probarRegistroNotas(resultado);
    probarEdicionPublicacion(resultado);
    probarPromedios(resultado);
    probarLimitesAprobacion(resultado);
    probarIntegracion(resultado);

    resultado.finalizar();
}

// ==========================================
// ENTRADA DE DATOS INTERACTIVA
// ==========================================

string leerTexto(const string& mensaje) {

    string texto;

    cout << mensaje << ": ";

    if (!getline(cin, texto)) {
        cout << "\nEntrada finalizada.\n";
        exit(0);
    }

    return texto;
}

double leerDecimal(const string& mensaje) {

    while (true) {

        string texto = leerTexto(mensaje);

        for (char& caracter : texto) {
            if (caracter == ',') {
                caracter = '.';
            }
        }

        istringstream flujo(texto);

        double valor;
        char extra;

        if (
            (flujo >> valor) &&
            !(flujo >> extra) &&
            isfinite(valor)
        ) {
            return valor;
        }

        cout << "Ingrese un numero valido.\n";
    }
}

int leerOpcion() {

    while (true) {

        string texto = leerTexto(
            "Seleccione una opcion"
        );

        istringstream flujo(texto);

        int opcion;
        char extra;

        if (
            (flujo >> opcion) &&
            !(flujo >> extra)
        ) {
            return opcion;
        }

        cout << "Ingrese una opcion valida.\n";
    }
}

// ==========================================
// MOSTRAR EVALUACIONES
// ==========================================

void mostrarEvaluaciones(
    const EntornoPruebas& e
) {

    string curso = leerTexto(
        "Codigo del curso"
    );

    auto lista =
        e.calificaciones.consultarEvaluaciones(curso);

    if (lista.empty()) {
        cout << "No existen evaluaciones.\n";
        return;
    }

    double total = 0.0;

    for (const Evaluacion& evaluacion : lista) {

        const auto& d =
            evaluacion.obtenerDatos();

        cout << d.codigo
             << " | "
             << d.nombre
             << " | "
             << d.tipo
             << " | "
             << d.fecha
             << " | "
             << d.ponderacion
             << "%\n";

        total += d.ponderacion;
    }

    cout << "Ponderacion total: "
         << total << "%\n";
}

// ==========================================
// MENU: REGISTRAR EVALUACION
// ==========================================

void menuRegistrarEvaluacion(
    EntornoPruebas& e
) {

    DatosEvaluacion d{};

    cout << "\n=== REGISTRAR EVALUACION ===\n";

    d.codigo = leerTexto("Codigo evaluacion");
    d.codigoCurso = leerTexto("Codigo curso");
    d.nombre = leerTexto("Nombre");
    d.tipo = leerTexto("Tipo");
    d.fecha = leerTexto("Fecha AAAA-MM-DD");
    d.ponderacion = leerDecimal("Ponderacion %");

    string docente = leerTexto("Codigo docente");

    if (
        e.calificaciones.registrarEvaluacion(
            d, docente
        )
    ) {
        cout << "[OK] Evaluacion registrada.\n";
    } else {
        cout << "[ERROR] "
             << e.calificaciones.obtenerUltimoError()
             << "\n";
    }
}

// ==========================================
// MENU: REGISTRAR NOTA
// ==========================================

void menuRegistrarNota(EntornoPruebas& e) {

    cout << "\n=== REGISTRAR NOTA ===\n";

    string evaluacion =
        leerTexto("Codigo evaluacion");

    string rut =
        leerTexto("RUT estudiante");

    double nota =
        leerDecimal("Nota entre 1.0 y 7.0");

    string docente =
        leerTexto("Codigo docente");

    if (
        e.calificaciones.registrarNota(
            evaluacion, rut, nota, docente
        )
    ) {
        cout << "[OK] Nota registrada.\n";
    } else {
        cout << "[ERROR] "
             << e.calificaciones.obtenerUltimoError()
             << "\n";
    }
}

// ==========================================
// MENU: MODIFICAR NOTA
// ==========================================

void menuModificarNota(EntornoPruebas& e) {

    cout << "\n=== MODIFICAR NOTA ===\n";

    string evaluacion =
        leerTexto("Codigo evaluacion");

    string rut =
        leerTexto("RUT estudiante");

    double nota =
        leerDecimal("Nueva nota");

    string motivo =
        leerTexto("Motivo");

    string docente =
        leerTexto("Codigo docente");

    if (
        e.calificaciones.modificarNota(
            evaluacion, rut, nota, motivo, docente
        )
    ) {
        cout << "[OK] Nota modificada.\n";
    } else {
        cout << "[ERROR] "
             << e.calificaciones.obtenerUltimoError()
             << "\n";
    }
}

// ==========================================
// MENU: PUBLICAR NOTA
// ==========================================

void menuPublicarNota(EntornoPruebas& e) {

    cout << "\n=== PUBLICAR NOTA ===\n";

    string evaluacion =
        leerTexto("Codigo evaluacion");

    string rut =
        leerTexto("RUT estudiante");

    string docente =
        leerTexto("Codigo docente");

    if (
        e.calificaciones.publicarNota(
            evaluacion, rut, docente
        )
    ) {
        cout << "[OK] Nota publicada.\n";
    } else {
        cout << "[ERROR] "
             << e.calificaciones.obtenerUltimoError()
             << "\n";
    }
}

// ==========================================
// MENU: CONSULTAR NOTAS
// ==========================================

void menuConsultarNotas(
    const EntornoPruebas& e
) {

    cout << "\n=== CONSULTAR NOTAS ===\n";

    string rut = leerTexto("RUT estudiante");
    string curso = leerTexto("Codigo curso");

    string respuesta =
        leerTexto("Mostrar no publicadas? (S/N)");

    bool soloPublicadas =
        respuesta != "S" && respuesta != "s";

    auto notas = e.calificaciones.consultarNotas(
        rut,
        curso,
        soloPublicadas
    );

    if (notas.empty()) {
        cout << "No existen notas visibles.\n";
        return;
    }

    for (const auto& nota : notas) {

        cout << "\nEvaluacion: "
             << nota.codigoEvaluacion
             << "\n";

        cout << "Nota: "
             << nota.nota << "\n";

        cout << "Estado: "
             << (
                 nota.publicada
                 ? "Publicada"
                 : "No publicada"
             )
             << "\n";

        cout << "Cambios registrados: "
             << nota.historial.size()
             << "\n";

        for (const auto& cambio : nota.historial) {

            cout << "  "
                 << cambio.notaAnterior
                 << " -> "
                 << cambio.notaNueva
                 << " | "
                 << cambio.motivo
                 << " | "
                 << cambio.responsable
                 << " | "
                 << cambio.fechaHora
                 << "\n";
        }
    }
}

// ==========================================
// MENU: CALCULAR PROMEDIO
// ==========================================

void menuConsultarPromedio(
    const EntornoPruebas& e
) {

    cout << "\n=== CONSULTAR PROMEDIO ===\n";

    string rut = leerTexto("RUT estudiante");
    string curso = leerTexto("Codigo curso");

    ResultadoPromedio p =
        e.calificaciones.calcularPromedio(
            rut, curso
        );

    cout << p.mensaje << "\n";

    if (p.disponible) {

        cout << fixed
             << setprecision(2);

        cout << "Promedio: "
             << p.promedio << "\n";

        cout << "Ponderacion evaluada: "
             << p.ponderacionEvaluada << "%\n";

        cout << "Tipo: "
             << (
                 p.definitivo
                 ? "Definitivo"
                 : "Parcial"
             )
             << "\n";
    }

    CondicionPorNotas condicion =
        e.calificaciones.determinarCondicionPorNotas(
            rut, curso
        );

    if (condicion == CondicionPorNotas::Aprobado) {

        cout << "APRUEBA POR NOTAS\n";

    } else if (
        condicion == CondicionPorNotas::Reprobado
    ) {

        cout << "REPRUEBA POR NOTAS\n";

    } else {

        cout << "RESULTADO PENDIENTE\n";
    }

    cout << "No se han integrado otras "
         << "condiciones institucionales.\n";
}

// ==========================================
// MOSTRAR DATOS DE PRUEBA
// ==========================================

void mostrarCatalogos(const EntornoPruebas& e) {

    cout << "\n=== CATALOGOS DE PRUEBA ===\n";

    cout << "Periodo: PA-2026-2\n";

    cout << "Cursos:\n";
    cout << "  CUR-INF101-A | Docente DOC001\n";
    cout << "  CUR-MAT101-A | Docente DOC002\n";

    cout << "Estudiantes simulados:\n";
    cout << "  " << RUT1 << "\n";
    cout << "  " << RUT2 << "\n";

    cout << "Ambos estan matriculados "
         << "en CUR-INF101-A.\n";

    cout << "Evaluaciones registradas "
         << "en INF101: "
         << e.calificaciones.consultarEvaluaciones(
                "CUR-INF101-A"
            ).size()
         << "\n";

    cout << "Estos datos no estan conectados "
         << "todavia a PostgreSQL.\n";
}

// ==========================================
// MENU PRINCIPAL
// ==========================================

void mostrarMenu() {

    cout << "\n========================================\n";
    cout << " ACADEMIA - EVALUACIONES Y CALIFICACIONES\n";
    cout << "========================================\n";

    cout << "1. Registrar evaluacion\n";
    cout << "2. Listar evaluaciones\n";
    cout << "3. Registrar nota\n";
    cout << "4. Modificar nota\n";
    cout << "5. Publicar nota\n";
    cout << "6. Consultar notas\n";
    cout << "7. Consultar promedio y condicion\n";
    cout << "8. Ver catalogos de prueba\n";
    cout << "9. Ejecutar pruebas automaticas ampliadas\n";
    cout << "0. Salir\n";
}

// ==========================================
// PROGRAMA PRINCIPAL
// ==========================================

int main() {

    cout << fixed << setprecision(2);

    EntornoPruebas entorno;

    if (!entorno.inicializado) {

        cerr << "ERROR: No fue posible inicializar "
             << "los datos academicos.\n";

        return 1;
    }

    while (true) {

        mostrarMenu();

        int opcion = leerOpcion();

        switch (opcion) {

            case 1:
                menuRegistrarEvaluacion(entorno);
                break;

            case 2:
                mostrarEvaluaciones(entorno);
                break;

            case 3:
                menuRegistrarNota(entorno);
                break;

            case 4:
                menuModificarNota(entorno);
                break;

            case 5:
                menuPublicarNota(entorno);
                break;

            case 6:
                menuConsultarNotas(entorno);
                break;

            case 7:
                menuConsultarPromedio(entorno);
                break;

            case 8:
                mostrarCatalogos(entorno);
                break;

            case 9:
                ejecutarPruebasAutomaticas();
                break;

            case 0:
                cout << "\nSaliendo de AcademIA...\n";
                return 0;

            default:
                cout << "Opcion invalida.\n";
                break;
        }
    }
}
