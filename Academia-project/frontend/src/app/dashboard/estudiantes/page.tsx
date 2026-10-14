'use client';

import { useState, useEffect } from 'react';
import { useForm } from 'react-hook-form';
import DataTable from '@/components/ui/DataTable';
import Modal from '@/components/ui/Modal';

const mockEstudiantes = [
  { rut: '21.111.222-3', nombres: 'Carlos Andrés', apellidos: 'Soto Pérez', correo: 'carlos.soto@estudiantes.unap.cl', carrera: 'Ing. Civil en Computación', año_ingreso: 2024, estado: 'Activo' },
  { rut: '22.333.444-5', nombres: 'María Ignacia', apellidos: 'López Vega', correo: 'maria.lopez@estudiantes.unap.cl', carrera: 'Ing. Civil en Computación', año_ingreso: 2025, estado: 'Activo' },
  { rut: '20.555.666-7', nombres: 'Felipe Ignacio', apellidos: 'Tapia Silva', correo: 'felipe.tapia@estudiantes.unap.cl', carrera: 'Ing. Civil en Computación', año_ingreso: 2023, estado: 'Inactivo' },
];

export default function GestionEstudiantesPage() {
  const [isModalOpen, setIsModalOpen] = useState(false);
  // Estado para manejar el año actual de forma segura para Next.js
  const [currentYear, setCurrentYear] = useState(2026); 
  const { register, handleSubmit, reset } = useForm();

  // useEffect calcula el año real una vez que el componente ya está montado en el cliente
  useEffect(() => {
    setCurrentYear(new Date().getFullYear());
  }, []);

  const columns = [
    { header: 'RUT', accessor: 'rut' },
    { header: 'Nombres', accessor: 'nombres' },
    { header: 'Apellidos', accessor: 'apellidos' },
    { header: 'Carrera', accessor: 'carrera' },
    { header: 'Año Ingreso', accessor: 'año_ingreso' },
    { header: 'Estado', accessor: 'estado' },
  ];

  const handleRutInput = (e: React.ChangeEvent<HTMLInputElement>) => {
    let value = e.target.value.replace(/[^0-9kK]/g, '').toUpperCase();
    if (value.length > 1) {
      const dv = value.slice(-1);
      let body = value.slice(0, -1);
      body = body.replace(/\B(?=(\d{3})+(?!\d))/g, ".");
      e.target.value = `${body}-${dv}`;
    } else {
      e.target.value = value;
    }
  };

  const handleLettersOnly = (e: React.ChangeEvent<HTMLInputElement>) => {
    e.target.value = e.target.value.replace(/[^a-zA-ZáéíóúÁÉÍÓÚñÑ\s]/g, '');
  };

  const onSubmit = (data: any) => {
    console.log("Datos del nuevo estudiante:", data);
    alert('✅ Estudiante registrado exitosamente. Código de barra generado (Simulación)');
    setIsModalOpen(false);
    reset();
  };

  return (
    <div className="space-y-6">
      <div className="flex flex-col sm:flex-row justify-between items-start sm:items-center gap-4">
        <div>
          <h1 className="text-2xl font-bold text-gray-800">Gestión de Estudiantes</h1>
          <p className="text-sm text-gray-500 mt-1">
            Registra, busca y administra el estado de los alumnos matriculados.
          </p>
        </div>
        
        <button 
          onClick={() => setIsModalOpen(true)}
          className="bg-blue-600 hover:bg-blue-700 text-white px-4 py-2 rounded shadow transition-colors font-medium flex items-center"
        >
          <span className="mr-2 text-lg leading-none">+</span> Registrar Estudiante
        </button>
      </div>

      <DataTable 
        columns={columns} 
        data={mockEstudiantes} 
        searchPlaceholder="Buscar por nombre, RUT, carrera o estado..." 
      />

      <Modal isOpen={isModalOpen} onClose={() => setIsModalOpen(false)} title="Registrar Nuevo Estudiante">
        <form onSubmit={handleSubmit(onSubmit)} className="space-y-4">
          
          <div className="grid grid-cols-2 gap-4">
            <div>
              <label className="block text-sm font-medium text-gray-700 mb-1">RUT *</label>
              <input 
                type="text" 
                {...register('rut', { onChange: handleRutInput })} 
                className="w-full p-2 border border-gray-300 rounded focus:ring-2 focus:ring-blue-500" 
                placeholder="12.345.678-9" 
                maxLength={12}
                required 
              />
            </div>
            <div>
              <label className="block text-sm font-medium text-gray-700 mb-1">Correo Institucional *</label>
              <input type="email" {...register('correo')} className="w-full p-2 border border-gray-300 rounded focus:ring-2 focus:ring-blue-500" placeholder="alumno@estudiantes.unap.cl" required />
            </div>
          </div>

          <div className="grid grid-cols-1 md:grid-cols-3 gap-4">
            <div>
              <label className="block text-sm font-medium text-gray-700 mb-1">Nombres *</label>
              <input 
                type="text" 
                {...register('nombres', { onChange: handleLettersOnly })} 
                className="w-full p-2 border border-gray-300 rounded focus:ring-2 focus:ring-blue-500" 
                required 
              />
            </div>
            <div>
              <label className="block text-sm font-medium text-gray-700 mb-1">Apellido Paterno *</label>
              <input 
                type="text" 
                {...register('apellido_paterno', { onChange: handleLettersOnly })} 
                className="w-full p-2 border border-gray-300 rounded focus:ring-2 focus:ring-blue-500" 
                required 
              />
            </div>
            <div>
              <label className="block text-sm font-medium text-gray-700 mb-1">Apellido Materno *</label>
              <input 
                type="text" 
                {...register('apellido_materno', { onChange: handleLettersOnly })} 
                className="w-full p-2 border border-gray-300 rounded focus:ring-2 focus:ring-blue-500" 
                required 
              />
            </div>
          </div>

          <div className="grid grid-cols-2 gap-4">
            <div>
              <label className="block text-sm font-medium text-gray-700 mb-1">Fecha de Nacimiento *</label>
              <input type="date" {...register('fecha_nacimiento')} className="w-full p-2 border border-gray-300 rounded focus:ring-2 focus:ring-blue-500" required />
            </div>
            <div>
              <label className="block text-sm font-medium text-gray-700 mb-1">Género *</label>
              <select {...register('genero')} className="w-full p-2 border border-gray-300 rounded focus:ring-2 focus:ring-blue-500" required>
                <option value="">Seleccione...</option>
                <option value="Masculino">Masculino</option>
                <option value="Femenino">Femenino</option>
                <option value="Otro">Otro</option>
              </select>
            </div>
          </div>

          <div className="grid grid-cols-2 gap-4">
            <div>
              <label className="block text-sm font-medium text-gray-700 mb-1">Comuna de Residencia *</label>
              <input 
                type="text" 
                {...register('comuna', { onChange: handleLettersOnly })} 
                className="w-full p-2 border border-gray-300 rounded focus:ring-2 focus:ring-blue-500" 
                placeholder="Ej. Iquique" 
                required 
              />
            </div>
            <div>
              <label className="block text-sm font-medium text-gray-700 mb-1">Dirección *</label>
              <input type="text" {...register('direccion')} className="w-full p-2 border border-gray-300 rounded focus:ring-2 focus:ring-blue-500" required />
            </div>
          </div>

          <div className="grid grid-cols-2 gap-4">
            <div>
              <label className="block text-sm font-medium text-gray-700 mb-1">Año de Ingreso *</label>
              {/* Ahora usamos currentYear calculado de forma segura */}
              <input 
                type="number" 
                {...register('año_ingreso')} 
                className="w-full p-2 border border-gray-300 rounded focus:ring-2 focus:ring-blue-500" 
                min="2000" 
                max={currentYear} 
                required 
              />
            </div>
            <div>
              <label className="block text-sm font-medium text-gray-700 mb-1">Carrera *</label>
              <select {...register('carrera')} className="w-full p-2 border border-gray-300 rounded focus:ring-2 focus:ring-blue-500" required>
                <option value="">Seleccione una carrera...</option>
                <option value="Ingeniería Civil en Computación">Ing. Civil en Computación e Informática</option>
                <option value="Ingeniería Civil Industrial">Ing. Civil Industrial</option>
              </select>
            </div>
          </div>

          <div className="flex justify-end space-x-3 mt-6 pt-4 border-t border-gray-100">
            <button type="button" onClick={() => setIsModalOpen(false)} className="px-4 py-2 text-gray-600 hover:bg-gray-100 rounded transition-colors">
              Cancelar
            </button>
            <button type="submit" className="px-4 py-2 bg-blue-600 hover:bg-blue-700 text-white rounded transition-colors">
              Registrar Estudiante
            </button>
          </div>
        </form>
      </Modal>
    </div>
  );
}