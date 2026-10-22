'use client';

import { useState, useEffect } from 'react';
import { estimarDemandaFutura, type ResultadoEstimacionDemanda, type DemandaAsignatura } from './actions';

export default function EstimarDemandaPage() {
  const [periodo, setPeriodo] = useState('2026-2');
  const [tamanoCupo, setTamanoCupo] = useState(30);
  const [data, setData] = useState<ResultadoEstimacionDemanda | null>(null);
  const [loading, setLoading] = useState(false);

  const cargarEstimacion = async () => {
    setLoading(true);
    const res = await estimarDemandaFutura('TODAS', periodo, tamanoCupo);
    setData(res);
    setLoading(false);
  };

  useEffect(() => {
    cargarEstimacion();
  }, []);

  return (
    <div className="p-6 max-w-5xl mx-auto space-y-6">
      <header className="border-b pb-4">
        <h1 className="text-2xl font-bold text-gray-800">Estimación de Demanda Futura de Secciones y Cupos</h1>
        <p className="text-sm text-gray-500">Módulo Jefe de Carrera - Planificación Académica Proyectada</p>
      </header>

      {/* PANEL DE CONFIGURACIÓN DE PARÁMETROS */}
      <div className="bg-white p-4 rounded border shadow-sm space-y-3">
        <h3 className="text-sm font-bold text-gray-700 uppercase tracking-wide">Parámetros de Proyección</h3>
        
        <div className="grid grid-cols-1 sm:grid-cols-3 gap-4">
          <div>
            <label className="block text-xs font-semibold text-gray-500 mb-1">Período Académico a Proyectar</label>
            <select
              value={periodo}
              onChange={(e) => setPeriodo(e.target.value)}
              className="w-full px-3 py-2 border rounded text-sm bg-white focus:outline-none"
            >
              <option value="2026-2">2026-2 (Próximo Semestre)</option>
              <option value="2027-1">2027-1</option>
              <option value="2027-2">2027-2</option>
            </select>
          </div>

          <div>
            <label className="block text-xs font-semibold text-gray-500 mb-1">Capacidad Máxima por Sección (Cupos)</label>
            <input
              type="number"
              min="10"
              max="100"
              value={tamanoCupo}
              onChange={(e) => setTamanoCupo(parseInt(e.target.value, 10) || 30)}
              className="w-full px-3 py-2 border rounded text-sm focus:outline-none"
            />
          </div>

          <div className="flex items-end">
            <button
              onClick={cargarEstimacion}
              disabled={loading}
              className="w-full px-4 py-2 bg-gray-800 text-white rounded text-sm hover:bg-gray-900 transition disabled:opacity-50"
            >
              {loading ? 'Calculando BD...' : 'Reescalar Estimación'}
            </button>
          </div>
        </div>
      </div>

      {data && (
        <div className="space-y-6">
          {/* DISTINCIÓN DE HISTÓRICOS VS SIMULADOS (CA 1) */}
          <div className="grid grid-cols-1 sm:grid-cols-3 gap-4">
            <div className="bg-white p-4 rounded border shadow-sm border-l-4 border-l-blue-500">
              <span className="text-xs text-gray-400 font-semibold uppercase">Base Estudiantes Activos</span>
              <p className="text-3xl font-bold text-blue-600 mt-1">{data.totalEstudiantesConsiderados} Alumnos</p>
              <p className="text-xs text-gray-400 mt-1">Dato Histórico PostgreSQL</p>
            </div>

            <div className="bg-white p-4 rounded border shadow-sm border-l-4 border-l-purple-500">
              <span className="text-xs text-gray-400 font-semibold uppercase">Período Simulado</span>
              <p className="text-3xl font-bold text-purple-600 mt-1">{data.periodoProyectado}</p>
              <p className="text-xs text-gray-400 mt-1">Simulación con Rezago e Inserción</p>
            </div>

            <div className="bg-white p-4 rounded border shadow-sm border-l-4 border-l-green-500">
              <span className="text-xs text-gray-400 font-semibold uppercase">Secciones Totales Proyectadas</span>
              <p className="text-3xl font-bold text-green-600 mt-1">
                {data.demandas.reduce((acc, item) => acc + item.seccionesRequeridas, 0)} Secciones
              </p>
              <p className="text-xs text-gray-400 mt-1">Para toda la Malla</p>
            </div>
          </div>

          {data.mensajeControl ? (
            <div className="p-4 bg-yellow-50 border border-yellow-200 text-yellow-800 rounded text-sm">
              {data.mensajeControl}
            </div>
          ) : (
            /* TABLA DE DEMANDA POR ASIGNATURA Y PERÍODO (CA 2) */
            <div className="bg-white p-6 rounded border shadow-sm space-y-4">
              <div className="flex justify-between items-center border-b pb-3">
                <div>
                  <h3 className="text-md font-bold text-gray-800">
                    Proyección de Demanda por Asignatura ({data.periodoProyectado})
                  </h3>
                  <p className="text-xs text-gray-500">
                    Cálculo basado en cupos por sección de {tamanoCupo} alumnos.
                  </p>
                </div>
                <span className="text-xs bg-gray-100 border px-2.5 py-1 rounded text-gray-600 font-mono">
                  PostgreSQL Sync OK
                </span>
              </div>

              <div className="overflow-x-auto">
                <table className="w-full text-left border-collapse">
                  <thead>
                    <tr className="border-b text-xs text-gray-500 uppercase bg-gray-50">
                      <th className="py-2 px-3">Semestre Malla</th>
                      <th className="py-2 px-3">Código</th>
                      <th className="py-2 px-3">Asignatura</th>
                      <th className="py-2 px-3 text-center">Demanda Histórica</th>
                      <th className="py-2 px-3 text-center">Demanda Simula (Futura)</th>
                      <th className="py-2 px-3 text-center font-bold">Secciones Requeridas</th>
                      <th className="py-2 px-3 text-center">Cupos Proyectados</th>
                    </tr>
                  </thead>
                  <tbody className="divide-y text-sm text-gray-700">
                    {data.demandas.map((item: DemandaAsignatura) => (
                      <tr key={item.idRamo} className="hover:bg-gray-50">
                        <td className="py-2 px-3 font-semibold text-xs text-gray-500">{item.semestreMalla}° Sem.</td>
                        <td className="py-2 px-3 font-mono text-xs">{item.idRamo}</td>
                        <td className="py-2 px-3 font-medium">{item.nombreRamo}</td>
                        <td className="py-2 px-3 text-center text-gray-500">{item.estudiantesHistoricos} al.</td>
                        <td className="py-2 px-3 text-center font-semibold text-purple-700">{item.estudiantesSimulados} al.</td>
                        <td className="py-2 px-3 text-center">
                          <span className="px-2.5 py-1 bg-green-100 text-green-800 rounded font-bold text-xs">
                            {item.seccionesRequeridas} Secc.
                          </span>
                        </td>
                        <td className="py-2 px-3 text-center text-xs font-mono">{item.cuposTotalesProyectados} cupos</td>
                      </tr>
                    ))}
                  </tbody>
                </table>
              </div>
            </div>
          )}
        </div>
      )}
    </div>
  );
}