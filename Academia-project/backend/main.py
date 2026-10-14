from fastapi import FastAPI, HTTPException
from psycopg.rows import dict_row
from backend.database import Database
from pydantic import BaseModel
from fastapi import HTTPException
from backend.services.autenticacion import Autenticacion
from backend.services.token_service import TokenService
from fastapi import Depends
from fastapi.security import HTTPBearer, HTTPAuthorizationCredentials
from backend.repositorios.usuario_repositorio import UsuarioRepositorio
from fastapi.middleware.cors import CORSMiddleware

app = FastAPI(title="Academia")
app.add_middleware(
    CORSMiddleware,
    allow_origins=["http://localhost:3000"],
    allow_credentials=False,
    allow_methods=["GET", "POST", "PUT", "PATCH", "DELETE"],
    allow_headers=["Authorization", "Content-Type"],
)


@app.get("/")
def inicio():
    return {"mensaje": "Backend funcionando"}


@app.get("/api/estudiantes")
def listar_estudiantes():

    db = Database()

    try:
        with db.conectar() as conexion:
            with conexion.cursor(row_factory=dict_row) as cursor:

                cursor.execute("""
                    SELECT
                        rut,
                        nombres,
                        apellido_paterno,
                        apellido_materno,
                        correo_institucional
                    FROM estudiante
                """)

                estudiantes = cursor.fetchall()

                return estudiantes

    except Exception as error:
        print("ERROR EN POSTGRESQL:", repr(error))

        raise HTTPException(
            status_code=500,
            detail="Error al consultar estudiantes"
        )

class DatosLogin(BaseModel):
    id_usuario: str
    contrasena: str


@app.post("/api/login")
def login(datos: DatosLogin):

    autenticacion = Autenticacion()

    resultado = autenticacion.iniciar_sesion(
        datos.id_usuario,
        datos.contrasena
    )

    if resultado is None:
        raise HTTPException(
            status_code=401,
            detail="Credenciales incorrectas"
        )

    token_service = TokenService()

    token = token_service.generar_token(
        resultado["id_usuario"]
    )

    return {
        "mensaje": "Inicio de sesión exitoso",
        "access_token": token,
        "token_type": "bearer",
        "usuario": resultado
    }
    
seguridad = HTTPBearer()


def obtener_usuario_actual(
    credenciales: HTTPAuthorizationCredentials = Depends(seguridad)
):
    token_service = TokenService()

    id_usuario = token_service.verificar_token(
        credenciales.credentials
    )

    if id_usuario is None:
        raise HTTPException(
            status_code=401,
            detail="Token inválido o expirado"
        )

    repositorio = UsuarioRepositorio()
    usuario = repositorio.buscar_por_id(id_usuario)

    if usuario is None:
        raise HTTPException(
            status_code=401,
            detail="Usuario no encontrado"
        )

    return {
        "id_usuario": id_usuario,
        "roles": repositorio.obtener_roles(id_usuario)
    }


@app.get("/api/mi-perfil")
def mi_perfil(usuario=Depends(obtener_usuario_actual)):
    return usuario

@app.get("/api/estudiante/panel")
def panel_estudiante(
    usuario=Depends(obtener_usuario_actual)
):

    if "Estudiante" not in usuario["roles"]:
        raise HTTPException(
            status_code=403,
            detail="No tienes permiso para acceder"
        )

    return {
        "mensaje": "Bienvenido al panel de estudiante",
        "id_usuario": usuario["id_usuario"]
    }