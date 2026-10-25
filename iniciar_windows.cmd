@echo off
setlocal
cd /d "%~dp0"
if not exist ".env" (
  echo [ERROR] No se encontro .env. Ejecuta: copy .env.example .env
  pause
  exit /b 1
)
rem Cargar configuracion local sin subir las credenciales a Git.
for /f "usebackq eol=# tokens=1,* delims==" %%A in (".env") do set "%%A=%%B"
rem DLL necesarias para binarios compilados con GCC UCRT64.
set "PATH=C:\msys64\ucrt64\bin;%PATH%"
if exist "build-ucrt\servidor_estudiantes.exe" (
  "build-ucrt\servidor_estudiantes.exe"
) else if exist "build\Release\servidor_estudiantes.exe" (
  "build\Release\servidor_estudiantes.exe"
) else if exist "build\servidor_estudiantes.exe" (
  "build\servidor_estudiantes.exe"
) else (
  echo [ERROR] No se encontro servidor_estudiantes.exe. Compila primero con CMake.
)
pause
