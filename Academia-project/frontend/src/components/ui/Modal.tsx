'use client';

import { useEffect } from 'react';

interface ModalProps {
  isOpen: boolean;
  onClose: () => void;
  title: string;
  children: React.ReactNode;
}

export default function Modal({ isOpen, onClose, title, children }: ModalProps) {
  // Prevenir que el fondo haga scroll cuando el modal está abierto
  useEffect(() => {
    if (isOpen) {
      document.body.style.overflow = 'hidden';
    } else {
      document.body.style.overflow = 'unset';
    }
    return () => {
      document.body.style.overflow = 'unset';
    };
  }, [isOpen]);

  if (!isOpen) return null;

  return (
    <div className="fixed inset-0 z-50 flex items-center justify-center p-4 bg-black bg-opacity-50 transition-opacity">
      {/* Contenedor principal de la ventana */}
      <div className="bg-white rounded-lg shadow-xl w-full max-w-lg overflow-hidden flex flex-col max-h-[90vh]">
        
        {/* Cabecera del Modal */}
        <div className="flex justify-between items-center p-4 border-b border-gray-200 bg-gray-50">
          <h3 className="text-lg font-bold text-gray-800">{title}</h3>
          <button
            onClick={onClose}
            className="text-gray-400 hover:text-gray-600 hover:bg-gray-200 rounded-full w-8 h-8 flex items-center justify-center transition-colors"
          >
            ✕
          </button>
        </div>

        {/* Cuerpo del Modal (Aquí se inyectan los formularios) */}
        <div className="p-6 overflow-y-auto">
          {children}
        </div>
      </div>
    </div>
  );
}