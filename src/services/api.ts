import axios from 'axios';
import { IndicadorEstudiante } from '@/types/indicadores';

const api = axios.create({
  baseURL: process.env.NEXT_PUBLIC_API_URL || 'http://localhost:8000/api',
  headers: {
    'Content-Type': 'application/json',
  },
});

api.interceptors.request.use(
  (config) => {
    if (typeof window !== 'undefined') {
      const token = localStorage.getItem('token');
      if (token && config.headers) {
        config.headers.Authorization = `Bearer ${token}`;
      }
    }
    return config;
  },
  (error) => Promise.reject(error)
);

api.interceptors.response.use(
  (response) => response,
  (error) => {
    if (error.response) {
      const status = error.response.status;
      if (status >= 400) {
        console.error('Ocurrió un error técnico:', error.response.data);
      }
      if (status === 401 && typeof window !== 'undefined') {
        localStorage.removeItem('token');
        window.location.href = '/login';
      }
    }
    return Promise.reject(error);
  }
);

export async function getIndicadoresEstudiante(search: string): Promise<IndicadorEstudiante | null> {
  try {
    const response = await api.get<IndicadorEstudiante>(`/indicadores/estudiante`, {
      params: { q: search },
    });
    return response.data;
  } catch (error) {
    console.error('Error al obtener indicadores del estudiante:', error);
    return null;
  }
}

export default api;