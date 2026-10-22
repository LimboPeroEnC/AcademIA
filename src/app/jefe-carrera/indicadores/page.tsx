'use client';

import { useState } from 'react';
import { IndicadorEstudiante } from '@/types/indicadores';
import { getIndicadoresEstudiante } from '@/services/api';

export default function IndicadoresPage() {
  const [search, setSearch] = useState('');
  const [data, setData] = useState<IndicadorEstudiante | null>(null);
  const [searched, setSearched] = useState(false);
  const [loading, setLoading] = useState(false);

  const handleSearch = async (e: React.FormEvent) => {
    e.preventDefault();
    if (!search.trim()) return;

    setLoading(true);
    setSearched(true);

    const resultado = await getIndicadoresEstudiante(search);
    setData(resultado);
    setLoading(false);
  };

  return (
    <div className="p-6 max-w-5xl mx-auto space-y-6">
      <header>
        <h1 className="text-2xl font-bold text-gray-800">Indicadores Individuales de Rendimiento</h1>
        <p className="text-sm text-gray-500">Módulo de Seguimiento - Jefe de Carrera</p>
      </header>

      <form onSubmit={handleSearch} className="flex gap-3 bg-white p-4 rounded border shadow-sm">
        <input
          type="text"
          placeholder="Buscar por RUT o Nombre del estudiante..."
          value={search}
          onChange={(e) => setSearch(e.target.value)}
          className="flex-1 px-3 py-2 border rounded focus:outline-none"
        />
        <button
          type="submit"
          disabled={loading}
          className="px-5 py-2 bg-blue-600 text-white rounded hover:bg-blue-700 disabled:bg-blue-300 transition"
        >
          {loading ? 'Buscando...' : 'Buscar'}
        </button>
      </form>

      {searched && !loading && !data && (
        <div className="p-4 bg-yellow-50 border border-yellow-200 text-yellow-800 rounded">
          No se encontraron resultados para el estudiante ingresado o el acceso está restringido.
        </div>
      )}

      {data && (
        <div className="space-y-4">
          <div className="bg-white p-4 rounded border flex justify-between items-center shadow-sm">
            <div>
              <h2 className="text-lg font-bold">{data.nombreCompleto}</h2>
              <p className="text-sm text-gray-500">RUT: {data.rut} | {data.carrera}</p>
            </div>
            <span className="px-3 py-1 rounded-full text-xs font-semibold bg-green-100 text-green-800">
              {data.estadoAcademico}
            </span>
          </div>

          <div className="grid grid-cols-1 md:grid-cols-3 gap-4">
            <div className="bg-white p-4 rounded border shadow-sm">
              <span className="text-xs text-gray-400 font-semibold uppercase">Promedio Ponderado</span>
              <p className="text-3xl font-bold text-blue-600 mt-1">{data.promedioPonderado}</p>
            </div>
            <div className="bg-white p-4 rounded border shadow-sm">
              <span className="text-xs text-gray-400 font-semibold uppercase">Asistencia Global</span>
              <p className="text-3xl font-bold text-green-600 mt-1">{data.porcentajeAsistencia}%</p>
            </div>
            <div className="bg-white p-4 rounded border shadow-sm">
              <span className="text-xs text-gray-400 font-semibold uppercase">Avance Curricular</span>
              <p className="text-3xl font-bold text-purple-600 mt-1">
                {Math.round((data.creditosAprobados / data.creditosTotales) * 100)}%
              </p>
            </div>
          </div>
        </div>
      )}
    </div>
  );
}