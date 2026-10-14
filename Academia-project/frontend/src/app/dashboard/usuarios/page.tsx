'use client';

import { useState } from 'react';
import { useForm } from 'react-hook-form';
import DataTable from '@/components/ui/DataTable';
import Modal from '@/components/ui/Modal';

const mockUsuarios = [
  { rut: '19.123.456-7', nombres: 'Limbert', apellidos: 'Parra', correo: 'limbert.parra@academia.cl', rol: 'Administrador', estado: 'Activo' },
  { rut: '20.987.654-3', nombres: 'Javier', apellidos: 'Mancilla', correo: 'javier.mancilla@academia.cl', rol: 'Académico', estado: 'Activo' },
  { rut: '15.444.333-2', nombres: 'Johanan', apellidos: 'Najera', correo: 'johanan.najera@academia.cl', rol: 'Docente', estado: 'Inactivo' },
];

export default function GestionUsuariosPage() {
  const [isModalOpen, setIsModalOpen] = useState(false);
  const [editingUser, setEditingUser] = useState<any>(null); // Guardará el usuario a editar
  const { register, handleSubmit, reset } = useForm();

  const columns = [
    { header: 'RUT', accessor: 'rut' },
    { header: 'Nombres', accessor: 'nombres' },
    { header: 'Apellidos', accessor: 'apellidos' },
    { header: 'Correo', accessor: 'correo' },
    { header: 'Rol', accessor: 'rol' },
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

  // Función al guardar (Sirve para crear y para editar)
  const onSubmit = (data: any) => {
    if (editingUser) {
      alert('✅ Los datos fueron actualizados correctamente (SC-050)');
    } else {
      alert('✅ Usuario creado exitosamente (AC-048)');
    }
    closeModal();
  };

  // AC-050: Abrir modal en modo edición
  const handleEdit = (user: any) => {
    setEditingUser(user);
    reset(user); // Rellena el formulario con los datos del usuario seleccionado
    setIsModalOpen(true);
  };

  // SC-051: Cambiar estado (Inhabilitar requiere motivo)
  const handleToggleStatus = (user: any) => {
    if (user.estado === 'Activo') {
      const motivo = window.prompt('Ingrese el motivo de inhabilitación (SC-051):');
      if (motivo !== null && motivo.trim() !== '') {
        alert(`✅ El usuario fue inhabilitado correctamente. Motivo: ${motivo}`);
      } else if (motivo !== null) {
        alert('⚠️ El motivo de inhabilitación es obligatorio.');
      }
    } else {
      const confirmar = window.confirm('¿Desea volver a activar a este usuario?');
      if (confirmar) alert('✅ Usuario activado exitosamente.');
    }
  };

  // Limpiar estados al cerrar
  const closeModal = () => {
    setIsModalOpen(false);
    setEditingUser(null);
    reset({}); // Vacía el formulario
  };

  return (
    <div className="space-y-6">
      <div className="flex flex-col sm:flex-row justify-between items-start sm:items-center gap-4">
        <div>
          <h1 className="text-2xl font-bold text-gray-800">Gestión de Usuarios</h1>
          <p className="text-sm text-gray-500 mt-1">Administra los perfiles de acceso del sistema AcademIA.</p>
        </div>
        
        <button 
          onClick={() => { setEditingUser(null); reset({}); setIsModalOpen(true); }}
          className="bg-blue-600 hover:bg-blue-700 text-white px-4 py-2 rounded shadow transition-colors font-medium flex items-center"
        >
          <span className="mr-2 text-lg leading-none">+</span> Nuevo Usuario
        </button>
      </div>

      <DataTable 
        columns={columns} 
        data={mockUsuarios} 
        searchPlaceholder="Buscar por nombre, RUT o rol..." 
        onEdit={handleEdit} 
        onToggleStatus={handleToggleStatus} 
      />

      <Modal isOpen={isModalOpen} onClose={closeModal} title={editingUser ? "Editar Usuario" : "Crear Nuevo Usuario"}>
        <form onSubmit={handleSubmit(onSubmit)} className="space-y-4">
          
          <div className="grid grid-cols-2 gap-4">
            <div>
              <label className="block text-sm font-medium text-gray-700 mb-1">RUT</label>
              {/* Si estamos editando, el RUT se bloquea por seguridad según AC-050 */}
              <input 
                type="text" 
                {...register('rut', { onChange: handleRutInput })} 
                className={`w-full p-2 border rounded focus:ring-2 focus:ring-blue-500 ${editingUser ? 'bg-gray-100 border-gray-300 text-gray-500' : 'border-gray-300'}`}
                placeholder="12.345.678-9" 
                maxLength={12}
                required 
                readOnly={!!editingUser} 
              />
            </div>
            <div>
              <label className="block text-sm font-medium text-gray-700 mb-1">Rol</label>
              <select {...register('rol')} className="w-full p-2 border border-gray-300 rounded focus:ring-2 focus:ring-blue-500" required>
                <option value="">Seleccione...</option>
                <option value="Administrador">Administrador</option>
                <option value="Académico">Académico</option>
                <option value="Docente">Docente</option>
                <option value="Estudiante">Estudiante</option>
              </select>
            </div>
          </div>

          <div className="grid grid-cols-2 gap-4">
            <div>
              <label className="block text-sm font-medium text-gray-700 mb-1">Nombres</label>
              <input type="text" {...register('nombres', { onChange: handleLettersOnly })} className="w-full p-2 border border-gray-300 rounded focus:ring-2 focus:ring-blue-500" required />
            </div>
            <div>
              <label className="block text-sm font-medium text-gray-700 mb-1">Apellidos</label>
              <input type="text" {...register('apellidos', { onChange: handleLettersOnly })} className="w-full p-2 border border-gray-300 rounded focus:ring-2 focus:ring-blue-500" required />
            </div>
          </div>

          <div>
            <label className="block text-sm font-medium text-gray-700 mb-1">Correo Institucional</label>
            <input type="email" {...register('correo')} className="w-full p-2 border border-gray-300 rounded focus:ring-2 focus:ring-blue-500" placeholder="usuario@academia.cl" required />
          </div>

          {/* Solo pedimos contraseña provisional al CREAR un usuario */}
          {!editingUser && (
            <div>
              <label className="block text-sm font-medium text-gray-700 mb-1">Contraseña Provisional</label>
              <input type="password" {...register('password')} className="w-full p-2 border border-gray-300 rounded focus:ring-2 focus:ring-blue-500" required />
            </div>
          )}

          <div className="flex justify-end space-x-3 mt-6 pt-4 border-t border-gray-100">
            <button type="button" onClick={closeModal} className="px-4 py-2 text-gray-600 hover:bg-gray-100 rounded transition-colors">
              Cancelar
            </button>
            <button type="submit" className="px-4 py-2 bg-blue-600 hover:bg-blue-700 text-white rounded transition-colors">
              {editingUser ? "Actualizar Datos" : "Guardar Usuario"}
            </button>
          </div>
        </form>
      </Modal>
    </div>
  );
}