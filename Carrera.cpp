
#include "Carrera.hpp"

#include <stdexcept>
#include <utility>

Carrera::Carrera(
    const DatosCarrera& datos,
    EstadoCarrera estado
)
    : id_(datos.id),
      facultadId_(datos.facultadId),
      codigo_(datos.codigo),
      nombre_(datos.nombre),
      duracionSemestres_(datos.duracionSemestres),
      modalidad_(datos.modalidad),
      estado_(estado)
{
    if (
        id_ <= 0 ||
        facultadId_ <= 0 ||
        codigo_.empty() ||
        nombre_.empty() ||
        duracionSemestres_ <= 0 ||
        modalidad_.empty()
    ) {
        throw std::invalid_argument(
            "Carrera: datos obligatorios invalidos"
        );
    }
}


int Carrera::getId() const {
    return id_;
}

int Carrera::getFacultadId() const {
    return facultadId_;
}

const std::string& Carrera::getCodigo() const {
    return codigo_;
}

const std::string& Carrera::getNombre() const {
    return nombre_;
}

int Carrera::getDuracionSemestres() const {
    return duracionSemestres_;
}

const std::string& Carrera::getModalidad() const {
    return modalidad_;
}

EstadoCarrera Carrera::getEstado() const {
    return estado_;
}

// ========================================
// EDITAR CARRERA
// ========================================

void Carrera::editar(
    const std::string& nombre,
    int duracionSemestres,
    const std::string& modalidad
) {
    if (
        nombre.empty() ||
        duracionSemestres <= 0 ||
        modalidad.empty()
    ) {
        throw std::invalid_argument(
            "Datos de edicion de Carrera invalidos"
        );
    }

    nombre_ = nombre;
    duracionSemestres_ = duracionSemestres;
    modalidad_ = modalidad;
}

// ========================================
// CAMBIAR ESTADO
// ========================================

void Carrera::cambiarEstado(
    EstadoCarrera estado
) {
    estado_ = estado;
}

// ========================================
// IMPLEMENTACION FACTORY METHOD
// ========================================

// CREADOR ABSTRACTO
std::unique_ptr<Carrera>
CreadorCarrera::prepararRegistro(
    const DatosCarrera& datos
) const {

    if (
        datos.id <= 0 ||
        datos.facultadId <= 0 ||
        datos.codigo.empty() ||
        datos.nombre.empty() ||
        datos.duracionSemestres <= 0 ||
        datos.modalidad.empty()
    ) {
        throw std::invalid_argument(
            "Datos obligatorios de Carrera incompletos"
        );
    }

    return crearCarrera(datos);
}

std::unique_ptr<Carrera>
CreadorCarreraActiva::crearCarrera(
    const DatosCarrera& datos
) const {

    // Crear una carrera con estado inicial Activo
    return std::make_unique<Carrera>(
        datos,
        EstadoCarrera::Activo
    );
}


GestorCarreras::GestorCarreras(
    const GestorFacultades& facultades,
    const CreadorCarrera& fabrica
)
    : facultades_(facultades),
      fabrica_(fabrica)
{
}


void GestorCarreras::registrar(
    const DatosCarrera& datos
) {

    // Verificar que la Facultad exista
    const Facultad* facultad =
        facultades_.buscar(datos.facultadId);

    if (!facultad) {
        throw std::invalid_argument(
            "No existe la facultad indicada"
        );
    }

    if (
        facultad->getEstado() !=
        EstadoFacultad::Activo
    ) {
        throw std::invalid_argument(
            "No se puede registrar en facultad inactiva"
        );
    }

    if (carreras_.count(datos.id)) {
        throw std::invalid_argument(
            "Ya existe una carrera con ese ID"
        );
    }

    // Verificar que el codigo sea unico
    for (const auto& item : carreras_) {
        if (
            item.second->getCodigo() ==
            datos.codigo
        ) {
            throw std::invalid_argument(
                "Ya existe ese codigo de carrera"
            );
        }
    }

    // Crear Carrera mediante Factory Method
    auto carrera = fabrica_.prepararRegistro(datos);

    // Registrar en memoria
    carreras_.emplace(
        carrera->getId(),
        std::move(carrera)
    );
}


const Carrera* GestorCarreras::buscar(
    int id
) const {

    const auto it = carreras_.find(id);

    if (it == carreras_.end()) {
        return nullptr;
    }

    return it->second.get();
}


std::vector<const Carrera*>
GestorCarreras::listarPorFacultad(
    int facultadId
) const {

    std::vector<const Carrera*> resultado;

    for (const auto& item : carreras_) {
        if (
            item.second->getFacultadId() ==
            facultadId
        ) {
            resultado.push_back(
                item.second.get()
            );
        }
    }

    return resultado;
}

void GestorCarreras::editar(
    int id,
    const std::string& nombre,
    int duracionSemestres,
    const std::string& modalidad
) {

    const auto it = carreras_.find(id);

    if (it == carreras_.end()) {
        throw std::invalid_argument(
            "Carrera no encontrada"
        );
    }

    it->second->editar(
        nombre,
        duracionSemestres,
        modalidad
    );
}


void GestorCarreras::cambiarEstado(
    int id,
    EstadoCarrera estado
) {

    const auto it = carreras_.find(id);

    if (it == carreras_.end()) {
        throw std::invalid_argument(
            "Carrera no encontrada"
        );
    }

    it->second->cambiarEstado(estado);
}

