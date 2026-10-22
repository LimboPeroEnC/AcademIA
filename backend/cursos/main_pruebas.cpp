
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <sstream>
#include <iomanip>
#include <functional>
#include <cstdlib>

#include "Curso.hpp"
#include "GestorCursos.hpp"
#include "../periodos/GestorPeriodos.hpp"

using namespace std;

// ==========================================
// ENTORNO DE PRUEBAS DE ACADEMIA
// ==========================================

struct EntornoPruebas {

    GestorPeriodos periodos;

    // Catalogos temporales.
    // Mas adelante se conectaran con los
    // modulos reales de asignaturas y docentes.

    set<string> asignaturas;
    set<string> docentes;

    GestorCursos gestor;

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
              "DOC003",
              "DOC004"
          }),
          gestor(
              [this](const string& codigo) {
                  return periodos.permiteMatricula(codigo);
              },

              [this](const string& codigo) {
                  return asignaturas.count(codigo) > 0;
              },

              [this](const string& codigo) {
                  return docentes.count(codigo) > 0;
              }
          ) {

        // Periodos reales del modulo AC-58.
        // Solamente son datos iniciales de prueba.

        periodos.crearPeriodo({
            "PA-2026-2",
            "Segundo semestre 2026",
            2026,
            2,
            "2026-08-03",
            "2026-12-18"
        });

        periodos.crearPeriodo({
            "PA-2027-1",
            "Primer semestre 2027",
            2027,
            1,
            "2027-03-01",
            "2027-07-16"
        });
    }
};

// ==========================================
// FUNCIONES DE ENTRADA
// ==========================================

string leerTexto(
    const string& mensaje,
    const string& valorActual = ""
) {

    string entrada;

    cout << mensaje;

    if (!valorActual.empty()) {
        cout << " [" << valorActual << "]";
    }

    cout << ": ";

    if (!getline(cin, entrada)) {
        cout << "\nEntrada finalizada.\n";
        exit(0);
    }

    if (entrada.empty()) {
        return valorActual;
    }

    return entrada;
}

