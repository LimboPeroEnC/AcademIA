'use client';

import React, { useState } from 'react';

export default function EstudianteDashboard() {
  const [mensajeIa, setMensajeIa] = useState('');
  const [respuestaIa, setRespuestaIa] = useState<string | null>(null);

  const handleConsultarIA = (e: React.FormEvent) => {
    e.preventDefault();
    if (!mensajeIa.trim()) return;
    // Simulación de respuesta mientras conectan el backend con LLM
    setRespuestaIa(
      `Basado en tus calificaciones en Cálculo y Estructura de Datos, te recomiendo reforzar los conceptos de derivadas parciales y árboles binarios. Tienes un 65% de riesgo en la próxima evaluación.`
    );
  };

  return (
    <div className="p-8 max-w-7xl mx-auto space-y-8">
      {/* Encabezado */}
      <div>
        <h1 className="text-3xl font-bold text-gray-800">
          Portal del Estudiante - AcademIA
        </h1>
        <p className="text-gray-500 mt-1">
          Seguimiento académico y predicción de rendimiento personalizado
        </p>
      </div>

      {/* Tarjeta Alerta de Riesgo (Modelo Predictivo ML) */}
      <div className="bg-amber-50 border-l-4 border-amber-500 p-6 rounded-r-lg shadow-sm">
        <div className="flex items-center justify-between">
          <div>
            <span className="text-xs font-bold uppercase tracking-wide text-amber-800">
              Estado de Riesgo Académico (Predicción XGBoost)
            </span>
            <h3 className="text-xl font-bold text-amber-900 mt-1">
              Riesgo Moderado en Cálculo Vectorial
            </h3>
            <p className="text-sm text-amber-700 mt-1">
              Tu asistencia actual (72%) y nota parcial (3.8) indican probabilidad de reprobación si no mejoras en el certamen 2.
            </p>
          </div>
          <span className="px-4 py-2 bg-amber-200 text-amber-900 font-bold rounded-full text-sm">
            Riesgo: 58%
          </span>
        </div>
      </div>

      {/* Resumen de Ramos en Curso */}
      <div className="bg-white rounded-xl shadow p-6 border border-gray-200">
        <h2 className="text-xl font-bold text-gray-800 mb-4">Mis Asignaturas Actuales</h2>
        <div className="grid grid-cols-1 md:grid-cols-3 gap-4">
          <div className="p-4 border rounded-lg bg-gray-50">
            <h4 className="font-bold text-gray-800">Sistemas Operativos</h4>
            <p className="text-sm text-gray-600 mt-1">Promedio: <span className="font-bold text-green-600">5.8</span></p>
            <p className="text-xs text-gray-500">Asistencia: 90%</p>
          </div>

          <div className="p-4 border rounded-lg bg-gray-50">
            <h4 className="font-bold text-gray-800">Cálculo Vectorial</h4>
            <p className="text-sm text-gray-600 mt-1">Promedio: <span className="font-bold text-red-600">3.8</span></p>
            <p className="text-xs text-gray-500">Asistencia: 72%</p>
          </div>

          <div className="p-4 border rounded-lg bg-gray-50">
            <h4 className="font-bold text-gray-800">Bases de Datos</h4>
            <p className="text-sm text-gray-600 mt-1">Promedio: <span className="font-bold text-green-600">5.2</span></p>
            <p className="text-xs text-gray-500">Asistencia: 85%</p>
          </div>
        </div>
      </div>

      {/* Asistente Virtual AcademIA (Consulta LLM) */}
      <div className="bg-white rounded-xl shadow p-6 border border-gray-200">
        <h2 className="text-xl font-bold text-gray-800 mb-2">
          Asistente de Recomendación Académica (IA)
        </h2>
        <p className="text-sm text-gray-500 mb-4">
          Haz preguntas sobre tus asignaturas o pide recomendaciones de estudio.
        </p>

        <form onSubmit={handleConsultarIA} className="flex gap-2">
          <input
            type="text"
            className="flex-1 border rounded-lg px-4 py-2 text-sm focus:outline-none focus:ring-2 focus:ring-blue-500"
            placeholder="Ej: ¿Qué temas debo estudiar para mejorar en Cálculo?"
            value={mensajeIa}
            onChange={(e) => setMensajeIa(e.target.value)}
          />
          <button
            type="submit"
            className="bg-blue-600 text-white px-6 py-2 rounded-lg font-medium hover:bg-blue-700 text-sm"
          >
            Consultar
          </button>
        </form>

        {respuestaIa && (
          <div className="mt-4 p-4 bg-blue-50 border border-blue-200 rounded-lg text-sm text-blue-900">
            <strong>Recomendación de AcademIA:</strong> {respuestaIa}
          </div>
        )}
      </div>
    </div>
  );
}