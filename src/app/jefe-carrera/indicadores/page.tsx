'use client';

import { useState, useEffect } from 'react';
import {
  buscarEstudianteCompleto,
  obtenerOpcionesFiltro,
  filtrarIndicadoresBD,
  IndicadorEstudianteBD,
  OpcionesFiltro,
  ResultadoFiltroIndicadores,
  AsignaturaRepetida,
  AsignaturaHistorial,
} from './actions';

export default function IndicadoresPage() {
  const [search, setSearch] = useState('');
  const [data, setData] = useState<IndicadorEstudianteBD | null>(null);
  const [searched, setSearched] = useState(false);
  const [loading, setLoading] = useState(false);
  const [filtroEstado, setFiltroEstado] = useState<'Todas' | 'Aprobada' | 'Reprobada' | 'Pendiente'>('Todas');

  const [opciones, setOpciones] = useState<OpcionesFiltro>({ carreras: [], cursos: [], semestres: [] });
  const [selectedCarrera, setSelectedCarrera] = useState('TODAS');
  const [selectedCurso, setSelectedCurso] = useState('TODOS');
  const [selectedSemestre, setSelectedSemestre] = useState('2026-1');
  const [filtroSoloRezago, setFiltroSoloRezago] = useState(false);
  const [resultadoFiltro, setResultadoFiltro] = useState<ResultadoFiltroIndicadores | null>(null);
  const [loadingFiltro, setLoadingFiltro] = useState(false);

  useEffect(() => {
    async function cargarOpciones() {
      const res = await obtenerOpcionesFiltro();
      setOpciones(res);
    }
    cargarOpciones();
  }, []);

  const handleSearch = async (e: React.FormEvent) => {
    e.preventDefault();
    if (!search.trim()) return;

    setLoading(true);
    setSearched(true);
    setResultadoFiltro(null);

    const resultado = await buscarEstudianteCompleto(search);
    setData(resultado);
    setLoading(false);
  };

  const handleAplicarFiltros = async () => {
    setLoadingFiltro(true);
    setData(null);
    setSearched(false);

    const res = await filtrarIndicadoresBD(selectedCarrera, selectedCurso, selectedSemestre, filtroSoloRezago);
    setResultadoFiltro(res);
    setLoadingFiltro(false);
  };

  const asignaturasFiltradas: AsignaturaHistorial[] = data?.asignaturasHistorial
    ? data.asignaturasHistorial.filter((asig: AsignaturaHistorial) => filtroEstado === 'Todas' || asig.estado === filtroEstado)
    : [];

  return (
    <div className="p-6 max-w-5xl mx-auto space-y-6">
      <header>
        <h1 className="text-2xl font-bold text-gray-800">Indicadores Académicos y Seguimiento</h1>
        <p className="text-sm text-gray-500">Módulo de Jefe de Carrera</p>
      </header>

      {/* PANEL: FILTROS E INDICADORES GLOBALES POR CARRERA Y REZAGO */}
      <div className="bg-white p-4 rounded border shadow-sm space-y-3">
        <h3 className="text-sm font-bold text-gray-700 uppercase tracking-wide">Filtros de Carrera, Cohorte y Rezago</h3>
        <div className="grid grid-cols-1 sm:grid-cols-3 gap-3">
          <div>
            <label className="block text-xs font-semibold text-gray-500 mb-1">Carrera</label>
            <select
              value={selectedCarrera}
              onChange={(e) => setSelectedCarrera(e.target.value)}
              className="w-full px-3 py-2 border rounded text-sm bg-white focus:outline-none"
            >
              <option value="TODAS">Todas las Carreras</option>
              {opciones.carreras.map((c) => (
                <option key={c.id} value={c.id}>
                  {c.nombre}
                </option>
              ))}
            </select>
          </div>

          <div>
            <label className="block text-xs font-semibold text-gray-500 mb-1">Curso / Asignatura</label>
            <select
              value={selectedCurso}
              onChange={(e) => setSelectedCurso(e.target.value)}
              className="w-full px-3 py-2 border rounded text-sm bg-white focus:outline-none"
            >
              <option value="TODOS">Todos los Cursos</option>
              {opciones.cursos.map((c) => (
                <option key={c.id} value={c.id}>
                  {c.nombre}
                </option>
              ))}
            </select>
          </div>

          <div>
            <label className="block text-xs font-semibold text-gray-500 mb-1">Semestre / Período</label>
            <select
              value={selectedSemestre}
              onChange={(e) => setSelectedSemestre(e.target.value)}
              className="w-full px-3 py-2 border rounded text-sm bg-white focus:outline-none"
            >
              {opciones.semestres.map((s) => (
                <option key={s} value={s}>
                  {s}
                </option>
              ))}
            </select>
          </div>
        </div>

        <div className="flex flex-col sm:flex-row justify-between items-center pt-2 gap-3 border-t">
          <label className="flex items-center gap-2 cursor-pointer text-xs font-semibold text-orange-700 bg-orange-50 px-3 py-1.5 rounded border border-orange-200">
            <input
              type="checkbox"
              checked={filtroSoloRezago}
              onChange={(e) => setFiltroSoloRezago(e.target.checked)}
              className="rounded text-orange-600 focus:ring-orange-500"
            />
            Filtrar únicamente estudiantes con Rezago Curricular
          </label>

          <button
            onClick={handleAplicarFiltros}
            disabled={loadingFiltro}
            className="w-full sm:w-auto px-4 py-2 bg-gray-800 text-white rounded text-sm hover:bg-gray-900 transition"
          >
            {loadingFiltro ? 'Consultando BD...' : 'Aplicar Filtros'}
          </button>
        </div>
      </div>

      {/* RESULTADOS DE INDICADORES GLOBALES Y REZAGO */}
      {resultadoFiltro && (
        <div className="space-y-6">
          {resultadoFiltro.mensajeEstado ? (
            <div className="p-4 bg-yellow-50 border border-yellow-200 text-yellow-800 text-sm rounded">
              {resultadoFiltro.mensajeEstado}
            </div>
          ) : (
            <>
              {/* TARJETAS DE METRICAS GLOBALES */}
              <div className="grid grid-cols-1 sm:grid-cols-4 gap-4">
                <div className="bg-white p-4 rounded border shadow-sm">
                  <span className="text-xs text-gray-400 font-semibold uppercase">Matrícula Total</span>
                  <p className="text-2xl font-bold text-gray-800 mt-1">
                    {resultadoFiltro.indicadoresGlobales.totalEstudiantes} Alumnos
                  </p>
                </div>
                <div className="bg-white p-4 rounded border shadow-sm">
                  <span className="text-xs text-gray-400 font-semibold uppercase">Promedio Carrera</span>
                  <p className="text-2xl font-bold text-blue-600 mt-1">
                    {resultadoFiltro.indicadoresGlobales.promedioGeneralCarrera}
                  </p>
                </div>
                <div className="bg-white p-4 rounded border shadow-sm">
                  <span className="text-xs text-gray-400 font-semibold uppercase">Estudiantes en Rezago</span>
                  <p className="text-2xl font-bold text-orange-600 mt-1">
                    {resultadoFiltro.indicadoresGlobales.estudiantesConRezago} Alumnos
                  </p>
                </div>
                <div className="bg-white p-4 rounded border shadow-sm">
                  <span className="text-xs text-gray-400 font-semibold uppercase">Alumnos en Riesgo</span>
                  <p className="text-2xl font-bold text-red-600 mt-1">
                    {resultadoFiltro.indicadoresGlobales.estudiantesEnRiesgo}
                  </p>
                </div>
              </div>

              {/* TABLA DE ESTUDIANTES CON COLUMNA DE REZAGO */}
              <div className="bg-white p-6 rounded border shadow-sm space-y-4">
                <div className="flex justify-between items-center border-b pb-3">
                  <h3 className="text-md font-bold text-gray-800">
                    Detalle de la Cohorte ({resultadoFiltro.totalRegistros} estudiantes)
                  </h3>
                  <span className="text-xs bg-blue-50 border border-blue-200 text-blue-800 px-2.5 py-1 rounded">
                    Período: {selectedSemestre}
                  </span>
                </div>

                <div className="overflow-x-auto">
                  <table className="w-full text-left border-collapse">
                    <thead>
                      <tr className="border-b text-xs text-gray-500 uppercase bg-gray-50">
                        <th className="py-2 px-3">RUT</th>
                        <th className="py-2 px-3">Estudiante</th>
                        <th className="py-2 px-3">Carrera</th>
                        <th className="py-2 px-3 text-center">Avance</th>
                        <th className="py-2 px-3 text-center">Rezago</th>
                        <th className="py-2 px-3 text-center">Estado</th>
                      </tr>
                    </thead>
                    <tbody className="divide-y text-sm text-gray-700">
                      {resultadoFiltro.estudiantes.map((est) => (
                        <tr key={est.rut} className="hover:bg-gray-50">
                          <td className="py-2 px-3 font-mono text-xs">{est.rut}</td>
                          <td className="py-2 px-3 font-medium">{est.nombreCompleto}</td>
                          <td className="py-2 px-3 text-xs text-gray-500">{est.carrera}</td>
                          <td className="py-2 px-3 text-center font-semibold text-purple-600">{est.porcentajeAvance}%</td>
                          <td className="py-2 px-3 text-center">
                            <span
                              className={`px-2 py-0.5 rounded text-xs font-semibold ${
                                est.tieneRezago ? 'bg-orange-100 text-orange-800' : 'bg-green-100 text-green-800'
                              }`}
                            >
                              {est.tieneRezago ? 'Con Rezago' : 'Al día'}
                            </span>
                          </td>
                          <td className="py-2 px-3 text-center">
                            <span className="px-2 py-0.5 rounded text-xs font-semibold bg-green-100 text-green-800">
                              {est.estadoAcademico}
                            </span>
                          </td>
                        </tr>
                      ))}
                    </tbody>
                  </table>
                </div>
              </div>
            </>
          )}
        </div>
      )}

      {/* BÚSQUEDA INDIVIDUAL POR RUT */}
      <form onSubmit={handleSearch} className="flex gap-3 bg-white p-4 rounded border shadow-sm">
        <input
          type="text"
          placeholder="O buscar individualmente por RUT (ej: 11111111-1) o Nombre..."
          value={search}
          onChange={(e) => setSearch(e.target.value)}
          className="flex-1 px-3 py-2 border rounded focus:outline-none text-sm"
        />
        <button
          type="submit"
          disabled={loading}
          className="px-5 py-2 bg-blue-600 text-white rounded text-sm hover:bg-blue-700 disabled:bg-blue-300 transition"
        >
          {loading ? 'Consultando BD...' : 'Buscar'}
        </button>
      </form>

      {/* Sin datos Búsqueda Individual */}
      {searched && !loading && !data && (
        <div className="p-4 bg-yellow-50 border border-yellow-200 text-yellow-800 rounded text-sm">
          El estudiante ingresado no se encuentra registrado en PostgreSQL.
        </div>
      )}

      {/* FICHA INDIVIDUAL CON ALERTA DE REZAGO */}
      {data && (
        <div className="space-y-6">
          <div className="bg-white p-4 rounded border flex justify-between items-center shadow-sm">
            <div>
              <h2 className="text-lg font-bold text-gray-900">{data.nombreCompleto}</h2>
              <p className="text-sm text-gray-500">RUT: {data.rut} | {data.carrera}</p>
            </div>
            <div className="flex gap-2">
              <span
                className={`px-3 py-1 rounded-full text-xs font-semibold ${
                  data.tieneRezago ? 'bg-orange-100 text-orange-800' : 'bg-green-100 text-green-800'
                }`}
              >
                {data.tieneRezago ? 'Con Rezago Curricular' : 'Avance Normal'}
              </span>
              <span className="px-3 py-1 rounded-full text-xs font-semibold bg-green-100 text-green-800">
                {data.estadoAcademico}
              </span>
            </div>
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
              <p className="text-3xl font-bold text-purple-600 mt-1">{data.porcentajeAvance}%</p>
            </div>
          </div>

          <div className="bg-white p-6 rounded border shadow-sm space-y-4">
            <div className="flex justify-between items-center border-b pb-3">
              <div>
                <h3 className="text-md font-bold text-gray-800">Asignaturas Repetidas y Críticas</h3>
                <p className="text-xs text-gray-500">
                  Regla: Asignaturas con ≥ 2 inscripciones. Crítica si es ≥ 3 (última oportunidad).
                </p>
              </div>
              <span className="text-xs bg-gray-100 border px-2.5 py-1 rounded text-gray-600 font-medium">
                Período: {data.periodoReferencia}
              </span>
            </div>

            {!data.tieneHistorialSuficiente ? (
              <div className="p-4 bg-blue-50 border border-blue-200 text-blue-800 text-sm rounded">
                <strong>Información de Control:</strong> No existen suficientes registros de asignaturas para evaluar repeticiones en la base de datos.
              </div>
            ) : data.asignaturasRepetidas.length > 0 ? (
              <table className="w-full text-left border-collapse">
                <thead>
                  <tr className="border-b text-xs text-gray-500 uppercase bg-gray-50">
                    <th className="py-2 px-3">Código</th>
                    <th className="py-2 px-3">Asignatura</th>
                    <th className="py-2 px-3 text-center">N° Oportunidad / Cursadas</th>
                    <th className="py-2 px-3 text-center">Clasificación</th>
                  </tr>
                </thead>
                <tbody className="divide-y text-sm text-gray-700">
                  {data.asignaturasRepetidas.map((item: AsignaturaRepetida, idx: number) => (
                    <tr key={idx}>
                      <td className="py-2 px-3 font-mono text-xs">{item.codigo}</td>
                      <td className="py-2 px-3 font-medium">{item.nombre}</td>
                      <td className="py-2 px-3 text-center font-bold text-red-600">{item.vecesInscrita}ª Vez</td>
                      <td className="py-2 px-3 text-center">
                        <span
                          className={`px-2 py-0.5 rounded text-xs font-semibold ${
                            item.esCritica ? 'bg-red-200 text-red-900' : 'bg-yellow-100 text-yellow-800'
                          }`}
                        >
                          {item.esCritica ? 'Crítica (3ª Oportunidad)' : 'Repetida (2ª Oportunidad)'}
                        </span>
                      </td>
                    </tr>
                  ))}
                </tbody>
              </table>
            ) : (
              <p className="text-sm text-gray-600 py-2">
                El estudiante no registra asignaturas repetidas o en condición crítica.
              </p>
            )}
          </div>

          <div className="bg-white p-6 rounded border shadow-sm space-y-4">
            <div className="flex flex-col sm:flex-row sm:items-center justify-between gap-4 border-b pb-4">
              <h3 className="text-md font-bold text-gray-800">Historial de Asignaturas</h3>
              <div className="flex flex-wrap gap-2">
                {(['Todas', 'Aprobada', 'Reprobada', 'Pendiente'] as const).map((estado) => (
                  <button
                    key={estado}
                    onClick={() => setFiltroEstado(estado)}
                    className={`px-3 py-1 rounded-full text-xs font-semibold transition ${
                      filtroEstado === estado
                        ? 'bg-blue-600 text-white'
                        : 'bg-gray-100 text-gray-600 hover:bg-gray-200'
                    }`}
                  >
                    {estado === 'Todas' ? 'Todas' : `${estado}s`}
                  </button>
                ))}
              </div>
            </div>

            {data.asignaturasHistorial.length > 0 ? (
              <div className="overflow-x-auto">
                <table className="w-full text-left border-collapse">
                  <thead>
                    <tr className="border-b text-xs text-gray-500 uppercase bg-gray-50">
                      <th className="py-2 px-3">Código</th>
                      <th className="py-2 px-3">Asignatura</th>
                      <th className="py-2 px-3">Semestre</th>
                      <th className="py-2 px-3 text-center">Créditos</th>
                      <th className="py-2 px-3 text-center">Nota Final</th>
                      <th className="py-2 px-3 text-center">Estado</th>
                    </tr>
                  </thead>
                  <tbody className="divide-y text-sm text-gray-700">
                    {asignaturasFiltradas.map((asig: AsignaturaHistorial, index: number) => (
                      <tr key={index} className="hover:bg-gray-50">
                        <td className="py-2 px-3 font-mono text-xs">{asig.codigo}</td>
                        <td className="py-2 px-3 font-medium">{asig.nombre}</td>
                        <td className="py-2 px-3 text-gray-500">{asig.semestre}</td>
                        <td className="py-2 px-3 text-center">{asig.creditos}</td>
                        <td className="py-2 px-3 text-center font-semibold">
                          {asig.notaFinal ? asig.notaFinal : '-'}
                        </td>
                        <td className="py-2 px-3 text-center">
                          <span
                            className={`px-2 py-0.5 rounded text-xs font-semibold ${
                              asig.estado === 'Aprobada'
                                ? 'bg-green-100 text-green-800'
                                : asig.estado === 'Reprobada'
                                ? 'bg-red-100 text-red-800'
                                : 'bg-yellow-100 text-yellow-800'
                            }`}
                          >
                            {asig.estado}
                          </span>
                        </td>
                      </tr>
                    ))}
                  </tbody>
                </table>
              </div>
            ) : (
              <p className="text-sm text-gray-500 py-4 text-center">
                El estudiante no registra historial de asignaturas cursadas en la base de datos por el momento.
              </p>
            )}
          </div>
        </div>
      )}
    </div>
  );
}