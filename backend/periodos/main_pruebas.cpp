
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

#include "GestorPeriodos.hpp"

using namespace std;

// ==========================================
// FUNCIONES AUXILIARES
// ==========================================

string leerTexto(
    const string& mensaje,
    const string& valorActual = ""
) {
    string texto;

    cout << mensaje;

    if (!valorActual.empty()) {
        cout << " [" << valorActual << "]";
    }

    cout << ": ";
    getline(cin, texto);

    if (texto.empty()) {
        return valorActual;
    }

    return texto;
}

int leerNumero(
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
        getline(cin, entrada);

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
    string respuesta = leerTexto(
        "Confirmar operacion (S/N)"
    );

    return respuesta == "S" || respuesta == "s";
}

// ==========================================
// MOSTRAR INFORMACION DEL PERIODO
// ==========================================

void mostrarPeriodo(const PeriodoAcademico& periodo) {

    const DatosPeriodoAcademico& datos =
        periodo.obtenerDatos();

    cout << "\n----------------------------------------\n";
    cout << "Codigo: " << datos.codigo << "\n";
    cout << "Nombre: " << datos.nombre << "\n";
    cout << "Anio academico: " << datos.anio << "\n";
    cout << "Semestre: " << datos.semestre << "\n";
    cout << "Fecha de inicio: " << datos.fechaInicio << "\n";
    cout << "Fecha de termino: " << datos.fechaFin << "\n";
    cout << "Estado: "
         << periodo.obtenerEstadoTexto() << "\n";
    cout << "----------------------------------------\n";
}

// ==========================================
// AC-58: CREAR PERIODO ACADEMICO
// ==========================================

void crearPeriodo(GestorPeriodos& gestor) {

    cout << "\n=== CREAR PERIODO ACADEMICO ===\n";

    DatosPeriodoAcademico datos{};

    datos.codigo = leerTexto("Codigo del periodo");
    datos.nombre = leerTexto("Nombre del periodo");
    datos.anio = leerNumero("Anio academico");
    datos.semestre = leerNumero("Semestre (1 o 2)");
    datos.fechaInicio = leerTexto(
        "Fecha de inicio (AAAA-MM-DD)"
    );
    datos.fechaFin = leerTexto(
        "Fecha de termino (AAAA-MM-DD)"
    );

    if (!confirmarOperacion()) {
        cout << "\nRegistro cancelado.\n";
        return;
    }

    if (gestor.crearPeriodo(datos)) {

        cout << "\nPeriodo academico registrado correctamente.\n";

        const PeriodoAcademico* periodo =
            gestor.buscarPeriodo(datos.codigo);

        if (periodo != nullptr) {
            mostrarPeriodo(*periodo);
        }

    } else {

        cout << "\nNo se pudo registrar el periodo.\n";
        cout << "Verifique los siguientes datos:\n";
        cout << "- Campos obligatorios.\n";
        cout << "- Codigo duplicado.\n";
        cout << "- Anio y semestre duplicados.\n";
        cout << "- Fechas invalidas.\n";
        cout << "- Fecha de inicio anterior al termino.\n";
    }
}

// ==========================================
// LISTAR PERIODOS ACADEMICOS
// ==========================================

void listarPeriodos(const GestorPeriodos& gestor) {

    cout << "\n=== LISTADO DE PERIODOS ===\n";

    vector<PeriodoAcademico> periodos =
        gestor.listarPeriodos();

    if (periodos.empty()) {
        cout << "\nNo existen periodos registrados.\n";
        return;
    }

    cout << "\nTotal de periodos: "
         << periodos.size() << "\n";

    for (const PeriodoAcademico& periodo : periodos) {
        mostrarPeriodo(periodo);
    }
}

// ==========================================
// BUSCAR PERIODO ACADEMICO
// ==========================================

void buscarPeriodo(const GestorPeriodos& gestor) {

    cout << "\n=== BUSCAR PERIODO ACADEMICO ===\n";

    string codigo = leerTexto(
        "Ingrese el codigo del periodo"
    );

    const PeriodoAcademico* periodo =
        gestor.buscarPeriodo(codigo);

    if (periodo == nullptr) {

        cout << "\nNo se encontro el periodo academico.\n";

    } else {

        cout << "\nPeriodo encontrado correctamente.\n";
        mostrarPeriodo(*periodo);
    }
}

// ==========================================
// MODIFICAR PERIODO ACADEMICO
// ==========================================

