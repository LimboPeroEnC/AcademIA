#ifndef ESTUDIANTE_H
#define ESTUDIANTE_H
#include <string>

// Se conservan nombres y metodos de la clase original.
struct DatosEstudiante {
    std::string rut;
    std::string nombres;
    std::string apellidoPaterno;
    std::string apellidoMaterno;
    std::string fechaNacimiento;    // YYYY-MM-DD
    std::string direccion;
    std::string comuna;
    std::string correoInstitucional;
    int anioIngreso = 0;           // compatibilidad con el codigo original
    std::string genero;
    std::string carreraId;
    // Campos agregados para columnas reales de public.estudiante:
    std::string fechaIngreso;       // YYYY-MM-DD -> anio_ingreso DATE
    std::string usuarioId;          // id_usuario nullable
};

class Estudiante {
private:
    DatosEstudiante datos;
    std::string codigoBarra;
    std::string estado;
public:
    Estudiante(const DatosEstudiante& datos, const std::string& codigoBarra);
    std::string obtenerRut() const;
    std::string obtenerNombreCompleto() const;
    std::string obtenerEstado() const;
    std::string obtenerCodigoBarra() const;
    DatosEstudiante obtenerDatos() const;
    void actualizarDatos(const DatosEstudiante& nuevosDatos);
    void establecerEstado(const std::string& nuevoEstado);
};
#endif