int leerEntero(
    const string& mensaje,
    int valorActual = -1
) {

    while (true) {

        string entrada;

        cout << mensaje;

        if (valorActual != -1) {
            cout << " [" << valorActual << "]";
        }

        cout << ": ";

        if (!getline(cin, entrada)) {
            cout << "\nEntrada finalizada.\n";
            exit(0);
        }

        if (entrada.empty() && valorActual != -1) {
            return valorActual;
        }

        istringstream flujo(entrada);

        int numero;
        char extra;

        if (flujo >> numero && !(flujo >> extra)) {
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
// FUNCIONES PARA HORARIOS
// ==========================================

string nombreDia(int dia) {

    switch (dia) {

        case 1: return "Lunes";
        case 2: return "Martes";
        case 3: return "Miercoles";
        case 4: return "Jueves";
        case 5: return "Viernes";
        case 6: return "Sabado";
        case 7: return "Domingo";

        default: return "Desconocido";
    }
}

string formatearHora(int minutos) {

    int hora = minutos / 60;
    int minuto = minutos % 60;

    ostringstream salida;

    salida << setfill('0')
           << setw(2) << hora
           << ":"
           << setw(2) << minuto;

    return salida.str();
}

bool convertirHora(
    const string& texto,
    int& minutos
) {

    if (
        texto.size() != 5 ||
        texto[2] != ':'
    ) {
        return false;
    }

    for (int i : {0, 1, 3, 4}) {

        if (
            texto[i] < '0' ||
            texto[i] > '9'
        ) {
            return false;
        }
    }

    int hora = stoi(texto.substr(0, 2));
    int minuto = stoi(texto.substr(3, 2));

    if (
        hora < 0 ||
        hora > 24 ||
        minuto < 0 ||
        minuto > 59
    ) {
        return false;
    }

    if (hora == 24 && minuto != 0) {
        return false;
    }

    minutos = hora * 60 + minuto;

    return true;
}

int leerHora(const string& mensaje) {

    while (true) {

        string entrada = leerTexto(
            mensaje + " (HH:MM)"
        );

        int minutos;

        if (convertirHora(entrada, minutos)) {
            return minutos;
        }

        cout << "Horario invalido. Ejemplo: 08:30\n";
    }
}

vector<HorarioCurso> ingresarHorarios() {

    vector<HorarioCurso> horarios;

    int cantidad;

    do {

        cantidad = leerEntero(
            "Cantidad de bloques de horario"
        );

        if (cantidad <= 0) {
            cout << "Debe ingresar al menos un bloque.\n";
        }

    } while (cantidad <= 0);

    for (int i = 0; i < cantidad; i++) {

        cout << "\n--- Bloque "
             << i + 1
             << " ---\n";

        cout << "1. Lunes\n";
        cout << "2. Martes\n";
        cout << "3. Miercoles\n";
        cout << "4. Jueves\n";
        cout << "5. Viernes\n";
        cout << "6. Sabado\n";
        cout << "7. Domingo\n";

        int dia = leerEntero("Dia");

        int inicio = leerHora("Hora inicio");
        int fin = leerHora("Hora termino");

        horarios.push_back({
            dia,
            inicio,
            fin
        });
    }

    return horarios;
}

// ==========================================
// MOSTRAR DATOS DEL CURSO
// ==========================================

void mostrarCurso(const Curso& curso) {

    const DatosCurso& datos =
        curso.obtenerDatos();

    cout << "\n----------------------------------------\n";

    cout << "Codigo: "
         << datos.codigo << "\n";

    cout << "Asignatura: "
         << datos.codigoAsignatura << "\n";

    cout << "Periodo academico: "
         << datos.codigoPeriodo << "\n";

    cout << "Seccion: "
         << datos.seccion << "\n";

    cout << "Modalidad: "
         << datos.modalidad << "\n";

    cout << "Estado: "
         << curso.obtenerEstadoTexto() << "\n";

    cout << "Docente: ";

    if (curso.obtenerDocente().empty()) {
        cout << "Sin asignar\n";
    } else {
        cout << curso.obtenerDocente() << "\n";
    }

    cout << "Capacidad maxima: "
         << datos.capacidadMaxima << "\n";

    cout << "Estudiantes inscritos: "
         << curso.obtenerCantidadInscritos() << "\n";

    cout << "Cupos disponibles: "
         << curso.obtenerCuposDisponibles() << "\n";

    cout << "Horarios:\n";

    for (const HorarioCurso& horario : datos.horarios) {

        cout << "  "
             << nombreDia(horario.dia)
             << " "
             << formatearHora(horario.horaInicio)
             << " - "
             << formatearHora(horario.horaFin)
             << "\n";
    }

    cout << "----------------------------------------\n";
}

// ==========================================
// MOSTRAR CATALOGOS DISPONIBLES
// ==========================================

void mostrarCatalogos(const EntornoPruebas& entorno) {

    cout << "\n=== CATALOGOS DE PRUEBA ===\n";

    cout << "\nPeriodos academicos:\n";

    for (
        const PeriodoAcademico& periodo :
        entorno.periodos.listarPeriodos()
    ) {

        cout << "- "
             << periodo.obtenerDatos().codigo
             << " | "
             << periodo.obtenerDatos().nombre
             << " | "
             << periodo.obtenerEstadoTexto()
             << "\n";
    }

    cout << "\nAsignaturas simuladas:\n";

    for (const string& codigo : entorno.asignaturas) {
        cout << "- " << codigo << "\n";
    }

    cout << "\nDocentes simulados:\n";

    for (const string& codigo : entorno.docentes) {
        cout << "- " << codigo << "\n";
    }
}

// ==========================================
// AC-59: REGISTRAR CURSO
// ==========================================

void registrarCurso(EntornoPruebas& entorno) {

    cout << "\n=== REGISTRAR CURSO ===\n";

    mostrarCatalogos(entorno);

    DatosCurso datos{};

    datos.codigo = leerTexto("Codigo del curso");

    datos.codigoAsignatura =
        leerTexto("Codigo asignatura");

    datos.codigoPeriodo =
        leerTexto("Codigo periodo academico");

    datos.seccion = leerTexto("Seccion");

    datos.capacidadMaxima =
        leerEntero("Capacidad maxima");

    datos.modalidad =
        leerTexto("Modalidad");

    cout << "\nIngrese los horarios del curso.\n";

    datos.horarios = ingresarHorarios();

    if (!confirmarOperacion()) {

        cout << "\nRegistro cancelado.\n";
        return;
    }

    if (entorno.gestor.registrarCurso(datos)) {

        cout << "\nCurso registrado correctamente.\n";

        const Curso* curso =
            entorno.gestor.buscarCurso(datos.codigo);

        if (curso != nullptr) {
            mostrarCurso(*curso);
        }

    } else {

        cout << "\nNo se pudo registrar el curso.\n";

        cout << "Posibles causas:\n";
        cout << "- Codigo duplicado.\n";
        cout << "- Seccion duplicada en el periodo.\n";
        cout << "- Asignatura no registrada.\n";
        cout << "- Periodo inexistente o cerrado.\n";
        cout << "- Capacidad no valida.\n";
        cout << "- Horarios incorrectos o superpuestos.\n";
    }
}

// ==========================================
// LISTAR CURSOS
// ==========================================

void listarCursos(const GestorCursos& gestor) {

    cout << "\n=== LISTADO DE CURSOS ===\n";

    vector<Curso> cursos = gestor.listarCursos();

    if (cursos.empty()) {

        cout << "No existen cursos registrados.\n";
        return;
    }

    cout << "Total de cursos: "
         << cursos.size() << "\n";

    for (const Curso& curso : cursos) {
        mostrarCurso(curso);
    }
}

// ==========================================
// BUSCAR CURSO
// ==========================================

void buscarCurso(const GestorCursos& gestor) {

    cout << "\n=== BUSCAR CURSO ===\n";

    string codigo = leerTexto("Codigo del curso");

    const Curso* curso =
        gestor.buscarCurso(codigo);

    if (curso == nullptr) {

        cout << "\nCurso no encontrado.\n";
        return;
    }

    cout << "\nCurso encontrado.\n";

    mostrarCurso(*curso);
}

// ==========================================
// MODIFICAR CURSO
// ==========================================

void modificarCurso(EntornoPruebas& entorno) {

    cout << "\n=== MODIFICAR CURSO ===\n";

    string codigo = leerTexto(
        "Codigo del curso a modificar"
    );

    const Curso* curso =
        entorno.gestor.buscarCurso(codigo);

    if (curso == nullptr) {

        cout << "\nCurso no encontrado.\n";
        return;
    }

    if (
        curso->obtenerEstado() ==
        EstadoCurso::Cerrado
    ) {

        cout << "\nNo se puede modificar un curso cerrado.\n";
        return;
    }

    mostrarCurso(*curso);

    DatosCurso datos = curso->obtenerDatos();

    cout << "\nPresione ENTER para mantener los datos.\n";

    cout << "El codigo, asignatura, periodo y seccion\n";
    cout << "no pueden modificarse desde esta opcion.\n";

    datos.capacidadMaxima = leerEntero(
        "Capacidad maxima",
        datos.capacidadMaxima
    );

    datos.modalidad = leerTexto(
        "Modalidad",
        datos.modalidad
    );

    string cambiar = leerTexto(
        "Desea modificar los horarios (S/N)"
    );

    if (cambiar == "S" || cambiar == "s") {
        datos.horarios = ingresarHorarios();
    }

    if (!confirmarOperacion()) {

        cout << "\nModificacion cancelada.\n";
        return;
    }

    if (
        entorno.gestor.modificarCurso(
            codigo,
            datos
        )
    ) {

        cout << "\nCurso modificado correctamente.\n";

        const Curso* actualizado =
            entorno.gestor.buscarCurso(codigo);

        if (actualizado != nullptr) {
            mostrarCurso(*actualizado);
        }

    } else {

        cout << "\nNo fue posible modificar el curso.\n";

        cout << "Verifique capacidad, horarios y conflictos docentes.\n";
    }
}

// ==========================================
// AC-60: ASIGNAR DOCENTE
// ==========================================

void asignarDocente(EntornoPruebas& entorno) {

    cout << "\n=== ASIGNAR DOCENTE ===\n";

    string codigo = leerTexto("Codigo del curso");

    const Curso* curso =
        entorno.gestor.buscarCurso(codigo);

    if (curso == nullptr) {

        cout << "\nCurso no encontrado.\n";
        return;
    }

    mostrarCurso(*curso);

    cout << "\nDocentes disponibles en el catalogo:\n";

    for (const string& docente : entorno.docentes) {
        cout << "- " << docente << "\n";
    }

    string docente = leerTexto(
        "Identificador del docente"
    );

    if (!confirmarOperacion()) {

        cout << "\nAsignacion cancelada.\n";
        return;
    }

    if (
        entorno.gestor.asignarDocente(
            codigo,
            docente
        )
    ) {

        cout << "\nDocente asignado correctamente.\n";

    } else {

        cout << "\nNo se pudo asignar el docente.\n";

        cout << "Posibles causas:\n";
        cout << "- Docente inexistente.\n";
        cout << "- Curso cerrado o inexistente.\n";
        cout << "- El curso ya tiene un docente.\n";
        cout << "- Existe conflicto de horario.\n";
    }
}

// ==========================================
// QUITAR DOCENTE
// ==========================================

void quitarDocente(EntornoPruebas& entorno) {

    cout << "\n=== QUITAR DOCENTE ===\n";

    string codigo = leerTexto("Codigo del curso");

    const Curso* curso =
        entorno.gestor.buscarCurso(codigo);

    if (curso == nullptr) {

        cout << "\nCurso no encontrado.\n";
        return;
    }

    mostrarCurso(*curso);

    if (!confirmarOperacion()) {
        cout << "\nOperacion cancelada.\n";
        return;
    }

    if (entorno.gestor.quitarDocente(codigo)) {

        cout << "\nDocente retirado correctamente.\n";

    } else {

        cout << "\nNo se pudo quitar el docente.\n";
    }
}

// ==========================================
// AC-65: CONSULTAR OFERTA ACADEMICA
// ==========================================

void consultarOferta(const EntornoPruebas& entorno) {

    cout << "\n=== CONSULTAR OFERTA ACADEMICA ===\n";

    cout << "Presione ENTER para omitir un filtro.\n";

    string periodo = leerTexto(
        "Codigo del periodo"
    );

    string asignatura = leerTexto(
        "Codigo de asignatura"
    );

    string seccion = leerTexto(
        "Seccion"
    );

    string respuesta = leerTexto(
        "Solo mostrar cursos con cupos (S/N)"
    );

    bool soloConCupos =
        respuesta == "S" || respuesta == "s";

    vector<Curso> cursos =
        entorno.gestor.consultarOferta(
            periodo,
            asignatura,
            seccion,
            soloConCupos
        );

    if (cursos.empty()) {

        cout << "\nNo se encontraron cursos.\n";
        return;
    }

    cout << "\nCursos encontrados: "
         << cursos.size() << "\n";

    for (const Curso& curso : cursos) {
        mostrarCurso(curso);
    }
}

// ==========================================
// CONSULTAR CUPOS
// ==========================================

void consultarCupos(const GestorCursos& gestor) {

    cout << "\n=== CONSULTAR CUPOS ===\n";

    string codigo = leerTexto("Codigo del curso");

    const Curso* curso =
        gestor.buscarCurso(codigo);

    if (curso == nullptr) {

        cout << "\nCurso no encontrado.\n";
        return;
    }

    cout << "\nCurso: "
         << codigo << "\n";

    cout << "Capacidad maxima: "
         << curso->obtenerDatos().capacidadMaxima
         << "\n";

    cout << "Inscritos simulados: "
         << curso->obtenerCantidadInscritos()
         << "\n";

    cout << "Cupos disponibles: "
         << curso->obtenerCuposDisponibles()
         << "\n";
}

// ==========================================
// SIMULAR OCUPACION Y LIBERACION DE CUPOS
// ==========================================

void simularCupos(EntornoPruebas& entorno) {

    cout << "\n=== SIMULACION DE CUPOS ===\n";

    cout << "Esta opcion NO registra matriculas reales.\n";

    string codigo = leerTexto("Codigo del curso");

    const Curso* curso =
        entorno.gestor.buscarCurso(codigo);

    if (curso == nullptr) {

        cout << "\nCurso no encontrado.\n";
        return;
    }

    cout << "\n1. Ocupar un cupo\n";
    cout << "2. Liberar un cupo\n";

    int opcion = leerEntero("Seleccione una opcion");

    bool resultado = false;

    if (opcion == 1) {

        resultado = entorno.gestor.ocuparCupo(codigo);

    } else if (opcion == 2) {

        resultado = entorno.gestor.liberarCupo(codigo);

    } else {

        cout << "\nOpcion no valida.\n";
        return;
    }

    if (resultado) {

        cout << "\nOperacion realizada correctamente.\n";

    } else {

        cout << "\nOperacion rechazada.\n";
        cout << "Revise el estado del curso y sus cupos.\n";
    }

    const Curso* actualizado =
        entorno.gestor.buscarCurso(codigo);

    if (actualizado != nullptr) {
        mostrarCurso(*actualizado);
    }
}

// ==========================================
// CERRAR CURSO
// ==========================================

void cerrarCurso(EntornoPruebas& entorno) {

    cout << "\n=== CERRAR CURSO ===\n";

    string codigo = leerTexto(
        "Codigo del curso a cerrar"
    );

    const Curso* curso =
        entorno.gestor.buscarCurso(codigo);

    if (curso == nullptr) {

        cout << "\nCurso no encontrado.\n";
        return;
    }

    mostrarCurso(*curso);

    cout << "\nADVERTENCIA:\n";
    cout << "El curso dejara de aceptar nuevos cupos.\n";

    if (!confirmarOperacion()) {

        cout << "\nCierre cancelado.\n";
        return;
    }

    if (entorno.gestor.cerrarCurso(codigo)) {

        cout << "\nCurso cerrado correctamente.\n";

    } else {

        cout << "\nNo fue posible cerrar el curso.\n";
    }
}

// ==========================================
// DATOS AUXILIARES PARA PRUEBAS AUTOMATICAS
// ==========================================

DatosCurso crearDatosPrueba(
    const string& codigo,
    const string& asignatura,
    const string& periodo,
    const string& seccion,
    int capacidad,
    int inicio,
    int fin
) {

    DatosCurso datos{};

    datos.codigo = codigo;
    datos.codigoAsignatura = asignatura;
    datos.codigoPeriodo = periodo;
    datos.seccion = seccion;
    datos.capacidadMaxima = capacidad;
    datos.modalidad = "Presencial";

    datos.horarios.push_back({
        1,
        inicio,
        fin
    });

    return datos;
}

// ==========================================
// PRUEBAS AUTOMATICAS
// ==========================================

void ejecutarPruebasAutomaticas() {

    cout << "\n========================================\n";
    cout << "       PRUEBAS AUTOMATICAS CURSOS\n";
    cout << "========================================\n";

    // Entorno independiente:
    // no modifica los cursos ingresados
    // en el menu principal.

    EntornoPruebas entorno;

    GestorCursos& gestor = entorno.gestor;

    int aprobadas = 0;
    int total = 0;

    auto comprobar = [&](bool condicion,
                         const string& descripcion) {

        total++;

        if (condicion) {

            aprobadas++;
            cout << "[OK] " << descripcion << "\n";

        } else {

            cout << "[ERROR] " << descripcion << "\n";
        }
    };

    DatosCurso cursoA = crearDatosPrueba(
        "CUR-A",
        "INF101",
        "PA-2026-2",
        "A",
        2,
        510,
        600
    );

    DatosCurso cursoB = crearDatosPrueba(
        "CUR-B",
        "INF102",
        "PA-2026-2",
        "A",
        3,
        540,
        660
    );

    DatosCurso cursoC = crearDatosPrueba(
        "CUR-C",
        "MAT101",
        "PA-2026-2",
        "A",
        4,
        600,
        720
    );

    // ======================================
    // AC-59: REGISTRO Y VALIDACIONES
    // ======================================

    comprobar(
        gestor.registrarCurso(cursoA),
        "Registrar curso valido"
    );

    comprobar(
        gestor.buscarCurso("CUR-A") != nullptr,
        "Buscar curso registrado"
    );

    comprobar(
        gestor.listarCursos().size() == 1,
        "Listar cursos registrados"
    );

    comprobar(
        !gestor.registrarCurso(cursoA),
        "Rechazar curso duplicado"
    );

    DatosCurso duplicado = cursoA;
    duplicado.codigo = "CUR-OTRO";

    comprobar(
        !gestor.registrarCurso(duplicado),
        "Rechazar asignatura-periodo-seccion duplicados"
    );

    DatosCurso sinPeriodo = cursoA;
    sinPeriodo.codigo = "CUR-SIN-PERIODO";
    sinPeriodo.codigoPeriodo = "PA-INEXISTENTE";

    comprobar(
        !gestor.registrarCurso(sinPeriodo),
        "Rechazar periodo inexistente"
    );

    DatosCurso sinAsignatura = cursoA;
    sinAsignatura.codigo = "CUR-SIN-ASIGNATURA";
    sinAsignatura.codigoAsignatura = "XXX999";

    comprobar(
        !gestor.registrarCurso(sinAsignatura),
        "Rechazar asignatura inexistente"
    );

    DatosCurso sinCapacidad = cursoA;
    sinCapacidad.codigo = "CUR-SIN-CAPACIDAD";
    sinCapacidad.capacidadMaxima = 0;

    comprobar(
        !gestor.registrarCurso(sinCapacidad),
        "Rechazar capacidad igual a cero"
    );

    DatosCurso diaInvalido = cursoA;
    diaInvalido.codigo = "CUR-DIA-INVALIDO";
    diaInvalido.horarios[0].dia = 8;

    comprobar(
        !gestor.registrarCurso(diaInvalido),
        "Rechazar dia de horario invalido"
    );

    DatosCurso horaInvalida = cursoA;
    horaInvalida.codigo = "CUR-HORA-INVALIDA";
    horaInvalida.horarios[0].horaFin = 450;

    comprobar(
        !gestor.registrarCurso(horaInvalida),
        "Rechazar hora de termino anterior al inicio"
    );

    DatosCurso superpuesto = cursoA;
    superpuesto.codigo = "CUR-SUPERPUESTO";

    superpuesto.horarios.push_back({
        1,
        540,
        630
    });

    comprobar(
        !gestor.registrarCurso(superpuesto),
        "Rechazar bloques internos superpuestos"
    );

    comprobar(
        gestor.registrarCurso(cursoB),
        "Registrar segundo curso"
    );

    comprobar(
        gestor.registrarCurso(cursoC),
        "Registrar tercer curso"
    );

    // ======================================
    // AC-60: ASIGNACION DOCENTE
    // ======================================

    comprobar(
        gestor.asignarDocente("CUR-A", "DOC001"),
        "Asignar docente activo"
    );

    comprobar(
        !gestor.asignarDocente("CUR-A", "DOC002"),
        "Rechazar segunda asignacion al mismo curso"
    );

    comprobar(
        !gestor.asignarDocente("CUR-B", "DOC001"),
        "Detectar conflicto de horario docente"
    );

    comprobar(
        gestor.asignarDocente("CUR-B", "DOC002"),
        "Permitir docente diferente en horario coincidente"
    );

    comprobar(
        !gestor.asignarDocente("CUR-C", "DOC999"),
        "Rechazar docente inexistente"
    );

    comprobar(
        gestor.asignarDocente("CUR-C", "DOC001"),
        "Permitir horario contiguo sin superposicion"
    );

    DatosCurso nuevoHorario = cursoC;

    nuevoHorario.horarios[0].horaInicio = 570;

    comprobar(
        !gestor.modificarCurso("CUR-C", nuevoHorario),
        "Rechazar modificacion con conflicto docente"
    );

    comprobar(
        gestor.quitarDocente("CUR-C"),
        "Quitar docente asignado"
    );

    // ======================================
    // AC-65: OFERTA ACADEMICA Y CUPOS
    // ======================================

    comprobar(
        gestor.consultarOferta("PA-2026-2").size() == 3,
        "Consultar oferta por periodo"
    );

    comprobar(
        gestor.consultarOferta(
            "PA-2026-2",
            "INF101"
        ).size() == 1,
        "Consultar oferta por asignatura"
    );

    comprobar(
        gestor.ocuparCupo("CUR-A"),
        "Ocupar primer cupo"
    );

    comprobar(
        gestor.ocuparCupo("CUR-A"),
        "Ocupar segundo cupo"
    );

    comprobar(
        !gestor.ocuparCupo("CUR-A"),
        "Rechazar sobrecupo"
    );

    const Curso* lleno =
        gestor.buscarCurso("CUR-A");

    comprobar(
        lleno != nullptr &&
        lleno->obtenerCuposDisponibles() == 0,
        "Comprobar curso sin cupos"
    );

    comprobar(
        gestor.consultarOferta(
            "PA-2026-2",
            "INF101",
            "",
            true
        ).empty(),
        "Excluir curso lleno de oferta disponible"
    );

    DatosCurso menorCapacidad = cursoA;
    menorCapacidad.capacidadMaxima = 1;

    comprobar(
        !gestor.modificarCurso(
            "CUR-A",
            menorCapacidad
        ),
        "Rechazar capacidad inferior a inscritos"
    );

    comprobar(
        gestor.liberarCupo("CUR-A"),
        "Liberar cupo ocupado"
    );

    comprobar(
        gestor.cerrarCurso("CUR-A"),
        "Cerrar curso"
    );

    comprobar(
        !gestor.ocuparCupo("CUR-A"),
        "Impedir nuevos cupos en curso cerrado"
    );

    comprobar(
        !gestor.modificarCurso(
            "CUR-A",
            cursoA
        ),
        "Impedir modificacion de curso cerrado"
    );

    comprobar(
        !gestor.cerrarCurso("CUR-A"),
        "Rechazar segundo cierre"
    );

    // ======================================
    // RESULTADOS
    // ======================================

    cout << "\n========================================\n";

    cout << "PRUEBAS CORRECTAS: "
         << aprobadas
         << " / "
         << total
         << "\n";

    if (aprobadas == total) {

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
    cout << "    GESTION DE CURSOS Y SECCIONES\n";
    cout << "========================================\n";

    cout << "1. Registrar curso y seccion\n";
    cout << "2. Listar cursos\n";
    cout << "3. Buscar curso\n";
    cout << "4. Modificar curso\n";
    cout << "5. Asignar docente\n";
    cout << "6. Quitar docente\n";
    cout << "7. Consultar oferta academica\n";
    cout << "8. Consultar cupos\n";
    cout << "9. Simular ocupacion/liberacion de cupos\n";
    cout << "10. Cerrar curso\n";
    cout << "11. Ver catalogos para pruebas\n";
    cout << "12. Ejecutar pruebas automaticas\n";
    cout << "0. Salir\n";

    cout << "========================================\n";
}

// ==========================================
// PROGRAMA PRINCIPAL
// ==========================================

int main() {

    EntornoPruebas entorno;

    int opcion;

    do {

        mostrarMenu();

        opcion = leerEntero(
            "Seleccione una opcion"
        );

        switch (opcion) {

            case 1:
                registrarCurso(entorno);
                break;

            case 2:
                listarCursos(entorno.gestor);
                break;

            case 3:
                buscarCurso(entorno.gestor);
                break;

            case 4:
                modificarCurso(entorno);
                break;

            case 5:
                asignarDocente(entorno);
                break;

            case 6:
                quitarDocente(entorno);
                break;

            case 7:
                consultarOferta(entorno);
                break;

            case 8:
                consultarCupos(entorno.gestor);
                break;

            case 9:
                simularCupos(entorno);
                break;

            case 10:
                cerrarCurso(entorno);
                break;

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
