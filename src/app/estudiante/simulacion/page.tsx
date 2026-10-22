'use client';

import { useState } from 'react';
import { simularProgresionAcademica, type ResultadoSimulacionBD, type AsignaturaSimulacion } from './actions';

export default function SimulacionProgresionPage() {
  const [rutInput, setRutInput] = useState('11111111-1');
  const [data, setData] = useState<ResultadoSimulacionBD | null>(null);
  const [loading, setLoading] = useState(false);
  const [searched, setSearched] = useState(false);
  const [filtroEstado, setFiltroEstado] = useState<'Todas' | 'Habilitada' | 'Bloqueada' | 'Ya Aprobada'>('Todas');

  const [seleccionadasProyeccion, setSeleccionadasProyeccion] = useState<string[]>([]);

  const handleSimular = async (e: React.FormEvent) => {
    e.preventDefault();
    if (!rutInput.trim()) return;

    setLoading(true);
    setSearched(true);
    setSeleccionadasProyeccion([]);

    const res = await simularProgresionAcademica(rutInput);
    setData(res);
    setLoading(false);
  };

  const toggleSeleccionAsignatura = (codigo: string) => {
    setSeleccionadasProyeccion((prev) =>
      prev.includes(codigo) ? prev.filter((c) => c !== codigo) : [...prev, codigo]
    );
  };

  const creditosProyectadosAdicionales = data
    ? data.asignaturas
        .filter((a: AsignaturaSimulacion) => seleccionadasProyeccion.includes(a.codigo))
        .reduce((sum: number, a: AsignaturaSimulacion) => sum + a.creditos, 0)
    : 0;

  const creditosTotalesProyectados = data
    ? data.proyeccion.creditosAprobadosActuales + creditosProyectadosAdicionales
    : 0;

  const porcentajeProyectadoCalculado = data
    ? Math.round((creditosTotalesProyectados / data.proyeccion.creditosTotalesCarrera) * 100)
    : 0;

  const asignaturasFiltradas: AsignaturaSimulacion[] = data?.asignaturas
    ? data.asignaturas.filter(
        (a: AsignaturaSimulacion) => filtroEstado === 'Todas' || a.estadoHabilitacion === filtroEstado
      )
    : [];

  return (
    <div className="p-6 max-w-5xl mx-auto space-y-6">
      <header className="border-b pb-4">
        <h1 className="text-2xl font-bold text-gray-800">Simulación y Proyección de Progresión Académica</h1>
        <p className="text-sm text-gray-500">Módulo Estudiante - Identificación de Prerrequisitos y Proyección Individual</p>
      </header>

      {/* Formulario de Consulta */}
      <form onSubmit={handleSimular} className="flex gap-3 bg-white p-4 rounded border shadow-sm">
        <div className="flex-1">
          <label className="block text-xs font-semibold text-gray-500 mb-1">RUT Estudiante</label>
          <input
            type="text"
            placeholder="Ej: 11111111-1"
            value={rutInput}
            onChange={(e) => setRutInput(e.target.value)}
            className="w-full px-3 py-2 border rounded text-sm focus:outline-none"
          />
        </div>
        <div className="flex items-end">
          <button
            type="submit"
            disabled={loading}
            className="px-5 py-2 bg-blue-600 text-white rounded text-sm hover:bg-blue-700 disabled:bg-blue-300 transition"
          >
            {loading ? 'Consultando...' : 'Cargar Malla y Proyectar'}
          </button>
        </div>
      </form>

      {searched && !loading && !data && (
        <div className="p-4 bg-yellow-50 border border-yellow-200 text-yellow-800 rounded text-sm">
          El RUT ingresado no posee registros en PostgreSQL.
        </div>
      )}

      {data && (
        <div className="space-y-6">
          {/* Encabezado con Supuestos */}
          <div className="bg-white p-4 rounded border flex flex-col sm:flex-row justify-between items-start sm:items-center gap-3 shadow-sm">
            <div>
              <h2 className="text-lg font-bold text-gray-900">{data.nombreEstudiante}</h2>
              <p className="text-sm text-gray-500">RUT: {data.rut} | {data.carrera}</p>
            </div>
            <span className="text-xs bg-purple-50 border border-purple-200 text-purple-800 px-3 py-1 rounded font-medium">
              Supuesto de Proyección / {data.periodoSimulado}
            </span>
          </div>

          {/* PANEL DE PROYECCIÓN DE AVANCE */}
          <div className="bg-white p-6 rounded border shadow-sm space-y-4 border-t-4 border-t-purple-600">
            <h3 className="text-md font-bold text-gray-800 border-b pb-2">
              Proyección de Avance Curricular
            </h3>

            <div className="grid grid-cols-1 sm:grid-cols-3 gap-4">
              <div className="bg-gray-50 p-4 rounded border text-center">
                <span className="text-xs text-gray-500 font-semibold uppercase">Avance Actual</span>
                <p className="text-3xl font-bold text-gray-700 mt-1">{data.proyeccion.porcentajeAvanceActual}%</p>
                <p className="text-xs text-gray-400 mt-1">
                  {data.proyeccion.creditosAprobadosActuales} / {data.proyeccion.creditosTotalesCarrera} Créditos
                </p>
              </div>

              <div className="bg-purple-50 p-4 rounded border border-purple-200 text-center">
                <span className="text-xs text-purple-700 font-semibold uppercase">Proyección Estimada</span>
                <p className="text-3xl font-bold text-purple-700 mt-1">{porcentajeProyectadoCalculado}%</p>
                <p className="text-xs text-purple-600 mt-1">
                  {creditosTotalesProyectados} / {data.proyeccion.creditosTotalesCarrera} Créditos
                </p>
              </div>

              <div className="bg-blue-50 p-4 rounded border border-blue-200 text-center">
                <span className="text-xs text-blue-700 font-semibold uppercase">Semestres Restantes</span>
                <p className="text-3xl font-bold text-blue-700 mt-1">
                  ~{data.proyeccion.semestresRestantesEstimados} Sem.
                </p>
                <p className="text-xs text-blue-600 mt-1">Para Egreso Estimado</p>
              </div>
            </div>
          </div>

          {/* TABLA DE MALLA Y CHECKBOXES */}
          {!data.tieneDatosSuficientes ? (
            <div className="p-4 bg-blue-50 border border-blue-200 text-blue-800 text-sm rounded">
              <strong>Información de Control:</strong> {data.mensajeControl}
            </div>
          ) : (
            <div className="bg-white p-6 rounded border shadow-sm space-y-4">
              <div className="flex flex-col sm:flex-row justify-between items-start sm:items-center gap-3 border-b pb-4">
                <div>
                  <h3 className="text-md font-bold text-gray-800">Malla Curricular y Simulación de Aprobación</h3>
                  <p className="text-xs text-gray-500">
                    Marca las asignaturas habilitadas que proyectas aprobar el próximo semestre para calcular tu nuevo avance.
                  </p>
                </div>
                <div className="flex flex-wrap gap-2">
                  {(['Todas', 'Habilitada', 'Bloqueada', 'Ya Aprobada'] as const).map((estado) => (
                    <button
                      key={estado}
                      onClick={() => setFiltroEstado(estado)}
                      className={`px-3 py-1 rounded-full text-xs font-semibold transition ${
                        filtroEstado === estado
                          ? 'bg-blue-600 text-white'
                          : 'bg-gray-100 text-gray-600 hover:bg-gray-200'
                      }`}
                    >
                      {estado}
                    </button>
                  ))}
                </div>
              </div>

              <div className="overflow-x-auto">
                <table className="w-full text-left border-collapse">
                  <thead>
                    <tr className="border-b text-xs text-gray-500 uppercase bg-gray-50">
                      <th className="py-2 px-3 text-center">Proyectar</th>
                      <th className="py-2 px-3">Semestre</th>
                      <th className="py-2 px-3">Código</th>
                      <th className="py-2 px-3">Asignatura</th>
                      <th className="py-2 px-3 text-center">Créditos</th>
                      <th className="py-2 px-3 text-center">Estado Habilitación</th>
                    </tr>
                  </thead>
                  <tbody className="divide-y text-sm text-gray-700">
                    {asignaturasFiltradas.map((asig: AsignaturaSimulacion, index: number) => {
                      const esHabilitada = asig.estadoHabilitacion === 'Habilitada';
                      const estaSeleccionada = seleccionadasProyeccion.includes(asig.codigo);

                      return (
                        <tr key={index} className="hover:bg-gray-50">
                          <td className="py-2 px-3 text-center">
                            <input
                              type="checkbox"
                              disabled={!esHabilitada}
                              checked={estaSeleccionada}
                              onChange={() => toggleSeleccionAsignatura(asig.codigo)}
                              className="rounded text-purple-600 focus:ring-purple-500 disabled:opacity-40 cursor-pointer"
                            />
                          </td>
                          <td className="py-2 px-3 font-semibold text-xs text-gray-500">{asig.semestre}° Sem.</td>
                          <td className="py-2 px-3 font-mono text-xs">{asig.codigo}</td>
                          <td className="py-2 px-3 font-medium">{asig.nombre}</td>
                          <td className="py-2 px-3 text-center font-semibold">{asig.creditos} SCT</td>
                          <td className="py-2 px-3 text-center">
                            <span
                              className={`px-2.5 py-1 rounded text-xs font-semibold ${
                                asig.estadoHabilitacion === 'Habilitada'
                                  ? 'bg-green-100 text-green-800'
                                  : asig.estadoHabilitacion === 'Bloqueada'
                                  ? 'bg-red-100 text-red-800'
                                  : 'bg-blue-100 text-blue-800'
                              }`}
                            >
                              {asig.estadoHabilitacion}
                            </span>
                          </td>
                        </tr>
                      );
                    })}
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