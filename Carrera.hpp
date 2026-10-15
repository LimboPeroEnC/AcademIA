
#pragma once

#include "Facultad.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

// Datos para la creacion de una carrera (ficha AC-032).
struct DatosCarrera {
    int id;
    int facultadId;
    std::string codigo;
    std::string nombre;
    int duracionSemestres;
    std::string modalidad;
};

// Estados disponibles para Carrera
enum class EstadoCarrera {
    Activo,
    Inactivo
};

// ========================================
// CLASE CARRERA
// ========================================

class Carrera {
private:
    int id_;
    int facultadId_;
    std::string codigo_;
    std::string nombre_;
    int duracionSemestres_;
    std::string modalidad_;
    EstadoCarrera estado_;

public:
    Carrera(const DatosCarrera& datos, EstadoCarrera estado);

    // Metodos de consulta
    int getId() const;
    int getFacultadId() const;
    const std::string& getCodigo() const;
    const std::string& getNombre() const;
    int getDuracionSemestres() const;
    const std::string& getModalidad() const;
    EstadoCarrera getEstado() const;

    // Metodos de modificacion
    void editar(
        const std::string& nombre,
        int duracionSemestres,
        const std::string& modalidad
    );

    void cambiarEstado(EstadoCarrera estado);
};

// ========================================
// FACTORY METHOD: CREADOR ABSTRACTO
// ========================================

class CreadorCarrera {
protected:
    virtual std::unique_ptr<Carrera> crearCarrera(
        const DatosCarrera& datos
    ) const = 0;

public:
    virtual ~CreadorCarrera() = default;

    std::unique_ptr<Carrera> prepararRegistro(
        const DatosCarrera& datos
    ) const;
};

// ========================================
// FACTORY METHOD: CREADOR CONCRETO
// ========================================

class CreadorCarreraActiva final : public CreadorCarrera {
protected:
    std::unique_ptr<Carrera> crearCarrera(
        const DatosCarrera& datos
    ) const override;
};

// ========================================
// GESTOR DE CARRERAS
// ========================================

class GestorCarreras {
private:
    const GestorFacultades& facultades_;
    const CreadorCarrera& fabrica_;

    std::unordered_map<
        int,
        std::unique_ptr<Carrera>
    > carreras_;

public:
    GestorCarreras(
        const GestorFacultades& facultades,
        const CreadorCarrera& fabrica
    );

    // Registrar una carrera
    void registrar(const DatosCarrera& datos);

    // Buscar carrera por ID
    const Carrera* buscar(int id) const;

    // Listar carreras asociadas a una Facultad
    std::vector<const Carrera*> listarPorFacultad(
        int facultadId
    ) const;

    // Editar informacion de Carrera
    void editar(
        int id,
        const std::string& nombre,
        int duracionSemestres,
        const std::string& modalidad
    );

    // Cambiar estado de Carrera
    void cambiarEstado(
        int id,
        EstadoCarrera estado
    );
};
