
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <sstream>
#include <cstdlib>

#include "Matricula.hpp"
#include "GestorMatriculas.hpp"
#include "../cursos/GestorCursos.hpp"
#include "../periodos/GestorPeriodos.hpp"

using namespace std;

// ==========================================
// DATOS DE PRUEBA
// ==========================================

const string ESTUDIANTE_1 = "12.345.678-5";
const string ESTUDIANTE_2 = "11.111.111-1";
const string ESTUDIANTE_3 = "9.876.543-3";
const string ESTUDIANTE_INACTIVO = "22.222.222-2";

// ==========================================
// ENTORNO DE PRUEBAS
// ==========================================

struct EntornoPruebas {

    GestorPeriodos periodos;

    set<string> asignaturas;
    set<string> docentes;
    set<string> estudiantesActivos;

    map<string, set<string>> asignaturasAprobadas;
    map<string, vector<string>> prerrequisitos;

    GestorCursos cursos;
    GestorMatriculas matriculas;

    bool datosInicialesCorrectos = true;

    EntornoPruebas()
        : asignaturas({
              "INF101",
              "INF102",
              "MAT101",
              "MAT102",
              "FIS101"
          }),
          docentes({
              "DOC001",
              "DOC002",
              "DOC003"
          }),
          estudiantesActivos({
              ESTUDIANTE_1,
              ESTUDIANTE_2,
              ESTUDIANTE_3
          }),
          asignaturasAprobadas({
              {ESTUDIANTE_1, {"INF101"}},
              {ESTUDIANTE_2, {}},
              {ESTUDIANTE_3, {"MAT101"}}
          }),
          prerrequisitos({
              {"INF101", {}},
              {"INF102", {"INF101"}},
              {"MAT101", {}},
              {"MAT102", {"MAT101"}},
              {"FIS101", {"MAT101"}}
          }),
          cursos(
              [this](const string& codigo) {

                  return periodos.permiteMatricula(
                      codigo
                  );
              },

              [this](const string& codigo) {

                  return asignaturas.count(codigo) > 0;
              },

              [this](const string& codigo) {

                  return docentes.count(codigo) > 0;
              }
          ),
          matriculas(
              cursos,
              periodos,

              [this](const string& rut) {

                  return estudiantesActivos.count(rut) > 0;
              },

              [this](
                  const string& rut,
                  const string& asignatura
              ) {

                  if (
                      estudiantesActivos.count(rut) == 0 ||
                      asignaturas.count(asignatura) == 0
                  ) {
                      return false;
                  }

                  auto requisitos =
                      prerrequisitos.find(asignatura);

                  if (requisitos == prerrequisitos.end()) {
                      return false;
                  }

                  auto aprobadas =
                      asignaturasAprobadas.find(rut);

                  for (
                      const string& requisito :
                      requisitos->second
                  ) {

                      if (
                          aprobadas ==
                          asignaturasAprobadas.end()
                      ) {
                          return false;
                      }

                      if (
                          aprobadas->second.count(
                              requisito
                          ) == 0
                      ) {
                          return false;
                      }
                  }

                  return true;
              }
          ) {

        // ==================================
        // PERIODOS ACADEMICOS
        // ==================================

        datosInicialesCorrectos &=
            periodos.crearPeriodo({
                "PA-2026-2",
                "Segundo semestre 2026",
                2026,
                2,
                "2026-08-03",
                "2026-12-18"
            });

        datosInicialesCorrectos &=
            periodos.crearPeriodo({
                "PA-2027-1",
                "Primer semestre 2027",
                2027,
                1,
                "2027-03-01",
                "2027-07-16"
            });

        // ==================================
        // CURSOS Y SECCIONES
        // ==================================

        DatosCurso inf101A{
            "CUR-INF101-A",
            "INF101",
            "PA-2026-2",
            "A",
            2,
            "Presencial",
            {{1, 510, 600}}
        };

        DatosCurso inf101B{
            "CUR-INF101-B",
            "INF101",
            "PA-2026-2",
            "B",
            2,
            "Presencial",
            {{2, 510, 600}}
        };

        DatosCurso inf102A{
            "CUR-INF102-A",
            "INF102",
            "PA-2026-2",
            "A",
            3,
            "Presencial",
            {{1, 600, 690}}
        };

        DatosCurso mat101A{
            "CUR-MAT101-A",
            "MAT101",
            "PA-2026-2",
            "A",
            3,
            "Presencial",
            {{1, 510, 600}}
        };

        DatosCurso mat101B{
            "CUR-MAT101-B",
            "MAT101",
            "PA-2026-2",
            "B",
            3,
            "Presencial",
            {{3, 660, 750}}
        };

        DatosCurso mat102A{
            "CUR-MAT102-A",
            "MAT102",
            "PA-2026-2",
            "A",
            3,
            "Presencial",
            {{3, 780, 870}}
        };

        datosInicialesCorrectos &=
            cursos.registrarCurso(inf101A);

        datosInicialesCorrectos &=
            cursos.registrarCurso(inf101B);

        datosInicialesCorrectos &=
            cursos.registrarCurso(inf102A);

        datosInicialesCorrectos &=
            cursos.registrarCurso(mat101A);

        datosInicialesCorrectos &=
            cursos.registrarCurso(mat101B);

        datosInicialesCorrectos &=
            cursos.registrarCurso(mat102A);
    }
};

