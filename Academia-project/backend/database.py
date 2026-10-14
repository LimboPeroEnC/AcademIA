import os
import psycopg
from pathlib import Path
from dotenv import load_dotenv

ruta_env = Path(__file__).resolve().parent.parent / ".env"

load_dotenv(dotenv_path=ruta_env, override=True)

class Database:

    def __init__(self):
        self.host = os.getenv("DB_HOST")
        self.port = os.getenv("DB_PORT")
        self.database = os.getenv("DB_NAME")
        self.user = os.getenv("DB_USER")
        self.password = os.getenv("DB_PASSWORD")

    def conectar(self):
        return psycopg.connect(
            host=self.host,
            port=self.port,
            dbname=self.database,
            user=self.user,
            password=self.password
        )

    def probar_conexion(self):
        try:
            with self.conectar() as conexion:
                print("Conexión exitosa con PostgreSQL")
                return True

        except Exception as error:
            print("Error de conexión:", error)
            return False