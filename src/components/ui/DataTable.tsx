'use client';

import { useState } from 'react';

interface Column {
  header: string;
  accessor: string;
}

interface DataTableProps {
  columns: Column[];
  data: any[];
  searchPlaceholder?: string;
  onEdit?: (item: any) => void;
  onToggleStatus?: (item: any) => void;
}

export default function DataTable({ columns, data, searchPlaceholder = "Buscar...", onEdit, onToggleStatus }: DataTableProps) {
  const [searchTerm, setSearchTerm] = useState('');

  const filteredData = data.filter((row) =>
    Object.values(row).some((val) =>
      String(val).toLowerCase().includes(searchTerm.toLowerCase())
    )
  );

  return (
    <div className="bg-white rounded-lg shadow overflow-hidden border border-gray-200">
      <div className="p-4 border-b border-gray-200 bg-gray-50 flex justify-between items-center">
        <input
          type="text"
          placeholder={searchPlaceholder}
          className="p-2 border border-gray-300 rounded focus:outline-none focus:ring-2 focus:ring-blue-500 w-full max-w-sm"
          value={searchTerm}
          onChange={(e) => setSearchTerm(e.target.value)}
        />
        <span className="text-sm text-gray-500">Total: {filteredData.length} registros</span>
      </div>

      <div className="overflow-x-auto">
        <table className="w-full text-left border-collapse">
          <thead>
            <tr className="bg-blue-900 text-white">
              {columns.map((col, index) => (
                <th key={index} className="p-3 font-semibold text-sm whitespace-nowrap">{col.header}</th>
              ))}
              <th className="p-3 font-semibold text-sm whitespace-nowrap text-center">Acciones</th>
            </tr>
          </thead>
          <tbody>
            {filteredData.length > 0 ? (
              filteredData.map((row, rowIndex) => (
                <tr key={rowIndex} className="border-b border-gray-100 hover:bg-gray-50 transition-colors">
                  {columns.map((col, colIndex) => (
                    <td key={colIndex} className="p-3 text-sm text-gray-700">
                      {col.accessor === 'estado' ? (
                        <span className={`px-2 py-1 rounded text-xs font-bold ${
                          row[col.accessor] === 'Activo' ? 'bg-green-100 text-green-700' : 'bg-red-100 text-red-700'
                        }`}>
                          {row[col.accessor]}
                        </span>
                      ) : (
                        row[col.accessor]
                      )}
                    </td>
                  ))}
                  
                  <td className="p-3 text-sm text-center whitespace-nowrap">
                    {/* Botón Editar conectado */}
                    <button 
                      onClick={() => onEdit && onEdit(row)}
                      className="text-blue-600 hover:text-blue-800 font-medium mx-2 transition-colors"
                    >
                      Editar
                    </button>
                    {/* Botón Estado conectado */}
                    <button 
                      onClick={() => onToggleStatus && onToggleStatus(row)}
                      className="text-orange-500 hover:text-orange-700 font-medium mx-2 transition-colors"
                    >
                      Estado
                    </button>
                  </td>
                </tr>
              ))
            ) : (
              <tr>
                <td colSpan={columns.length + 1} className="p-6 text-center text-gray-500">
                  No se encontraron resultados para "{searchTerm}"
                </td>
              </tr>
            )}
          </tbody>
        </table>
      </div>
    </div>
  );
}