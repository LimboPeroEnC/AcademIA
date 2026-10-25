# AcademIATEST — AC-45, AC-50, AC-51, AC-52, AC-54, AC-55 y AC-56

Extensión de **AcademIATEST** (C++17, Boost.Beast/JSON, PostgreSQL 18, HTML/CSS/JS). Mantiene los módulos anteriores AC-43/44/46/47/48/49/53/57.

## Requerimientos implementados

| Código | Interfaz | API / tablas |
|---|---|---|
| AC-45 Ficha académica básica | `/ficha.html` | `GET /api/estudiantes/{rut}/ficha` — estudiante + carrera + facultad + plan y ramos del plan |
| AC-50 Facultades | `/facultades.html` | `GET/POST /api/facultades`, `PUT/DELETE /api/facultades/{id}` — `facultad` |
| AC-51 Carreras | `/carreras.html` | `GET/POST /api/carreras`, `PUT /api/carreras/{id}` — `carrera` |
| AC-52 Planes | `/planes.html` | `GET/POST /api/planes`, `PUT /api/planes/{id}` — `plan_estudios` |
| AC-54 Prerrequisitos | `/prerrequisitos.html` | `GET/POST /api/planes/{id}/prerrequisitos`, `DELETE /api/planes/{id}/prerrequisitos/{ramo}/{requisito}` — tabla nueva `asignatura_prerrequisito` |
| AC-55 Asociación asignatura–plan–semestre | `/asociar-asignaturas.html` | `GET/POST /api/planes/{id}/asignaturas`, `DELETE /api/planes/{id}/asignaturas/{ramo}` — `plan_asignatura` |
| AC-56 Créditos | `/creditos.html` | `PATCH /api/asignaturas/{id}/creditos` — `ramo.cantidad_creditos` |

La ficha AC-45 **no** muestra avance individual, notas, cursos aprobados ni evaluaciones: esas tablas/relaciones no existen en el respaldo SQL original. El listado de ramos es el **plan curricular**, no un registro de asignaturas cursadas.

## 1. Instalación sin perder tus cambios

1. **Haz una copia de seguridad** de `AcademIATEST` y un respaldo PostgreSQL antes de aplicar la migración.
2. Si usas el archivo **actualización**, copia sus carpetas/archivos **sobre** la raíz de `AcademIATEST` (donde están `CMakeLists.txt`, `frontend/`, `backend/` y `sql/`). Sobrescribe los archivos de programa anteriores que traiga el paquete. Si modificaste archivos por tu cuenta, compara primero los cambios en VS Code.
3. **Conserva tu archivo `.env`** (el paquete no incluye contraseñas). Tu PostgreSQL 18 usa `DB_PORT=5433`; la API utiliza `API_PORT=8080`.
4. `sql/academico_en_curso.sql` debe haber sido ejecutado antes (crea `plan_estudios`, `plan_asignatura` y `estudiante_plan`).
5. En **pgAdmin 4**, abre `DBAcademiaUltimate` > **Query Tool** > **Open File (Ctrl+O)** > selecciona `sql/academico_pendientes.sql` > **F5**. **No** escribas el nombre del archivo como una consulta SQL.
6. La migración crea `asignatura_prerrequisito` y sus triggers. No elimina los registros anteriores. `DROP TRIGGER IF EXISTS` solo reemplaza los triggers de validación creados por esta entrega.

Comprueba la migración en Query Tool:

```sql
SELECT table_name FROM information_schema.tables
WHERE table_schema='public' AND table_name IN
('plan_estudios','plan_asignatura','estudiante_plan','asignatura_prerrequisito')
ORDER BY table_name;
```

Deben aparecer cuatro nombres si ya ejecutaste ambas migraciones. No ejecutes `demo_malla_OPCIONAL.sql` si no deseas insertar datos ficticios adicionales.

## 2. Compilar desde VS Code / PowerShell en AcademIATEST

```powershell
$env:Path = "C:\msys64\ucrt64\bin;" + $env:Path
cmake -S . -B build-ucrt -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=C:/msys64/ucrt64 -DCMAKE_C_COMPILER=C:/msys64/ucrt64/bin/gcc.exe -DCMAKE_CXX_COMPILER=C:/msys64/ucrt64/bin/g++.exe
cmake --build build-ucrt
.\iniciar_windows.cmd
```

Si CMake ya había generado la carpeta `build-ucrt`, los cambios a `CMakeLists.txt` se reconocen en la nueva configuración. Inicia **desde la raíz de AcademIATEST**. Abre `http://127.0.0.1:8080/` y comprueba `http://127.0.0.1:8080/api/estado`. Mantén la terminal del servidor abierta.

