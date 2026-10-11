'use client';

import { useState } from 'react';
import { useForm } from 'react-hook-form';
import DataTable from '@/components/ui/DataTable';
import Modal from '@/components/ui/Modal';

// Mock data: Ramos basados en la carrera de Ingeniería Civil en Computación
const mockRamos = [
  { codigo: 'ICC-101', nombre: 'Introducción a la Programación', creditos: 6, semestre: '1', carrera: 'Ing. Civil en Computación', prerrequisitos: 'Ninguno', estado: 'Activo' },
  { codigo: 'MAT-102', nombre: 'Cálculo I', creditos: 8, semestre: '1', carrera: 'Ing. Civil en Computación', prerrequisitos: 'Ninguno', estado: 'Activo' },
  { codigo: 'ICC-201', nombre: 'Estructura de Datos', creditos: 6, semestre: '3', carrera: 'Ing. Civil en Computación', prerrequisitos: 'ICC-101', estado: 'Activo' },
  { codigo: 'ICC-405', nombre: 'Ingeniería de Software', creditos: 8, semestre: '7', carrera: 'Ing. Civil en Computación', prerrequisitos: 'ICC-201', estado: 'Inactivo' },
];

export default function GestionRamosPage() {
  const [isModalOpen, setIsModalOpen] = useState(false);
  const [editingRamo, setEditingRamo] = useState<any>(null);
  const { register, handleSubmit, reset } = useForm();

  // Columnas para la tabla según AC-018
  const columns = [
    { header: 'Código', accessor: 'codigo' },
    { header: 'Nombre del Ramo', accessor: 'nombre' },
    { header: 'Créditos', accessor: 'creditos' },
    { header: 'Nivel/Semestre', accessor: 'semestre' },
    { header: 'Prerrequisitos', accessor: 'prerrequisitos' },
    { header: 'Estado', accessor: 'estado' },
  ];

  // AC-017 y AC-019: Función al guardar
  const onSubmit = (data: any) => {
    if (editingRamo) {
      alert('✅ El ramo fue actualizado exitosamente (AC-019)');
    } else {
      alert('✅ El ramo fue registrado exitosamente (AC-017)');
    }
    closeModal();
  };

  // AC-019: Preparar modal para edición
  const handleEdit = (ramo: any) => {
    setEditingRamo(ramo);
    reset(ramo);
    setIsModalOpen(true);
  };

  // AC-020: Cambiar estado
  const handleToggleStatus = (ramo: any) => {
    if (ramo.estado === 'Activo') {
      const motivo = window.prompt('Ingrese el motivo para inactivar este ramo (AC-020):');
      if (motivo !== null && motivo.trim() !== '') {
        alert(`✅ El estado del ramo fue actualizado a Inactivo. Motivo: ${motivo}`);
      } else if (motivo !== null) {
        alert('⚠️ El motivo es obligatorio para inactivar el ramo.');
      }
    } else {
      const confirmar = window.confirm('¿Desea volver a activar este ramo?');
      if (confirmar) alert('✅ Ramo activado exitosamente.');
    }
  };

  const closeModal = () => {
    setIsModalOpen(false);
    setEditingRamo(null);
    reset({});
  };

  return (
    <div className="space-y-6">
      <div className="flex flex-col sm:flex-row justify-between items-start sm:items-center gap-4">
        <div>
          <h1 className="text-2xl font-bold text-gray-800">Gestión de Ramos</h1>
          <p className="text-sm text-gray-500 mt-1">
            Administra el catálogo de asignaturas, créditos y prerrequisitos.
          </p>
        </div>
        
        <button 
          onClick={() => { setEditingRamo(null); reset({}); setIsModalOpen(true); }}
          className="bg-blue-600 hover:bg-blue-700 text-white px-4 py-2 rounded shadow transition-colors font-medium flex items-center"
        >
          <span className="mr-2 text-lg leading-none">+</span> Registrar Ramo
        </button>
      </div>

      <DataTable 
        columns={columns} 
        data={mockRamos} 
        searchPlaceholder="Buscar por código, nombre o nivel..." 
        onEdit={handleEdit}
        onToggleStatus={handleToggleStatus}
      />

      <Modal isOpen={isModalOpen} onClose={closeModal} title={editingRamo ? "Editar Ramo" : "Registrar Nuevo Ramo"}>
        <form onSubmit={handleSubmit(onSubmit)} className="space-y-4">
          
          <div className="grid grid-cols-2 gap-4">
            <div>
              <label className="block text-sm font-medium text-gray-700 mb-1">Código del Ramo *</label>
              {/* Bloqueado si está editando (AC-019) */}
              <input 
                type="text" 
                {...register('codigo')} 
                className={`w-full p-2 border rounded focus:ring-2 focus:ring-blue-500 uppercase ${editingRamo ? 'bg-gray-100 border-gray-300 text-gray-500' : 'border-gray-300'}`}
                placeholder="Ej. ICC-101" 
                required 
                readOnly={!!editingRamo} 
              />
            </div>
            <div>
              <label className="block text-sm font-medium text-gray-700 mb-1">Cantidad de Créditos *</label>
              {/* Validación: Debe ser mayor a 0 (AC-017) */}
              <input 
                type="number" 
                {...register('creditos')} 
                className="w-full p-2 border border-gray-300 rounded focus:ring-2 focus:ring-blue-500" 
                min="1"
                required 
              />
            </div>
          </div>

          <div>
            <label className="block text-sm font-medium text-gray-700 mb-1">Nombre del Ramo *</label>
            <input type="text" {...register('nombre')} className="w-full p-2 border border-gray-300 rounded focus:ring-2 focus:ring-blue-500" required />
          </div>

          <div>
            <label className="block text-sm font-medium text-gray-700 mb-1">Descripción</label>
            <textarea {...register('descripcion')} className="w-full p-2 border border-gray-300 rounded focus:ring-2 focus:ring-blue-500" rows={3} placeholder="Descripción de la asignatura..." />
          </div>

          <div className="grid grid-cols-2 gap-4">
            <div>
              <label className="block text-sm font-medium text-gray-700 mb-1">Nivel / Semestre</label>
              <input type="number" {...register('semestre')} className="w-full p-2 border border-gray-300 rounded focus:ring-2 focus:ring-blue-500" min="1" max="12" />
            </div>
            <div>
              <label className="block text-sm font-medium text-gray-700 mb-1">Carrera *</label>
              <select {...register('carrera')} className="w-full p-2 border border-gray-300 rounded focus:ring-2 focus:ring-blue-500" required>
                <option value="">Seleccione...</option>
                <option value="Ingeniería Civil en Computación">Ing. Civil en Computación</option>
                <option value="Ingeniería Civil Industrial">Ing. Civil Industrial</option>
              </select>
            </div>
          </div>

          <div>
            <label className="block text-sm font-medium text-gray-700 mb-1">Prerrequisitos</label>
            <input type="text" {...register('prerrequisitos')} className="w-full p-2 border border-gray-300 rounded focus:ring-2 focus:ring-blue-500" placeholder="Ej. MAT-101, ICC-101 (Opcional)" />
          </div>

          <div className="flex justify-end space-x-3 mt-6 pt-4 border-t border-gray-100">
            <button type="button" onClick={closeModal} className="px-4 py-2 text-gray-600 hover:bg-gray-100 rounded transition-colors">
              Cancelar
            </button>
            <button type="submit" className="px-4 py-2 bg-blue-600 hover:bg-blue-700 text-white rounded transition-colors">
              {editingRamo ? "Guardar Cambios" : "Registrar Ramo"}
            </button>
          </div>
        </form>
      </Modal>
    </div>
  );
}