// ==========================================
// FUNCIONES AUXILIARES
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

int leerNumero(const string& mensaje) {

    while (true) {

        string entrada = leerTexto(mensaje);

        istringstream flujo(entrada);

        int numero;
        char extra;

        if (
            flujo >> numero &&
            !(flujo >> extra)
        ) {
            return numero;
        }

        cout << "Ingrese un numero valido.\n";
    }
}

bool confirmarOperacion() {

    string respuesta =
        leerTexto("Confirmar operacion (S/N)");

    return respuesta == "S" ||
           respuesta == "s";
}

// ==========================================
// CREAR DATOS DE MATRICULA
// ==========================================

DatosMatricula crearDatosMatricula(
    const EntornoPruebas& entorno,
    const string& codigo,
    const string& rut,
    const string& codigoCurso
) {

    DatosMatricula datos{};

    datos.codigo = codigo;
    datos.rutEstudiante = rut;
    datos.codigoCurso = codigoCurso;

    const Curso* curso =
        entorno.cursos.buscarCurso(codigoCurso);

    if (curso != nullptr) {

        datos.codigoAsignatura =
            curso->obtenerDatos().codigoAsignatura;

        datos.codigoPeriodo =
            curso->obtenerDatos().codigoPeriodo;
    }

    return datos;
}

// ==========================================
// MOSTRAR DATOS DE MATRICULA
// ==========================================

void mostrarMatricula(const Matricula& matricula) {

    const DatosMatricula& datos =
        matricula.obtenerDatos();

    cout << "\n----------------------------------------\n";

    cout << "Codigo matricula: "
         << datos.codigo << "\n";

    cout << "RUT estudiante: "
         << datos.rutEstudiante << "\n";

    cout << "Curso: "
         << datos.codigoCurso << "\n";

    cout << "Asignatura: "
         << datos.codigoAsignatura << "\n";

    cout << "Periodo: "
         << datos.codigoPeriodo << "\n";

    cout << "Estado: "
         << matricula.obtenerEstadoTexto() << "\n";

    cout << "Fecha registro: "
         << matricula.obtenerFechaRegistro() << "\n";

    cout << "Movimientos historicos: "
         << matricula.obtenerHistorial().size()
         << "\n";

    cout << "----------------------------------------\n";
}

// ==========================================
// MOSTRAR HISTORIAL
// ==========================================

void mostrarHistorial(const Matricula& matricula) {

    const auto& historial =
        matricula.obtenerHistorial();

    cout << "\n=== HISTORIAL DE MATRICULA ===\n";

    mostrarMatricula(matricula);

    if (historial.empty()) {

        cout << "\nNo existen movimientos registrados.\n";
        return;
    }

    for (
        size_t i = 0;
        i < historial.size();
        i++
    ) {

        const MovimientoMatricula& movimiento =
            historial[i];

        cout << "\nMovimiento "
             << i + 1 << "\n";

        cout << "Tipo: ";

        if (
            movimiento.tipo ==
            TipoMovimientoMatricula::CambioSeccion
        ) {
            cout << "Cambio de seccion\n";
        } else {
            cout << "Anulacion\n";
        }

        cout << "Curso anterior: "
             << movimiento.codigoCursoAnterior
             << "\n";

        cout << "Curso nuevo: "
             << (
                 movimiento.codigoCursoNuevo.empty()
                 ? "No aplica"
                 : movimiento.codigoCursoNuevo
             )
             << "\n";

        cout << "Motivo: "
             << movimiento.motivo << "\n";

        cout << "Responsable: "
             << movimiento.responsable << "\n";

        cout << "Fecha: "
             << movimiento.fechaHora << "\n";
    }
}

// ==========================================
// CONSULTAR CUPOS
// ==========================================

