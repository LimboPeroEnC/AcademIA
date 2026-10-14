from backend.database import Database
from psycopg.rows import dict_row


class UsuarioRepositorio:

    def __init__(self):
        self.db = Database()

    def buscar_por_id(self, id_usuario):

        with self.db.conectar() as conexion:
            with conexion.cursor(row_factory=dict_row) as cursor:

                cursor.execute(
                    """
                    SELECT id_usuario, contrasena
                    FROM usuario
                    WHERE id_usuario = %s
                    """,
                    (id_usuario,)
                )

                return cursor.fetchone()

    def obtener_roles(self, id_usuario):

        with self.db.conectar() as conexion:
            with conexion.cursor(row_factory=dict_row) as cursor:

                cursor.execute(
                    """
                    SELECT r.nombre
                    FROM usuario_rol ur
                    INNER JOIN rol r
                        ON ur.id_rol = r.id_rol
                    WHERE ur.id_usuario = %s
                    """,
                    (id_usuario,)
                )

                resultados = cursor.fetchall()

                return [fila["nombre"] for fila in resultados]