    
#ifndef CURSO_HPP
#define CURSO_HPP

#include <string>
#include <vector>

// ==========================================
// ESTADOS DEL CURSO
// ==========================================

enum class EstadoCurso {
    Activo,
    Cerrado
};

// ==========================================
// HORARIO DEL CURSO
// ==========================================

// Dias:
// 1 = Lunes
// 2 = Martes
// 3 = Miercoles
// 4 = Jueves
// 5 = Viernes
// 6 = Sabado
// 7 = Domingo
//
// Las horas se guardan en minutos
// desde las 00:00.
//
// Ejemplo:
// 08:30 = 510 minutos
// 10:00 = 600 minutos

struct HorarioCurso {
    int dia;
    int horaInicio;
    int horaFin;
};

// ==========================================
// DATOS DEL CURSO
// ==========================================

struct DatosCurso {
    std::string codigo;
    std::string codigoAsignatura;
    std::string codigoPeriodo;
    std::string seccion;
    int capacidadMaxima;
    std::string modalidad;
    std::vector<HorarioCurso> horarios;
};

// ==========================================
// CLASE CURSO
// ==========================================

class Curso {

private:
    DatosCurso datos;

    EstadoCurso estado;

    std::string identificadorDocente;

    int cantidadInscritos;

public:

    // Constructor
    explicit Curso(
        const DatosCurso& datos
    );

    // ======================================
    // CONSULTAR INFORMACION
    // ======================================

    const DatosCurso& obtenerDatos() const;

    EstadoCurso obtenerEstado() const;

    std::string obtenerEstadoTexto() const;

    std::string obtenerDocente() const;

    int obtenerCantidadInscritos() const;

    int obtenerCuposDisponibles() const;

    // ======================================
    // MODIFICAR INFORMACION
    // ======================================

    bool actualizarDatos(
        const DatosCurso& nuevosDatos
    );

    // ======================================
    // ASIGNACION DOCENTE
    // ======================================

    bool asignarDocente(
        const std::string& identificador
    );

    bool quitarDocente();

    // ======================================
    // GESTION DE CUPOS
    // ======================================

    bool ocuparCupo();

    bool liberarCupo();

    // ======================================
    // GESTION DE ESTADO
    // ======================================

    bool cerrarCurso();
};

#endif