void mostrarCupos(
    const GestorCursos& gestor,
    const string& codigoCurso
) {

    const Curso* curso =
        gestor.buscarCurso(codigoCurso);

    if (curso == nullptr) {

        cout << "\nCurso no encontrado.\n";
        return;
    }

    const DatosCurso& datos =
        curso->obtenerDatos();

    cout << "\n----------------------------------------\n";
    cout << "Curso: " << datos.codigo << "\n";
    cout << "Asignatura: "
         << datos.codigoAsignatura << "\n";
    cout << "Seccion: "
         << datos.seccion << "\n";
    cout << "Capacidad maxima: "
         << datos.capacidadMaxima << "\n";
    cout << "Inscritos vigentes: "
         << curso->obtenerCantidadInscritos() << "\n";
    cout << "Cupos disponibles: "
         << curso->obtenerCuposDisponibles() << "\n";
    cout << "----------------------------------------\n";
}

// ==========================================
// MOSTRAR CATALOGOS DE PRUEBA
// ==========================================

void mostrarCatalogos(const EntornoPruebas& entorno) {

    cout << "\n=== CATALOGOS DE PRUEBA ===\n";

    cout << "\nPERIODOS ACADEMICOS:\n";

    for (
        const PeriodoAcademico& periodo :
        entorno.periodos.listarPeriodos()
    ) {

        cout << "- "
             << periodo.obtenerDatos().codigo
             << " ("
             << periodo.obtenerEstadoTexto()
             << ")\n";
    }

    cout << "\nESTUDIANTES SIMULADOS:\n";

    cout << ESTUDIANTE_1
         << " | Activo | Aprobado: INF101\n";

    cout << ESTUDIANTE_2
         << " | Activo | Sin asignaturas aprobadas\n";

    cout << ESTUDIANTE_3
         << " | Activo | Aprobado: MAT101\n";

    cout << ESTUDIANTE_INACTIVO
         << " | Inactivo\n";

    cout << "\nPRERREQUISITOS:\n";
    cout << "INF101: Ninguno\n";
    cout << "INF102: INF101\n";
    cout << "MAT101: Ninguno\n";
    cout << "MAT102: MAT101\n";
    cout << "FIS101: MAT101\n";

    cout << "\nCURSOS DISPONIBLES:\n";

    for (
        const Curso& curso :
        entorno.cursos.listarCursos()
    ) {

        const DatosCurso& datos =
            curso.obtenerDatos();

        cout << "- "
             << datos.codigo
             << " | "
             << datos.codigoAsignatura
             << " | Seccion "
             << datos.seccion
             << " | Cupos: "
             << curso.obtenerCuposDisponibles()
             << "\n";
    }

    cout << "\nIMPORTANTE:\n";
    cout << "Estudiantes, aprobaciones y prerrequisitos\n";
    cout << "son datos simulados para estas pruebas.\n";
}

// ==========================================
// AC-62: CONSULTAR PRERREQUISITOS
// ==========================================

void consultarPrerequisitos(
    const EntornoPruebas& entorno
) {

    cout << "\n=== VALIDAR PRERREQUISITOS ===\n";

    string rut = leerTexto("RUT estudiante");

    string asignatura =
        leerTexto("Codigo asignatura");

    auto requisitos =
        entorno.prerrequisitos.find(asignatura);

    if (requisitos == entorno.prerrequisitos.end()) {

        cout << "\nAsignatura no registrada "
             << "en el catalogo de pruebas.\n";

        return;
    }

    cout << "\nAsignatura: "
         << asignatura << "\n";

    if (requisitos->second.empty()) {

        cout << "No exige prerrequisitos.\n";

    } else {

        cout << "Prerrequisitos exigidos:\n";

        for (const string& codigo : requisitos->second) {
            cout << "- " << codigo << "\n";
        }
    }

    bool cumple =
        entorno.matriculas.verificarPrerequisitos(
            rut,
            asignatura
        );

    if (cumple) {

        cout << "\nRESULTADO: CUMPLE LOS PRERREQUISITOS.\n";

    } else {

        cout << "\nRESULTADO: NO HABILITADO.\n";
        cout << "Revise el estudiante y las "
             << "asignaturas aprobadas.\n";
    }
}

// ==========================================
// AC-61: REGISTRAR MATRICULA
// ==========================================

