# Gestión de Estudiantes — C++ + PostgreSQL + frontend HTML/CSS/JS

Se incluyen **versiones adaptadas de los 4 archivos que compartiste**:

- `backend/modelos/Estudiante.hpp` y `.cpp`
- `backend/gestores/GestorEstudiantes.hpp` y `.cpp`

Se agregan una clase `RepositorioEstudiantes` (consultas SQL), `ConexionPostgreSQL` (libpq), un servidor HTTP local (Boost.Beast), validación de RUT de referencia, y frontend que llama a la API con `fetch`.

**Importante:** este paquete es un módulo *independiente*. No conocemos el resto del repositorio, de modo que debes integrar los cambios a las carpetas, clases existentes, CMake y `main.cpp` en vez de reemplazar todo sin comparar. En particular, si ya existe `ValidadorRut`, usa el tuyo o adapta sus métodos. No agregues un segundo `main` al mismo ejecutable: usa otro target de CMake.

## Estructura a copiar a la raíz del repositorio

```
MiRepositorio/
  backend/
    modelos/Estudiante.hpp, Estudiante.cpp
    gestores/GestorEstudiantes.hpp, GestorEstudiantes.cpp
    repositorios/RepositorioEstudiantes.hpp, RepositorioEstudiantes.cpp
    database/ConexionPostgreSQL.hpp, ConexionPostgreSQL.cpp
    util/ValidadorRut.hpp, ValidadorRut.cpp
    api/Servidor.cpp, BoostJson.cpp
  frontend/
    index.html, registrar.html, buscar.html, modificar.html, historial.html
    css/styles.css
    js/estudiantes.js
  sql/historial_estudiantes.sql
  CMakeLists.txt
  .env.example
  iniciar_windows.cmd
```

`frontend/` va al **nivel raíz del repositorio**, junto a `backend/`, no dentro de `backend/` ni dentro de `build/`.

## 1. Preparar la base de datos

En pgAdmin, selecciona **DBAcademiaUltimate** y ejecuta en Query Tool el archivo `sql/historial_estudiantes.sql`. El respaldo inicial no tiene la tabla de historial. Esta migración solo registra **cambios de estado nuevos**, no el historial académico de notas o inscripciones.

## 2. Dependencias en Windows (opción recomendada)

Instala Visual Studio Build Tools 2022 con el componente *Desarrollo de escritorio con C++*, CMake y Git. Para instalar las dependencias con vcpkg:

```cmd
git clone https://github.com/microsoft/vcpkg C:\vcpkg
C:\vcpkg\bootstrap-vcpkg.bat
C:\vcpkg\vcpkg.exe install boost-beast boost-asio boost-json libpq --triplet x64-windows
```

Luego, desde la raíz del proyecto, ejecuta:

```cmd
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build --config Release
```

Si tu repositorio ya tiene `CMakeLists.txt`, integra las fuentes y dependencias en ese archivo en lugar de reemplazarlo.

## 3. Configurar PostgreSQL

Desde CMD en la raíz:

```cmd
copy .env.example .env
notepad .env
```

Ejemplo de contenido, completa tus datos reales:

```
DB_HOST=127.0.0.1
DB_PORT=5432
DB_NAME=DBAcademiaUltimate
DB_USER=postgres
DB_PASSWORD=TU_CONTRASENA_POSTGRESQL
API_PORT=8080
```

Como tienes PostgreSQL 16 y 18 instalados, verifica en pgAdmin el puerto de **PostgreSQL 18**, podría ser distinto de `5432`. **No subas `.env` a GitHub.**

## 4. Iniciar y probar

```cmd
iniciar_windows.cmd
```

Abre estas direcciones (el servidor solo escucha en 127.0.0.1):

- http://127.0.0.1:8080/ — página principal
- http://127.0.0.1:8080/api/estado — prueba de conexión SQL
- http://127.0.0.1:8080/api/carreras — SELECT de carreras
- http://127.0.0.1:8080/api/estudiantes — SELECT de estudiantes (primeros 50)

**No abras los archivos HTML con doble clic**: el JavaScript realiza peticiones a `/api/...` del servidor C++; abrir páginas desde `file://` no ofrece ese servidor.

## 5. Módulos y endpoints

| Módulo | API | Base de datos |
|---|---|---|
| AC-43 Registrar | POST `/api/estudiantes` | INSERT en `public.estudiante` |
| AC-44 Buscar por RUT, nombre o carrera | GET `/api/estudiantes?rut=&nombre=&carrera=&pagina=1` | SELECT JOIN `estudiante` + `carrera` |
| AC-46 Modificar datos | GET/PUT `/api/estudiantes/{rut}` | SELECT/UPDATE en `estudiante` |
| Cambiar estado | POST `/api/estudiantes/{rut}/estado` | UPDATE más registro de historial dentro de una transacción |
| AC-49 Consultar historial de estados | GET `/api/estudiantes/{rut}/historial` | SELECT en `historial_estado_estudiante` |

Las consultas se parametrizan con `PQexecParams`, no con concatenación de valores provenientes de formularios.

## Correspondencia de atributos

- `anioIngreso` era un `int` en tu clase, pero en PostgreSQL `anio_ingreso` es tipo `DATE`. Se conserva el `int` para compatibilidad y se añade `fechaIngreso` (`AAAA-MM-DD`) para el formulario.
- `codigoBarra` es derivado (`EST-` + RUT), pues no existe columna homónima en la tabla del respaldo.
- `id_usuario` se envía opcionalmente; si se informa debe existir en `public.usuario`.
- `estado` inicia en `Activo` y los cambios se registran con motivo y responsable.

## Seguridad y límites

Proyecto de **desarrollo local académico**, no apto para producción: no implementa autenticación/autorización, CSRF, HTTPS o control de usuarios. No exponer el puerto a redes externas. El historial solo captura **cambios de estado**; modificar otros campos no genera eventos. El servidor es monohilo. No fue posible probar la conexión a tu PostgreSQL de Windows desde aquí.