void modificarPeriodo(GestorPeriodos& gestor) {

    cout << "\n=== MODIFICAR PERIODO ACADEMICO ===\n";

    string codigo = leerTexto(
        "Codigo del periodo a modificar"
    );

    const PeriodoAcademico* periodo =
        gestor.buscarPeriodo(codigo);

    if (periodo == nullptr) {
        cout << "\nPeriodo academico no encontrado.\n";
        return;
    }

    if (periodo->obtenerEstado() == EstadoPeriodo::Cerrado) {
        cout << "\nNo se puede modificar un periodo cerrado.\n";
        return;
    }

    cout << "\nDatos actuales del periodo:\n";
    mostrarPeriodo(*periodo);

    DatosPeriodoAcademico datos =
        periodo->obtenerDatos();

    cout << "\nIngrese los nuevos datos.\n";
    cout << "Presione ENTER para conservar el valor actual.\n";

    // El codigo identificador no puede modificarse.

    datos.nombre = leerTexto(
        "Nombre del periodo",
        datos.nombre
    );

    datos.anio = leerNumero(
        "Anio academico",
        datos.anio
    );

    datos.semestre = leerNumero(
        "Semestre",
        datos.semestre
    );

    datos.fechaInicio = leerTexto(
        "Fecha de inicio",
        datos.fechaInicio
    );

    datos.fechaFin = leerTexto(
        "Fecha de termino",
        datos.fechaFin
    );

    if (!confirmarOperacion()) {
        cout << "\nModificacion cancelada.\n";
        return;
    }

    if (gestor.modificarPeriodo(codigo, datos)) {

        cout << "\nPeriodo actualizado correctamente.\n";

        const PeriodoAcademico* actualizado =
            gestor.buscarPeriodo(codigo);

        if (actualizado != nullptr) {
            mostrarPeriodo(*actualizado);
        }

    } else {

        cout << "\nNo se pudo actualizar el periodo.\n";
        cout << "Verifique las fechas, el semestre y los duplicados.\n";
    }
}

// ==========================================
// CERRAR PERIODO ACADEMICO
// ==========================================

void cerrarPeriodo(GestorPeriodos& gestor) {

    cout << "\n=== CERRAR PERIODO ACADEMICO ===\n";

    string codigo = leerTexto(
        "Codigo del periodo a cerrar"
    );

    const PeriodoAcademico* periodo =
        gestor.buscarPeriodo(codigo);

    if (periodo == nullptr) {
        cout << "\nPeriodo no encontrado.\n";
        return;
    }

    cout << "\nPeriodo seleccionado:\n";
    mostrarPeriodo(*periodo);

    if (periodo->obtenerEstado() == EstadoPeriodo::Cerrado) {
        cout << "\nEste periodo ya se encuentra cerrado.\n";
        return;
    }

    cout << "\nADVERTENCIA:\n";
    cout << "Cerrar el periodo impedira nuevas matriculas.\n";

    if (!confirmarOperacion()) {
        cout << "\nOperacion cancelada.\n";
        return;
    }

    if (gestor.cerrarPeriodo(codigo)) {

        cout << "\nPeriodo cerrado correctamente.\n";

    } else {

        cout << "\nNo fue posible cerrar el periodo.\n";
    }
}

// ==========================================
// VERIFICAR PERMISO DE MATRICULA
// ==========================================

void verificarMatricula(const GestorPeriodos& gestor) {

    cout << "\n=== VERIFICAR DISPONIBILIDAD DE MATRICULA ===\n";

    string codigo = leerTexto(
        "Codigo del periodo"
    );

    const PeriodoAcademico* periodo =
        gestor.buscarPeriodo(codigo);

    if (periodo == nullptr) {
        cout << "\nEl periodo no existe.\n";
        return;
    }

    mostrarPeriodo(*periodo);

    if (gestor.permiteMatricula(codigo)) {

        cout << "\nEl periodo permite matriculas.\n";

    } else {

        cout << "\nEl periodo NO permite nuevas matriculas.\n";
    }
}

// ==========================================
// MENU PRINCIPAL
// ==========================================

void mostrarMenu() {

    cout << "\n========================================\n";
    cout << "               ACADEMIA\n";
    cout << "     GESTION DE PERIODOS ACADEMICOS\n";
    cout << "========================================\n";

    cout << "1. Crear periodo academico\n";
    cout << "2. Listar periodos academicos\n";
    cout << "3. Buscar periodo academico\n";
    cout << "4. Modificar periodo academico\n";
    cout << "5. Cerrar periodo academico\n";
    cout << "6. Verificar disponibilidad de matricula\n";
    cout << "0. Salir\n";

    cout << "========================================\n";
}

// ==========================================
// PROGRAMA PRINCIPAL
// ==========================================

int main() {

    GestorPeriodos gestor;

    int opcion;

    do {

        mostrarMenu();

        opcion = leerNumero(
            "Seleccione una opcion"
        );

        switch (opcion) {

            case 1:
                crearPeriodo(gestor);
                break;

            case 2:
                listarPeriodos(gestor);
                break;

            case 3:
                buscarPeriodo(gestor);
                break;

            case 4:
                modificarPeriodo(gestor);
                break;

            case 5:
                cerrarPeriodo(gestor);
                break;

            case 6:
                verificarMatricula(gestor);
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