void registrarMatricula(EntornoPruebas& entorno) {

    cout << "\n=== REGISTRAR MATRICULA ===\n";

    string codigo =
        leerTexto("Codigo de matricula");

    string rut =
        leerTexto("RUT del estudiante");

    string curso =
        leerTexto("Codigo del curso");

    DatosMatricula datos = crearDatosMatricula(
        entorno,
        codigo,
        rut,
        curso
    );

    if (!confirmarOperacion()) {

        cout << "\nRegistro cancelado.\n";
        return;
    }

    if (
        entorno.matriculas.registrarMatricula(
            datos
        )
    ) {

        cout << "\nMATRICULA REGISTRADA CORRECTAMENTE.\n";

        const Matricula* registrada =
            entorno.matriculas.buscarMatricula(codigo);

        if (registrada != nullptr) {
            mostrarMatricula(*registrada);
        }

        mostrarCupos(entorno.cursos, curso);

    } else {

        cout << "\nNO SE PUDO REGISTRAR LA MATRICULA.\n";

        cout << "Motivo: "
             << entorno.matriculas.obtenerUltimoError()
             << "\n";
    }
}

// ==========================================
// LISTAR MATRICULAS
// ==========================================

void listarMatriculas(const GestorMatriculas& gestor) {

    cout << "\n=== LISTADO DE MATRICULAS ===\n";

    vector<Matricula> matriculas =
        gestor.listarMatriculas();

    if (matriculas.empty()) {

        cout << "\nNo existen matriculas registradas.\n";
        return;
    }

    cout << "\nTotal de registros: "
         << matriculas.size() << "\n";

    for (const Matricula& matricula : matriculas) {
        mostrarMatricula(matricula);
    }
}

// ==========================================
// BUSCAR MATRICULA
// ==========================================

void buscarMatricula(const GestorMatriculas& gestor) {

    cout << "\n=== BUSCAR MATRICULA ===\n";

    string codigo =
        leerTexto("Codigo de matricula");

    const Matricula* matricula =
        gestor.buscarMatricula(codigo);

    if (matricula == nullptr) {

        cout << "\nMatricula no encontrada.\n";
        return;
    }

    mostrarMatricula(*matricula);
}

// ==========================================
// AC-63: CONSULTAR INSCRITOS POR CURSO
// ==========================================

void consultarInscritos(
    const EntornoPruebas& entorno
) {

    cout << "\n=== ESTUDIANTES INSCRITOS ===\n";

    string codigoCurso =
        leerTexto("Codigo del curso");

    if (
        entorno.cursos.buscarCurso(codigoCurso) ==
        nullptr
    ) {

        cout << "\nEl curso no existe.\n";
        return;
    }

    vector<Matricula> inscritos =
        entorno.matriculas.consultarInscritos(
            codigoCurso
        );

    cout << "\nTotal de inscritos vigentes: "
         << inscritos.size() << "\n";

    if (inscritos.empty()) {

        cout << "\nNo existen estudiantes inscritos.\n";

    } else {

        for (const Matricula& matricula : inscritos) {
            mostrarMatricula(matricula);
        }
    }

    mostrarCupos(entorno.cursos, codigoCurso);
}

// ==========================================
// CONSULTAR MATRICULAS POR ESTUDIANTE
// ==========================================

void consultarPorEstudiante(
    const GestorMatriculas& gestor
) {

    cout << "\n=== MATRICULAS POR ESTUDIANTE ===\n";

    string rut = leerTexto("RUT estudiante");

    cout << "\n1. Todas las matriculas\n";
    cout << "2. Solo matriculas vigentes\n";

    int opcion = leerNumero("Seleccione una opcion");

    bool incluirAnuladas = opcion != 2;

    vector<Matricula> resultado =
        gestor.consultarPorEstudiante(
            rut,
            incluirAnuladas
        );

    if (resultado.empty()) {

        cout << "\nNo existen matriculas encontradas.\n";
        return;
    }

    for (const Matricula& matricula : resultado) {
        mostrarMatricula(matricula);
    }
}

// ==========================================
// AC-64: CAMBIAR SECCION
// ==========================================

