from pwdlib import PasswordHash
from backend.repositorios.usuario_repositorio import UsuarioRepositorio


class Autenticacion:

    def __init__(self):
        self.password_hash = PasswordHash.recommended()
        self.usuario_repo = UsuarioRepositorio()

    def generar_hash(self, contrasena):
        return self.password_hash.hash(contrasena)

    def verificar_contrasena(self, contrasena, hash_guardado):
        return self.password_hash.verify(
            contrasena,
            hash_guardado
        )

    def iniciar_sesion(self, id_usuario, contrasena):

        usuario = self.usuario_repo.buscar_por_id(id_usuario)

        if usuario is None:
            return None

        if not self.verificar_contrasena(
            contrasena,
            usuario["contrasena"]
        ):
            return None

        roles = self.usuario_repo.obtener_roles(id_usuario)

        return {
            "id_usuario": id_usuario,
            "roles": roles
        }