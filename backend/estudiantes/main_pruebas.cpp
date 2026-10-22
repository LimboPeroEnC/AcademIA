
#include <iostream>
#include <string>
#include <vector>
#include <optional>
#include <sstream>
#include <limits>

#include "GestorEstudiantes.hpp"
#include "ValidadorRut.hpp"

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

void mostrarEstudiante(const Estudiante& estudiante) {
    DatosEstudiante datos = estudiante.obtenerDatos();

    cout << "\n--------------------------------\n";
    cout << "RUT: " << datos.rut << "\n";
    cout << "Nombres: " << datos.nombres << "\n";
    cout << "Apellido paterno: "
         << datos.apellidoPaterno << "\n";
    cout << "Apellido materno: "
         << datos.apellidoMaterno << "\n";
    cout << "Fecha nacimiento: "
         << datos.fechaNacimiento << "\n";
    cout << "Direccion: " << datos.direccion << "\n";
    cout << "Comuna: " << datos.comuna << "\n";
    cout << "Correo: "
         << datos.correoInstitucional << "\n";
    cout << "Anio ingreso: "
         << datos.anioIngreso << "\n";
    cout << "Genero: " << datos.genero << "\n";
    cout << "Carrera: " << datos.carreraId << "\n";
    cout << "Codigo barra: "
         << estudiante.obtenerCodigoBarra() << "\n";
    cout << "Estado: "
         << estudiante.obtenerEstado() << "\n";
    cout << "--------------------------------\n";
}

optional<Estudiante> buscarPorRut(
    const GestorEstudiantes& gestor,
    const string& rut
) {
    auto resultados = gestor.buscarEstudiantes(rut);

    string rutBuscado = ValidadorRut::normalizar(rut);

    for (const Estudiante& estudiante : resultados) {
        if (
            ValidadorRut::normalizar(
                estudiante.obtenerRut()
            ) == rutBuscado
        ) {
            return estudiante;
        }
    }

    return nullopt;
}

// ==========================================
// AC-001: REGISTRAR ESTUDIANTE
// ==========================================

void registrarEstudiante(GestorEstudiantes& gestor) {
    cout << "\n=== REGISTRAR ESTUDIANTE ===\n";

    DatosEstudiante datos{};

    datos.rut = leerTexto("RUT");
    datos.nombres = leerTexto("Nombres");
    datos.apellidoPaterno =
        leerTexto("Apellido paterno");
    datos.apellidoMaterno =
        leerTexto("Apellido materno");
    datos.fechaNacimiento =
        leerTexto("Fecha nacimiento (AAAA-MM-DD)");
    datos.direccion = leerTexto("Direccion");
    datos.comuna = leerTexto("Comuna");
    datos.correoInstitucional =
        leerTexto("Correo institucional");
    datos.anioIngreso =
        leerNumero("Anio de ingreso");
    datos.genero = leerTexto("Genero");
    datos.carreraId = leerTexto("Codigo carrera");

    if (gestor.registrarEstudiante(datos)) {
        cout << "\nEstudiante registrado correctamente.\n";

        auto estudiante = buscarPorRut(gestor, datos.rut);

        if (estudiante) {
            mostrarEstudiante(*estudiante);
        }
    } else {
        cout << "\nNo se pudo registrar al estudiante.\n";
        cout << "Verifique los campos, RUT y correo.\n";
    }
}

// ==========================================
// AC-002: BUSCAR ESTUDIANTE
// ==========================================

void buscarEstudiante(const GestorEstudiantes& gestor) {
    cout << "\n=== BUSCAR ESTUDIANTE ===\n";

    string criterio = leerTexto(
        "Ingrese RUT, nombre, apellido o carrera"
    );

    auto resultados = gestor.buscarEstudiantes(criterio);

    if (resultados.empty()) {
        cout << "\nNo se encontraron estudiantes.\n";
        return;
    }

    cout << "\nEstudiantes encontrados: "
         << resultados.size() << "\n";

    for (const Estudiante& estudiante : resultados) {
        mostrarEstudiante(estudiante);
    }
}

// ==========================================
// AC-003: EDITAR ESTUDIANTE
// ==========================================

void editarEstudiante(GestorEstudiantes& gestor) {
    cout << "\n=== EDITAR ESTUDIANTE ===\n";

    string rut = leerTexto("RUT del estudiante");

    auto encontrado = buscarPorRut(gestor, rut);

    if (!encontrado) {
        cout << "\nEstudiante no encontrado.\n";
        return;
    }

    cout << "\nDatos actuales:\n";
    mostrarEstudiante(*encontrado);

    DatosEstudiante datos = encontrado->obtenerDatos();

    cout << "\nIngrese los nuevos datos.\n";
    cout << "Presione ENTER para conservar el valor actual.\n";

    // El RUT no puede modificarse.
    datos.nombres = leerTexto(
        "Nombres", datos.nombres
    );

    datos.apellidoPaterno = leerTexto(
        "Apellido paterno", datos.apellidoPaterno
    );

    datos.apellidoMaterno = leerTexto(
        "Apellido materno", datos.apellidoMaterno
    );

    datos.fechaNacimiento = leerTexto(
        "Fecha nacimiento", datos.fechaNacimiento
    );

    datos.direccion = leerTexto(
        "Direccion", datos.direccion
    );

    datos.comuna = leerTexto(
        "Comuna", datos.comuna
    );

    datos.correoInstitucional = leerTexto(
        "Correo", datos.correoInstitucional
    );

    datos.anioIngreso = leerNumero(
        "Anio ingreso", datos.anioIngreso
    );

    datos.genero = leerTexto(
        "Genero", datos.genero
    );

    datos.carreraId = leerTexto(
        "Carrera", datos.carreraId
    );

    string confirmar = leerTexto(
        "Confirmar cambios (S/N)"
    );

    if (confirmar != "S" && confirmar != "s") {
        cout << "\nEdicion cancelada.\n";
        return;
    }

    if (gestor.editarEstudiante(rut, datos)) {
        cout << "\nEstudiante actualizado correctamente.\n";

        auto actualizado = buscarPorRut(gestor, rut);

        if (actualizado) {
            mostrarEstudiante(*actualizado);
        }
    } else {
        cout << "\nNo se pudo actualizar el estudiante.\n";
        cout << "Verifique los datos ingresados.\n";
    }
}