void cambiarSeccion(EntornoPruebas& entorno) {

    cout << "\n=== CAMBIAR SECCION ===\n";

    string codigo =
        leerTexto("Codigo de matricula");

    const Matricula* matricula =
        entorno.matriculas.buscarMatricula(codigo);

    if (matricula == nullptr) {

        cout << "\nMatricula no encontrada.\n";
        return;
    }

    mostrarMatricula(*matricula);

    string codigoAnterior =
        matricula->obtenerDatos().codigoCurso;

    string nuevoCurso =
        leerTexto("Codigo del nuevo curso");

    string motivo =
        leerTexto("Motivo del cambio");

    string responsable =
        leerTexto("Responsable del cambio");

    if (!confirmarOperacion()) {

        cout << "\nCambio cancelado.\n";
        return;
    }

    if (
        entorno.matriculas.cambiarSeccion(
            codigo,
            nuevoCurso,
            motivo,
            responsable
        )
    ) {

        cout << "\nSECCION MODIFICADA CORRECTAMENTE.\n";

        const Matricula* actualizada =
            entorno.matriculas.buscarMatricula(codigo);

        if (actualizada != nullptr) {
            mostrarMatricula(*actualizada);
        }

        cout << "\nCupos del curso anterior:\n";
        mostrarCupos(
            entorno.cursos,
            codigoAnterior
        );

        cout << "\nCupos del nuevo curso:\n";
        mostrarCupos(
            entorno.cursos,
            nuevoCurso
        );

    } else {

        cout << "\nNO SE PUDO CAMBIAR LA SECCION.\n";

        cout << "Motivo: "
             << entorno.matriculas.obtenerUltimoError()
             << "\n";
    }
}

// ==========================================
// AC-64: ANULAR MATRICULA
// ==========================================

void anularMatricula(EntornoPruebas& entorno) {

    cout << "\n=== ANULAR MATRICULA ===\n";

    string codigo =
        leerTexto("Codigo de matricula");

    const Matricula* matricula =
        entorno.matriculas.buscarMatricula(codigo);

    if (matricula == nullptr) {

        cout << "\nMatricula no encontrada.\n";
        return;
    }

    mostrarMatricula(*matricula);

    string codigoCurso =
        matricula->obtenerDatos().codigoCurso;

    string motivo =
        leerTexto("Motivo de anulacion");

    string responsable =
        leerTexto("Responsable");

    if (!confirmarOperacion()) {

        cout << "\nAnulacion cancelada.\n";
        return;
    }

    if (
        entorno.matriculas.anularMatricula(
            codigo,
            motivo,
            responsable
        )
    ) {

        cout << "\nMATRICULA ANULADA CORRECTAMENTE.\n";

        const Matricula* actualizada =
            entorno.matriculas.buscarMatricula(codigo);

        if (actualizada != nullptr) {
            mostrarMatricula(*actualizada);
        }

        mostrarCupos(
            entorno.cursos,
            codigoCurso
        );

    } else {

        cout << "\nNO SE PUDO ANULAR LA MATRICULA.\n";

        cout << "Motivo: "
             << entorno.matriculas.obtenerUltimoError()
             << "\n";
    }
}

// ==========================================
// CONSULTAR HISTORIAL DE MATRICULA
// ==========================================

void consultarHistorial(
    const GestorMatriculas& gestor
) {

    cout << "\n=== CONSULTAR HISTORIAL ===\n";

    string codigo =
        leerTexto("Codigo de matricula");

    const Matricula* matricula =
        gestor.buscarMatricula(codigo);

    if (matricula == nullptr) {

        cout << "\nMatricula no encontrada.\n";
        return;
    }

    mostrarHistorial(*matricula);
}

// ==========================================
// PRUEBAS AUTOMATICAS
// ==========================================

