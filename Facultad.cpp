
#include "Facultad.hpp"

#include <stdexcept>
#include <utility>


Facultad::Facultad(
    int id,
    std::string codigo,
    std::string nombre,
    std::string descripcion,
    EstadoFacultad estado
)
    : id_(id),
      codigo_(std::move(codigo)),
      nombre_(std::move(nombre)),
      descripcion_(std::move(descripcion)),
      estado_(estado)
{
    if (id_ <= 0 || codigo_.empty() || nombre_.empty()) {
        throw std::invalid_argument(
            "Facultad: datos obligatorios invalidos"
        );
    }
}

int Facultad::getId() const {
    return id_;
}

const std::string& Facultad::getCodigo() const {
    return codigo_;
}

const std::string& Facultad::getNombre() const {
    return nombre_;
}

const std::string& Facultad::getDescripcion() const {
    return descripcion_;
}

EstadoFacultad Facultad::getEstado() const {
    return estado_;
}

void Facultad::editar(
    const std::string& nombre,
    const std::string& descripcion
) {
    if (nombre.empty()) {
        throw std::invalid_argument(
            "Nombre de facultad obligatorio"
        );
    }

    nombre_ = nombre;
    descripcion_ = descripcion;
}

void Facultad::cambiarEstado(EstadoFacultad estado) {
    estado_ = estado;
}

// ========================================
// FACTORY METHOD
// ========================================

// CREADOR ABSTRACTO
std::unique_ptr<Facultad>
CreadorFacultad::prepararRegistro(
    int id,
    const std::string& codigo,
    const std::string& nombre,
    const std::string& descripcion
) const {

    // Validar informacion antes de crear
    if (id <= 0 || codigo.empty() || nombre.empty()) {
        throw std::invalid_argument(
            "Datos de facultad incompletos"
        );
    }

    // Delegar la creacion al Factory Method
    return crearFacultad(
        id,
        codigo,
        nombre,
        descripcion
    );
}

// CREADOR CONCRETO
std::unique_ptr<Facultad>
CreadorFacultadActiva::crearFacultad(
    int id,
    const std::string& codigo,
    const std::string& nombre,
    const std::string& descripcion
) const {

    // Crear facultad con estado Activo
    return std::make_unique<Facultad>(
        id,
        codigo,
        nombre,
        descripcion,
        EstadoFacultad::Activo
    );
}

// ========================================
// GESTOR DE FACULTADES
// ========================================

// REGISTRAR FACULTAD
void GestorFacultades::registrar(
    std::unique_ptr<Facultad> facultad
) {
    if (!facultad) {
        throw std::invalid_argument(
            "Facultad nula"
        );
    }

    // Validar que el ID sea unico
    if (facultades_.count(facultad->getId())) {
        throw std::invalid_argument(
            "Ya existe una facultad con ese ID"
        );
    }

    // Validar que el codigo sea unico
    for (const auto& item : facultades_) {
        if (
            item.second->getCodigo() ==
            facultad->getCodigo()
        ) {
            throw std::invalid_argument(
                "Ya existe ese codigo de facultad"
            );
        }
    }

    // Registrar en memoria
    facultades_.emplace(
        facultad->getId(),
        std::move(facultad)
    );
}

// BUSCAR FACULTAD
const Facultad* GestorFacultades::buscar(int id) const {
    const auto it = facultades_.find(id);

    if (it == facultades_.end()) {
        return nullptr;
    }

    return it->second.get();
}

// EDITAR FACULTAD
void GestorFacultades::editar(
    int id,
    const std::string& nombre,
    const std::string& descripcion
) {
    const auto it = facultades_.find(id);

    if (it == facultades_.end()) {
        throw std::invalid_argument(
            "Facultad no encontrada"
        );
    }

    it->second->editar(nombre, descripcion);
}

// CAMBIAR ESTADO DE FACULTAD
void GestorFacultades::cambiarEstado(
    int id,
    EstadoFacultad estado
) {
    const auto it = facultades_.find(id);

    if (it == facultades_.end()) {
        throw std::invalid_argument(
            "Facultad no encontrada"
        );
    }

    it->second->cambiarEstado(estado);
}