// ==========================================
// AC-004: CAMBIAR ESTADO
// ==========================================

void cambiarEstado(GestorEstudiantes& gestor) {
    cout << "\n=== CAMBIAR ESTADO ===\n";

    string rut = leerTexto("RUT del estudiante");

    auto encontrado = buscarPorRut(gestor, rut);

    if (!encontrado) {
        cout << "\nEstudiante no encontrado.\n";
        return;
    }

    cout << "\nEstudiante: "
         << encontrado->obtenerNombreCompleto() << "\n";

    cout << "Estado actual: "
         << encontrado->obtenerEstado() << "\n";

    cout << "\n1. Activo\n";
    cout << "2. Inactivo\n";

    int opcion = leerNumero("Nuevo estado");

    string nuevoEstado;

    if (opcion == 1) {
        nuevoEstado = "Activo";
    } else if (opcion == 2) {
        nuevoEstado = "Inactivo";
    } else {
        cout << "\nOpcion no valida.\n";
        return;
    }

    string motivo = leerTexto("Motivo del cambio");

    string responsable = leerTexto(
        "Identificador del usuario responsable"
    );

    string confirmar = leerTexto(
        "Confirmar cambio (S/N)"
    );

    if (confirmar != "S" && confirmar != "s") {
        cout << "\nOperacion cancelada.\n";
        return;
    }

    if (gestor.cambiarEstado(
            rut,
            nuevoEstado,
            motivo,
            responsable
        )) {
        cout << "\nEstado actualizado correctamente.\n";
    } else {
        cout << "\nNo se pudo cambiar el estado.\n";
        cout << "Revise el motivo, responsable y nuevo estado.\n";
    }
}

// ==========================================
// CONSULTAR HISTORIAL DE ESTADOS
// ==========================================

void consultarHistorial(const GestorEstudiantes& gestor) {
    cout << "\n=== HISTORIAL DE ESTADOS ===\n";

    string rut = leerTexto("RUT del estudiante");

    auto historial = gestor.consultarHistorial(rut);

    if (historial.empty()) {
        cout << "\nNo existen cambios de estado registrados.\n";
        return;
    }

    for (const HistorialEstado& registro : historial) {
        cout << "\n--------------------------------\n";
        cout << "RUT: " << registro.rutEstudiante << "\n";
        cout << "Estado anterior: "
             << registro.estadoAnterior << "\n";
        cout << "Estado nuevo: "
             << registro.estadoNuevo << "\n";
        cout << "Motivo: " << registro.motivo << "\n";
        cout << "Responsable: "
             << registro.usuarioResponsable << "\n";
        cout << "Fecha (timestamp): "
             << registro.fechaCambio << "\n";
        cout << "--------------------------------\n";
    }
}

// ==========================================
// LISTAR ESTUDIANTES
// ==========================================

void listarEstudiantes(const GestorEstudiantes& gestor) {
    cout << "\n=== LISTADO DE ESTUDIANTES ===\n";

    size_t total = 0;

    for (const string& estado : {"Activo", "Inactivo"}) {
        FiltrosEstudiante filtros{};
        filtros.estado = estado;

        for (size_t pagina = 1; ; pagina++) {
            auto resultados = gestor.buscarEstudiantes(
                filtros, pagina
            );

            if (resultados.empty()) {
                break;
            }

            for (const Estudiante& estudiante : resultados) {
                mostrarEstudiante(estudiante);
                total++;
            }
        }
    }

    cout << "\nTotal de estudiantes: " << total << "\n";
}

// ==========================================
// MENU PRINCIPAL
// ==========================================

void mostrarMenu() {
    cout << "\n========================================\n";
    cout << "              ACADEMIA\n";
    cout << "       GESTION DE ESTUDIANTES\n";
    cout << "========================================\n";

    cout << "1. Registrar estudiante\n";
    cout << "2. Buscar estudiante\n";
    cout << "3. Editar estudiante\n";
    cout << "4. Cambiar estado del estudiante\n";
    cout << "5. Consultar historial de estados\n";
    cout << "6. Listar estudiantes\n";
    cout << "0. Salir\n";

    cout << "========================================\n";
}

// ==========================================
// PROGRAMA PRINCIPAL
// ==========================================

int main() {
    GestorEstudiantes gestor;

    int opcion;

    do {
        mostrarMenu();

        opcion = leerNumero("Seleccione una opcion");

        switch (opcion) {
            case 1:
                registrarEstudiante(gestor);
                break;

            case 2:
                buscarEstudiante(gestor);
                break;

            case 3:
                editarEstudiante(gestor);
                break;

            case 4:
                cambiarEstado(gestor);
                break;

            case 5:
                consultarHistorial(gestor);
                break;

            case 6:
                listarEstudiantes(gestor);
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
