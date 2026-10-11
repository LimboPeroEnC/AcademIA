'use client';

import { useState } from 'react';
import { useForm } from 'react-hook-form';
import { zodResolver } from '@hookform/resolvers/zod';
import * as z from 'zod';
import { useRouter } from 'next/navigation'; 

// Esquema de validación estricta
const loginSchema = z.object({
  identificador: z.string().min(8, { message: 'Ingresa un RUT o correo válido' }),
  password: z.string().min(1, { message: 'La contraseña es obligatoria' }),
});

type LoginFormValues = z.infer<typeof loginSchema>;

export default function LoginPage() {
  const router = useRouter();
  const [globalError, setGlobalError] = useState<string | null>(null);

  const { register, handleSubmit, formState: { errors, isSubmitting } } = useForm<LoginFormValues>({
    resolver: zodResolver(loginSchema),
  });

  const onSubmit = async (data: LoginFormValues) => {
    setGlobalError(null);
    try {
      // Simulación de retraso de red
      await new Promise((resolve) => setTimeout(resolve, 1500));

      // Simulamos credenciales válidas para probar
      if (data.identificador === 'admin@academia.com' && data.password === '123456') {
        localStorage.setItem('token', 'token_simulado_123');
        router.push('/dashboard');
      } else {
        throw new Error('Credenciales incorrectas. (Prueba con admin@test.com / 123456)');
      }
    } catch (error: any) {
      setGlobalError(error.message || 'Ocurrió un error técnico');
    }
  };

  return (
    <div className="min-h-screen flex items-center justify-center bg-gray-100 p-4">
      <div className="bg-white p-8 rounded-lg shadow-md w-full max-w-md">
        <h2 className="text-2xl font-bold text-center mb-6 text-gray-800">AcademIA - Iniciar Sesión</h2>
        
        {globalError && (
          <div className="mb-4 p-3 bg-red-100 text-red-700 rounded text-sm text-center">
            {globalError}
          </div>
        )}

        <form onSubmit={handleSubmit(onSubmit)} className="space-y-4">
          <div>
            <label className="block text-sm font-medium text-gray-700 mb-1">Correo Institucional o RUT</label>
            <input
              type="text"
              {...register('identificador')}
              className={`w-full p-2 border rounded focus:outline-none focus:ring-2 focus:ring-blue-500 ${
                errors.identificador ? 'border-red-500' : 'border-gray-300'
              }`}
              placeholder="admin@test.com"
            />
            {errors.identificador && <p className="text-red-500 text-xs mt-1">{errors.identificador.message}</p>}
          </div>

          <div>
            <label className="block text-sm font-medium text-gray-700 mb-1">Contraseña</label>
            <input
              type="password"
              {...register('password')}
              className={`w-full p-2 border rounded focus:outline-none focus:ring-2 focus:ring-blue-500 ${
                errors.password ? 'border-red-500' : 'border-gray-300'
              }`}
              placeholder="******"
            />
            {errors.password && <p className="text-red-500 text-xs mt-1">{errors.password.message}</p>}
          </div>

          <button
            type="submit"
            disabled={isSubmitting}
            className="w-full bg-blue-900 text-white p-2 rounded hover:bg-blue-800 transition-colors disabled:bg-blue-400 flex justify-center items-center"
          >
            {isSubmitting ? 'Verificando...' : 'Ingresar'}
          </button>
        </form>
      </div>
    </div>
  );
}