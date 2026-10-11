'use client';

// Mock Data: Información consolidada que el Backend (FastAPI + XGBoost) enviará en el futuro
const kpis = {
  totalEstudiantes: 450,
  estudiantesRiesgo: 32,
  promedioGeneral: 5.2,
  porcentajeAprobacion: 78,
};

const alertasIA = [
  { id: 1, estudiante: 'Felipe Ignacio Tapia Silva', rut: '20.555.666-7', probabilidad: '85%', motivo: 'Bajo rendimiento histórico y 2 reprobaciones previas en Cálculo I', severidad: 'Alta' },
  { id: 2, estudiante: 'María Ignacia López Vega', rut: '22.333.444-5', probabilidad: '60%', motivo: 'Retraso en malla curricular por falta de prerrequisitos', severidad: 'Media' },
  { id: 3, estudiante: 'Carlos Andrés Soto Pérez', rut: '21.111.222-3', probabilidad: '45%', motivo: 'Asistencia irregular (menor al 70%) en el semestre actual', severidad: 'Baja' },
];

const asignaturasCriticas = [
  { codigo: 'MAT-102', nombre: 'Cálculo I', reprobacion: 42 },
  { codigo: 'ICC-201', nombre: 'Estructura de Datos', reprobacion: 35 },
  { codigo: 'FIS-101', nombre: 'Física Mecánica', reprobacion: 28 },
];

export default function DashboardPage() {
  return (
    <div className="space-y-6">
      {/* Encabezado */}
      <div>
        <h1 className="text-2xl font-bold text-gray-800">Dashboard Académico</h1>
        <p className="text-sm text-gray-500 mt-1">
          Resumen general del estado académico y alertas predictivas generadas por AcademIA.
        </p>
      </div>

      {/* Tarjetas de Indicadores Clave (KPIs) */}
      <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-4">
        <div className="bg-white p-6 rounded-lg shadow border-l-4 border-blue-500">
          <p className="text-sm text-gray-500 font-medium">Total Estudiantes</p>
          <p className="text-3xl font-bold text-gray-800 mt-2">{kpis.totalEstudiantes}</p>
        </div>
        
        <div className="bg-white p-6 rounded-lg shadow border-l-4 border-red-500">
          <p className="text-sm text-gray-500 font-medium">Estudiantes en Riesgo (IA)</p>
          <p className="text-3xl font-bold text-red-600 mt-2">{kpis.estudiantesRiesgo}</p>
        </div>
        
        <div className="bg-white p-6 rounded-lg shadow border-l-4 border-green-500">
          <p className="text-sm text-gray-500 font-medium">Promedio General</p>
          <p className="text-3xl font-bold text-gray-800 mt-2">{kpis.promedioGeneral}</p>
        </div>
        
        <div className="bg-white p-6 rounded-lg shadow border-l-4 border-purple-500">
          <p className="text-sm text-gray-500 font-medium">Tasa de Aprobación</p>
          <div className="mt-2 flex items-center">
            <p className="text-3xl font-bold text-gray-800 mr-3">{kpis.porcentajeAprobacion}%</p>
            <div className="w-full bg-gray-200 rounded-full h-2.5">
              <div className="bg-purple-600 h-2.5 rounded-full" style={{ width: `${kpis.porcentajeAprobacion}%` }}></div>
            </div>
          </div>
        </div>
      </div>

      {/* Panel Inferior: Alertas IA y Estadísticas */}
      <div className="grid grid-cols-1 lg:grid-cols-3 gap-6">
        
        {/* Columna Izquierda: Alertas Predictivas (2/3 del espacio) */}
        <div className="lg:col-span-2 bg-white rounded-lg shadow border border-gray-200">
          <div className="p-4 border-b border-gray-200 bg-gray-50 flex justify-between items-center">
            <h2 className="text-lg font-bold text-gray-800 flex items-center">
              <span className="mr-2">🤖</span> Alertas de Riesgo Académico (Modelo Predictivo)
            </h2>
          </div>
          <div className="p-0">
            <table className="w-full text-left border-collapse">
              <thead>
                <tr className="bg-gray-50 text-gray-500 text-xs uppercase">
                  <th className="p-4 font-medium">Estudiante</th>
                  <th className="p-4 font-medium text-center">Probabilidad de Riesgo</th>
                  <th className="p-4 font-medium">Análisis de la IA</th>
                </tr>
              </thead>
              <tbody className="divide-y divide-gray-100">
                {alertasIA.map((alerta) => (
                  <tr key={alerta.id} className="hover:bg-gray-50 transition-colors">
                    <td className="p-4">
                      <p className="font-semibold text-sm text-gray-800">{alerta.estudiante}</p>
                      <p className="text-xs text-gray-500">{alerta.rut}</p>
                    </td>
                    <td className="p-4 text-center">
                      <span className={`px-3 py-1 rounded-full text-xs font-bold inline-block w-16 text-center ${
                        alerta.severidad === 'Alta' ? 'bg-red-100 text-red-700' :
                        alerta.severidad === 'Media' ? 'bg-orange-100 text-orange-700' :
                        'bg-yellow-100 text-yellow-700'
                      }`}>
                        {alerta.probabilidad}
                      </span>
                    </td>
                    <td className="p-4 text-sm text-gray-600">
                      {alerta.motivo}
                    </td>
                  </tr>
                ))}
              </tbody>
            </table>
          </div>
          <div className="p-4 border-t border-gray-100 text-center">
            <button className="text-blue-600 hover:text-blue-800 text-sm font-medium transition-colors">
              Ver reporte completo de predicciones ➔
            </button>
          </div>
        </div>

        {/* Columna Derecha: Asignaturas Críticas (1/3 del espacio) */}
        <div className="bg-white rounded-lg shadow border border-gray-200 flex flex-col">
          <div className="p-4 border-b border-gray-200 bg-gray-50">
            <h2 className="text-lg font-bold text-gray-800">📉 Asignaturas Críticas</h2>
            <p className="text-xs text-gray-500 mt-1">Mayor tasa de reprobación histórica</p>
          </div>
          <div className="p-4 flex-1">
            <div className="space-y-5">
              {asignaturasCriticas.map((asig, index) => (
                <div key={index}>
                  <div className="flex justify-between items-center mb-1">
                    <div>
                      <p className="text-sm font-bold text-gray-800">{asig.codigo}</p>
                      <p className="text-xs text-gray-500 truncate max-w-[150px]">{asig.nombre}</p>
                    </div>
                    <span className="text-sm font-bold text-red-600">{asig.reprobacion}%</span>
                  </div>
                  {/* Barra visual de reprobación */}
                  <div className="w-full bg-gray-100 rounded-full h-2">
                    <div className="bg-red-500 h-2 rounded-full" style={{ width: `${asig.reprobacion}%` }}></div>
                  </div>
                </div>
              ))}
            </div>
          </div>
        </div>
        
      </div>
    </div>
  );
}