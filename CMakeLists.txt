cmake_minimum_required(VERSION 3.20)
project(SistemaAcademicoEstudiantes LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(PostgreSQL REQUIRED)
# Boost.Beast, Boost.Asio y Boost.JSON; Boost.JSON se compila como unidad de traduccion separada.
find_path(BOOST_INCLUDE_DIR boost/beast.hpp REQUIRED)
add_executable(servidor_estudiantes
    backend/modelos/Estudiante.cpp
    backend/gestores/GestorEstudiantes.cpp
    backend/repositorios/RepositorioEstudiantes.cpp
    backend/database/ConexionPostgreSQL.cpp
    backend/util/ValidadorRut.cpp
    backend/academico/RepositorioAcademico.cpp
    backend/academico/GestorAcademico.cpp
    backend/academico/RepositorioCatalogos.cpp
    backend/academico/GestorCatalogos.cpp
    backend/api/Servidor.cpp
    backend/api/BoostJson.cpp)
target_include_directories(servidor_estudiantes PRIVATE ${BOOST_INCLUDE_DIR})
target_compile_definitions(servidor_estudiantes PRIVATE BOOST_ERROR_CODE_HEADER_ONLY BOOST_ALL_NO_LIB BOOST_JSON_NO_LIB)
target_link_libraries(servidor_estudiantes PRIVATE PostgreSQL::PostgreSQL)
if(WIN32)
  target_link_libraries(servidor_estudiantes PRIVATE ws2_32 mswsock)
else()
  find_package(Threads REQUIRED)
  target_link_libraries(servidor_estudiantes PRIVATE Threads::Threads)
endif()