void ejecutarPruebasAutomaticas() {

    cout << "\n========================================\n";
    cout << "     PRUEBAS AUTOMATICAS MATRICULAS\n";
    cout << "========================================\n";

    // Se utiliza un entorno nuevo para
    // no modificar las matriculas del menu.

    EntornoPruebas entorno;

    GestorMatriculas& gestor = entorno.matriculas;

    int correctas = 0;
    int total = 0;

    auto comprobar = [&](bool condicion,
                         const string& descripcion) {

        total++;

        if (condicion) {

            correctas++;
            cout << "[OK] " << descripcion << "\n";

        } else {

            cout << "[ERROR] " << descripcion << "\n";
        }
    };

    comprobar(
        entorno.datosInicialesCorrectos,
        "Crear periodos y cursos de prueba"
    );

    if (!entorno.datosInicialesCorrectos) {

        cout << "\nNo se pudieron preparar "
             << "los datos iniciales.\n";

        return;
    }

    // ======================================
    // AC-62: PRERREQUISITOS
    // ======================================

    comprobar(
        gestor.verificarPrerequisitos(
            ESTUDIANTE_1,
            "INF102"
        ),
        "Aceptar prerrequisito INF101 aprobado"
    );

    comprobar(
        !gestor.verificarPrerequisitos(
            ESTUDIANTE_2,
            "INF102"
        ),
        "Rechazar prerrequisito pendiente"
    );

    comprobar(
        gestor.verificarPrerequisitos(
            ESTUDIANTE_2,
            "MAT101"
        ),
        "Aceptar asignatura sin prerrequisitos"
    );

    comprobar(
        !gestor.verificarPrerequisitos(
            ESTUDIANTE_1,
            "ASIGNATURA-INEXISTENTE"
        ),
        "Rechazar asignatura desconocida"
    );

    // ======================================
    // AC-61: REGISTRO DE MATRICULAS
    // ======================================

    DatosMatricula m1 = crearDatosMatricula(
        entorno,
        "MAT-001",
        ESTUDIANTE_2,
        "CUR-INF101-A"
    );

    comprobar(
        gestor.registrarMatricula(m1),
        "Registrar primera matricula"
    );

    comprobar(
        gestor.buscarMatricula("MAT-001") != nullptr,
        "Buscar matricula registrada"
    );

    comprobar(
        !gestor.registrarMatricula(m1),
        "Rechazar codigo de matricula duplicado"
    );

    DatosMatricula duplicada = crearDatosMatricula(
        entorno,
        "MAT-002",
        ESTUDIANTE_2,
        "CUR-INF101-B"
    );

    comprobar(
        !gestor.registrarMatricula(duplicada),
        "Rechazar misma asignatura en dos secciones"
    );

    DatosMatricula conflicto = crearDatosMatricula(
        entorno,
        "MAT-003",
        ESTUDIANTE_2,
        "CUR-MAT101-A"
    );

    comprobar(
        !gestor.registrarMatricula(conflicto),
        "Rechazar conflicto de horarios"
    );

    DatosMatricula m4 = crearDatosMatricula(
        entorno,
        "MAT-004",
        ESTUDIANTE_2,
        "CUR-MAT101-B"
    );

    comprobar(
        gestor.registrarMatricula(m4),
        "Permitir otra asignatura sin conflicto"
    );

    DatosMatricula sinPrerequisito = crearDatosMatricula(
        entorno,
        "MAT-005",
        ESTUDIANTE_2,
        "CUR-INF102-A"
    );

    comprobar(
        !gestor.registrarMatricula(sinPrerequisito),
        "Rechazar matricula sin prerrequisito"
    );

    DatosMatricula m6 = crearDatosMatricula(
        entorno,
        "MAT-006",
        ESTUDIANTE_1,
        "CUR-INF102-A"
    );

    comprobar(
        gestor.registrarMatricula(m6),
        "Registrar estudiante con prerrequisito"
    );

    DatosMatricula m7 = crearDatosMatricula(
        entorno,
        "MAT-007",
        ESTUDIANTE_3,
        "CUR-MAT102-A"
    );

    comprobar(
        gestor.registrarMatricula(m7),
        "Registrar estudiante en MAT102"
    );

    DatosMatricula desconocido = crearDatosMatricula(
        entorno,
        "MAT-008",
        "99.999.999-9",
        "CUR-INF101-A"
    );

    comprobar(
        !gestor.registrarMatricula(desconocido),
        "Rechazar estudiante inexistente"
    );

    DatosMatricula inactivo = crearDatosMatricula(
        entorno,
        "MAT-009",
        ESTUDIANTE_INACTIVO,
        "CUR-INF101-A"
    );

    comprobar(
        !gestor.registrarMatricula(inactivo),
        "Rechazar estudiante inactivo"
    );

    DatosMatricula incorrecta = crearDatosMatricula(
        entorno,
        "MAT-010",
        ESTUDIANTE_1,
        "CUR-INF101-A"
    );

    incorrecta.codigoAsignatura = "MAT101";

    comprobar(
        !gestor.registrarMatricula(incorrecta),
        "Rechazar asignatura que no coincide con curso"
    );

    DatosMatricula m11 = crearDatosMatricula(
        entorno,
        "MAT-011",
        ESTUDIANTE_1,
        "CUR-INF101-A"
    );

    comprobar(
        gestor.registrarMatricula(m11),
        "Ocupar ultimo cupo de seccion A"
    );

    const Curso* cursoA =
        entorno.cursos.buscarCurso("CUR-INF101-A");

    comprobar(
        cursoA != nullptr &&
        cursoA->obtenerCuposDisponibles() == 0,
        "Verificar seccion sin cupos"
    );

    DatosMatricula sobrecupo = crearDatosMatricula(
        entorno,
        "MAT-012",
        ESTUDIANTE_3,
        "CUR-INF101-A"
    );

    comprobar(
        !gestor.registrarMatricula(sobrecupo),
        "Impedir matricula con cupos completos"
    );

    // ======================================
    // AC-63: CONSULTAS
    // ======================================

    comprobar(
        gestor.consultarInscritos(
            "CUR-INF101-A"
        ).size() == 2,
        "Consultar dos inscritos en seccion A"
    );

    comprobar(
        gestor.consultarPorEstudiante(
            ESTUDIANTE_2
        ).size() == 2,
        "Consultar matriculas por estudiante"
    );

    comprobar(
        gestor.listarMatriculas().size() == 5,
        "Listar matriculas registradas"
    );

    // ======================================
    // AC-64: CAMBIAR SECCION
    // ======================================

    comprobar(
        gestor.cambiarSeccion(
            "MAT-001",
            "CUR-INF101-B",
            "Cambio solicitado",
            "ADMIN001"
        ),
        "Cambiar de seccion A a B"
    );

    const Matricula* modificada =
        gestor.buscarMatricula("MAT-001");

    comprobar(
        modificada != nullptr &&
        modificada->obtenerDatos().codigoCurso ==
            "CUR-INF101-B",
        "Actualizar codigo de curso"
    );

    comprobar(
        modificada != nullptr &&
        modificada->obtenerHistorial().size() == 1,
        "Registrar historial del cambio"
    );

    const Curso* cursoB =
        entorno.cursos.buscarCurso("CUR-INF101-B");

    comprobar(
        cursoA != nullptr &&
        cursoB != nullptr &&
        cursoA->obtenerCantidadInscritos() == 1 &&
        cursoB->obtenerCantidadInscritos() == 1,
        "Actualizar cupos de ambas secciones"
    );

    comprobar(
        !gestor.cambiarSeccion(
            "MAT-001",
            "CUR-INF101-B",
            "Cambio repetido",
            "ADMIN001"
        ),
        "Rechazar cambio a la misma seccion"
    );

    comprobar(
        !gestor.cambiarSeccion(
            "MAT-001",
            "CUR-MAT101-B",
            "Cambio de asignatura",
            "ADMIN001"
        ),
        "Rechazar cambio a otra asignatura"
    );

    DatosMatricula m13 = crearDatosMatricula(
        entorno,
        "MAT-013",
        ESTUDIANTE_3,
        "CUR-INF101-A"
    );

    comprobar(
        gestor.registrarMatricula(m13),
        "Ocupar cupo liberado en seccion A"
    );

    comprobar(
        !gestor.cambiarSeccion(
            "MAT-001",
            "CUR-INF101-A",
            "Regresar a seccion A",
            "ADMIN001"
        ),
        "Rechazar cambio hacia seccion llena"
    );

    modificada = gestor.buscarMatricula("MAT-001");

    comprobar(
        modificada != nullptr &&
        modificada->obtenerDatos().codigoCurso ==
            "CUR-INF101-B" &&
        modificada->obtenerHistorial().size() == 1,
        "Conservar matricula tras cambio rechazado"
    );

    // ======================================
    // AC-64: ANULAR MATRICULA
    // ======================================

    comprobar(
        !gestor.anularMatricula(
            "MAT-001",
            "",
            "ADMIN001"
        ),
        "Rechazar anulacion sin motivo"
    );

    comprobar(
        gestor.anularMatricula(
            "MAT-001",
            "Solicitud del estudiante",
            "ADMIN001"
        ),
        "Anular matricula vigente"
    );

    const Matricula* anulada =
        gestor.buscarMatricula("MAT-001");

    comprobar(
        anulada != nullptr &&
        anulada->obtenerEstado() ==
            EstadoMatricula::Anulada,
        "Actualizar estado a Anulada"
    );

    comprobar(
        anulada != nullptr &&
        anulada->obtenerHistorial().size() == 2,
        "Conservar cambio y anulacion en historial"
    );

    comprobar(
        cursoB != nullptr &&
        cursoB->obtenerCuposDisponibles() == 2,
        "Liberar cupo al anular matricula"
    );

    comprobar(
        gestor.consultarInscritos(
            "CUR-INF101-B"
        ).empty(),
        "Excluir anulados de inscritos vigentes"
    );

    comprobar(
        gestor.consultarPorEstudiante(
            ESTUDIANTE_2,
            true
        ).size() == 2,
        "Conservar anulados en historial academico"
    );

    comprobar(
        gestor.consultarPorEstudiante(
            ESTUDIANTE_2,
            false
        ).size() == 1,
        "Filtrar solamente matriculas vigentes"
    );

    comprobar(
        !gestor.anularMatricula(
            "MAT-001",
            "Anulacion repetida",
            "ADMIN001"
        ),
        "Rechazar segunda anulacion"
    );

    comprobar(
        !gestor.cambiarSeccion(
            "MAT-001",
            "CUR-INF101-A",
            "Modificar anulada",
            "ADMIN001"
        ),
        "Impedir modificar matricula anulada"
    );

    comprobar(
        !gestor.registrarMatricula(m1),
        "Impedir reutilizar codigo de matricula anulada"
    );

    // ======================================
    // VALIDAR CIERRE DE CURSOS Y PERIODOS
    // ======================================

    comprobar(
        entorno.cursos.cerrarCurso("CUR-INF101-B"),
        "Cerrar curso de destino"
    );

    DatosMatricula cursoCerrado =
        crearDatosMatricula(
            entorno,
            "MAT-014",
            ESTUDIANTE_3,
            "CUR-INF101-B"
        );

    comprobar(
        !gestor.registrarMatricula(cursoCerrado),
        "Rechazar matricula en curso cerrado"
    );

    comprobar(
        entorno.periodos.cerrarPeriodo("PA-2026-2"),
        "Cerrar periodo academico"
    );

    DatosMatricula periodoCerrado =
        crearDatosMatricula(
            entorno,
            "MAT-015",
            ESTUDIANTE_1,
            "CUR-MAT101-B"
        );

    comprobar(
        !gestor.registrarMatricula(periodoCerrado),
        "Rechazar matricula en periodo cerrado"
    );

    comprobar(
        !gestor.anularMatricula(
            "MAT-004",
            "Fuera del periodo",
            "ADMIN001"
        ),
        "Impedir anulacion fuera de periodo habilitado"
    );

    // ======================================
    // RESULTADOS
    // ======================================

    cout << "\n========================================\n";

    cout << "PRUEBAS CORRECTAS: "
         << correctas
         << " / "
         << total
         << "\n";

    if (correctas == total) {

        cout << "RESULTADO: TODAS LAS PRUEBAS PASARON\n";

    } else {

        cout << "RESULTADO: EXISTEN PRUEBAS FALLIDAS\n";
    }

    cout << "========================================\n";
}