Dependencias MSYS2 UCRT64: `mingw-w64-ucrt-x86_64-gcc`, `mingw-w64-ucrt-x86_64-cmake`, `mingw-w64-ucrt-x86_64-ninja`, `mingw-w64-ucrt-x86_64-boost`, `mingw-w64-ucrt-x86_64-postgresql`.

## 3. Pruebas funcionales recomendadas (datos ficticios)

1. **AC-50**: crea facultad `FAC-PRUEBA` y edita su nombre. Comprueba que **no puedes eliminarla** mientras tenga carreras.
2. **AC-51**: crea carrera `CAR-PRUEBA`, ligada a `FAC-PRUEBA`, con duración de 8 semestres. Edita su modalidad.
3. **AC-52**: crea plan `PLAN-PRUEBA` para `CAR-PRUEBA`; modifica nombre, año y vigencia.
4. **AC-56**: utiliza una asignatura ficticia existente (AC-53) o registra dos nuevas; actualiza sus créditos y revisa el resultado en `public.ramo`.
5. **AC-55**: agrega un ramo en semestre 1 y otro en semestre 2 del plan; confirma que el formulario muestra ambas asociaciones. No permite más de 8 semestres para la carrera de prueba.
6. **AC-54**: define el ramo de semestre 1 como prerrequisito del ramo de semestre 2. Debe rechazar un prerrequisito del mismo semestre o posterior.
7. **AC-45**: consulta un RUT ya registrado y con un plan asociado (AC-48). Comprueba datos de estudiante, carrera, facultad, plan y materias por semestre. No interpreta esas asignaturas como aprobadas.
8. Reinicia el servidor y vuelve a consultar en el navegador: deben mantenerse los cambios guardados en PostgreSQL.

Verificación con SQL:

```sql
SELECT * FROM public.facultad ORDER BY id_facultad;
SELECT * FROM public.carrera ORDER BY id_carrera;
SELECT * FROM public.plan_estudios ORDER BY id_plan;
SELECT * FROM public.plan_asignatura ORDER BY id_plan,semestre;
SELECT * FROM public.asignatura_prerrequisito ORDER BY id_plan,id_ramo;
SELECT id_ramo,nombre_ramo,cantidad_creditos FROM public.ramo ORDER BY id_ramo;
```

## Estructura POO y decisiones de modelado

- `backend/academico/GestorCatalogos.*`: validaciones y reglas de negocio de los siete requisitos.
- `backend/academico/RepositorioCatalogos.*`: consultas SQL parametrizadas con `libpq` (sin SQL dentro del frontend).
- `backend/api/Servidor.cpp`: API HTTP local mediante Boost.Beast/JSON.
- `frontend/js/restantes.js`: llamadas fetch a las rutas anteriores.
- `sql/academico_pendientes.sql`: DDL aditivo de prerrequisitos y triggers.
- Las asignaturas se asocian a **planes** y por esa relación a **carreras**. Un mismo ramo puede existir en más de un plan y ocupar un semestre distinto en cada uno.
- Los créditos de `ramo.cantidad_creditos` se definen globalmente por ramo, no por plan. Cuando se cambian, todas las mallas que incluyan ese ramo muestran los nuevos créditos.
- Las relaciones de prerrequisitos se guardan por plan. Ambas materias deben estar asociadas a ese plan y el prerrequisito debe ir en un semestre **anterior**; la migración también bloquea cambios de semestre que invaliden dependencias ya registradas.
- La eliminación de una asignatura de un plan elimina por cascada sus **vínculos de prerrequisitos de ese plan**; no elimina el ramo de la base. El frontend lo advierte antes de confirmar.
- Las facultades con carreras relacionadas **no se pueden eliminar**, evitando eliminación accidental de carreras, planes o estudiantes.
- Los IDs de facultad, carrera, plan y ramo son inmutables mediante las interfaces de edición de esta versión.

## Seguridad y restricciones actuales

**Solo para pruebas locales con datos ficticios.** La API todavía no verifica permisos/autenticación de cada operación, no debe publicarse en red ni conectarse a datos institucionales sensibles antes de implementar inicio de sesión, roles y controles de acceso en el backend. El servidor escucha en `127.0.0.1`. No guardes `.env` en GitHub. No se hizo una prueba de escritura contra tu PostgreSQL Windows desde este entorno; debes validar el funcionamiento SQL en tu equipo.
