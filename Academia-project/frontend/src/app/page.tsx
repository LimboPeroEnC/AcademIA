import { redirect } from 'next/navigation';

export default function HomePage() {
  // Redirige automáticamente a la pantalla de inicio de sesión
  redirect('/login');
}