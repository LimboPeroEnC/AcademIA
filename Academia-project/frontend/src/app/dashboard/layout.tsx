'use client';

import { useState, useEffect } from 'react';
import Link from 'next/link';
import { useRouter } from 'next/navigation';

export default function DashboardLayout({ children }: { children: React.ReactNode }) {
  const router = useRouter();
  const [isMobileMenuOpen, setIsMobileMenuOpen] = useState(false);
  const [userRole, setUserRole] = useState<string | null>(null);

  useEffect(() => {
    setUserRole('Administrador'); // Rol simulado
  }, []);

  const handleLogout = () => {
    const confirmar = window.confirm('¿Está seguro que desea cerrar su sesión?');
    if (confirmar) {
      localStorage.removeItem('token');
      router.push('/login');
    }
  };

  const menuItems = [
    { title: 'Dashboard Académico', path: '/dashboard' },
    { title: 'Gestión de Usuarios', path: '/dashboard/usuarios', roles: ['Administrador'] },
    { title: 'Gestión de Estudiantes', path: '/dashboard/estudiantes', roles: ['Administrador', 'Académico'] },
    { title: 'Gestión de Ramos', path: '/dashboard/ramos', roles: ['Administrador', 'Académico'] },
  ];

  return (
    <div className="flex h-screen bg-gray-50">
      <aside className={`${isMobileMenuOpen ? 'block' : 'hidden'} md:flex flex-col w-64 bg-blue-900 text-white absolute md:relative z-10 h-full`}>
        <div className="p-5 text-2xl font-bold border-b border-blue-800 flex justify-between items-center">
          <span>AcademIA</span>
          <button className="md:hidden text-white" onClick={() => setIsMobileMenuOpen(false)}>✕</button>
        </div>
        <nav className="flex-1 p-4 space-y-2 overflow-y-auto">
          {menuItems.map((item) => (
            (!item.roles || item.roles.includes(userRole || '')) && (
              <Link key={item.path} href={item.path} className="block p-3 rounded hover:bg-blue-800 transition-colors">
                {item.title}
              </Link>
            )
          ))}
        </nav>
        <div className="p-4 border-t border-blue-800">
          <button onClick={handleLogout} className="w-full bg-red-600 hover:bg-red-700 text-white p-2 rounded transition-colors">
            Cerrar Sesión
          </button>
        </div>
      </aside>

      <main className="flex-1 flex flex-col overflow-hidden">
        <header className="bg-white shadow-sm p-4 flex justify-between items-center md:hidden">
          <span className="text-xl font-bold text-blue-900">AcademIA</span>
          <button onClick={() => setIsMobileMenuOpen(true)} className="p-2 text-2xl">☰</button>
        </header>
        <div className="flex-1 overflow-y-auto p-6 text-gray-800">
          {children}
        </div>
      </main>
    </div>
  );
}