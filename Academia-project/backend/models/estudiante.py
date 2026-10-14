class Estudiante:

    def __init__(self, rut, nombres, apellido_paterno,
                 apellido_materno, correo_institucional):

        self.rut = rut
        self.nombres = nombres
        self.apellido_paterno = apellido_paterno
        self.apellido_materno = apellido_materno
        self.correo_institucional = correo_institucional

    def obtener_nombre_completo(self):
        return (
            f"{self.nombres} "
            f"{self.apellido_paterno} "
            f"{self.apellido_materno}"
        )