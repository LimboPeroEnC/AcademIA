
#pragma once

#include <memory>
#include <string>
#include <unordered_map>

// Estados disponibles para una facultad
enum class EstadoFacultad {
    Activo,
    Inactivo
};

// CLASE FACULTAD
class Facultad {
private:
    int id_;
    std::string codigo_;
    std::string nombre_;
    std::string descripcion_;
    EstadoFacultad estado_;

public:
    // Constructor
    Facultad(
        int id,
        std::string codigo,
        std::string nombre,
        std::string descripcion,
        EstadoFacultad estado
    );

    // Metodos de consulta (Getters)
    int getId() const;
    const std::string& getCodigo() const;
    const std::string& getNombre() const;
    const std::string& getDescripcion() const;
    EstadoFacultad getEstado() const;

    // Metodos de modificacion
    void editar(
        const std::string& nombre,
        const std::string& descripcion
    );

    void cambiarEstado(EstadoFacultad estado);
};

// FACTORY METHOD: CREADOR ABSTRACTO
class CreadorFacultad {
protected:
    // Metodo fabrica
    virtual std::unique_ptr<Facultad> crearFacultad(
        int id,
        const std::string& codigo,
        const std::string& nombre,
        const std::string& descripcion
    ) const = 0;

public:
    virtual ~CreadorFacultad() = default;

    std::unique_ptr<Facultad> prepararRegistro(
        int id,
        const std::string& codigo,
        const std::string& nombre,
        const std::string& descripcion
    ) const;
};

// FACTORY METHOD: CREADOR CONCRETO
class CreadorFacultadActiva final : public CreadorFacultad {
protected:
    std::unique_ptr<Facultad> crearFacultad(
        int id,
        const std::string& codigo,
        const std::string& nombre,
        const std::string& descripcion
    ) const override;
};

// GESTOR DE FACULTADES
class GestorFacultades {
private:
    std::unordered_map<
        int,
        std::unique_ptr<Facultad>
    > facultades_;

public:
    // Registrar una facultad
    void registrar(std::unique_ptr<Facultad> facultad);

    // Buscar una facultad por ID
    const Facultad* buscar(int id) const;

    // Editar una facultad
    void editar(
        int id,
        const std::string& nombre,
        const std::string& descripcion
    );

    // Cambiar estado de una facultad
    void cambiarEstado(
        int id,
        EstadoFacultad estado
    );
};