// ==========================================
// MENU PRINCIPAL
// ==========================================

void mostrarMenu() {

    cout << "\n========================================\n";
    cout << "               ACADEMIA\n";
    cout << "        GESTION DE MATRICULAS\n";
    cout << "========================================\n";

    cout << "1. Registrar matricula\n";
    cout << "2. Listar matriculas\n";
    cout << "3. Buscar matricula\n";
    cout << "4. Consultar estudiantes por curso\n";
    cout << "5. Consultar matriculas por estudiante\n";
    cout << "6. Verificar prerrequisitos\n";
    cout << "7. Cambiar seccion\n";
    cout << "8. Anular matricula\n";
    cout << "9. Consultar historial de matricula\n";
    cout << "10. Consultar cupos de un curso\n";
    cout << "11. Ver catalogos de prueba\n";
    cout << "12. Ejecutar pruebas automaticas\n";
    cout << "0. Salir\n";

    cout << "========================================\n";
}

// ==========================================
// PROGRAMA PRINCIPAL
// ==========================================

int main() {

    EntornoPruebas entorno;

    if (!entorno.datosInicialesCorrectos) {

        cout << "\nERROR: No se pudieron inicializar "
             << "los periodos o cursos de prueba.\n";

        return 1;
    }

    int opcion;

    do {

        mostrarMenu();

        opcion = leerNumero(
            "Seleccione una opcion"
        );

        switch (opcion) {

            case 1:
                registrarMatricula(entorno);
                break;

            case 2:
                listarMatriculas(
                    entorno.matriculas
                );
                break;

            case 3:
                buscarMatricula(
                    entorno.matriculas
                );
                break;

            case 4:
                consultarInscritos(entorno);
                break;

            case 5:
                consultarPorEstudiante(
                    entorno.matriculas
                );
                break;

            case 6:
                consultarPrerequisitos(entorno);
                break;

            case 7:
                cambiarSeccion(entorno);
                break;

            case 8:
                anularMatricula(entorno);
                break;

            case 9:
                consultarHistorial(
                    entorno.matriculas
                );
                break;

            case 10: {

                string codigo =
                    leerTexto("Codigo del curso");

                mostrarCupos(
                    entorno.cursos,
                    codigo
                );

                break;
            }

            case 11:
                mostrarCatalogos(entorno);
                break;

            case 12:
                ejecutarPruebasAutomaticas();
                break;

            case 0:
                cout << "\nSaliendo de AcademIA...\n";
                break;

            default:
                cout << "\nOpcion no valida.\n";
                break;
        }

    } while (opcion != 0);

    return 0;
}
