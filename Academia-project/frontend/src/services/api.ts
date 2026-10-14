import axios from 'axios';

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

          if (window.location.pathname !== '/login') {
              window.location.href = '/login';
          }
        }
    }
    return Promise.reject(error);
  }
);

export default api;