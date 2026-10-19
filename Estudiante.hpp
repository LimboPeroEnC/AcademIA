
#ifndef ESTUDIANTE_H
#define ESTUDIANTE_H

#include <string>

struct DatosEstudiante {
    std::string rut;
    std::string nombres;
    std::string apellidoPaterno;
    std::string apellidoMaterno;
    std::string fechaNacimiento;
    std::string direccion;
    std::string comuna;
    std::string correoInstitucional;
    int anioIngreso;
    std::string genero;
    std::string carreraId;
};

class Estudiante {
private:
    DatosEstudiante datos;
    std::string codigoBarra;
    std::string estado;

public:
    Estudiante(
        const DatosEstudiante& datos,
        const std::string& codigoBarra
    );

    std::string obtenerRut() const;
    std::string obtenerNombreCompleto() const;
    std::string obtenerEstado() const;
    std::string obtenerCodigoBarra() const;

    DatosEstudiante obtenerDatos() const;

    void actualizarDatos(
        const DatosEstudiante& nuevosDatos
    );

    void establecerEstado(
        const std::string& nuevoEstado
    );
};

#endif
