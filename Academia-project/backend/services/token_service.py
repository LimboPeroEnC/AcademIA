import os
from datetime import datetime, timedelta, timezone

import jwt
from dotenv import load_dotenv
from pathlib import Path

BASE_DIR = Path(__file__).resolve().parent.parent.parent
load_dotenv(BASE_DIR / ".env")


class TokenService:

    def __init__(self):
        self.secret_key = os.getenv("JWT_SECRET_KEY")
        self.algorithm = "HS256"
        self.expiration = int(
            os.getenv("JWT_EXPIRATION_MINUTES", "30")
        )

        if not self.secret_key:
            raise RuntimeError(
                "Falta configurar JWT_SECRET_KEY"
            )

    def generar_token(self, id_usuario):
        ahora = datetime.now(timezone.utc)

        datos = {
            "sub": id_usuario,
            "iat": ahora,
            "exp": ahora + timedelta(minutes=self.expiration)
        }

        return jwt.encode(
            datos,
            self.secret_key,
            algorithm=self.algorithm
        )

    def verificar_token(self, token):
        try:
            datos = jwt.decode(
                token,
                self.secret_key,
                algorithms=[self.algorithm],
                options={"require": ["sub", "exp"]}
            )

            return datos["sub"]

        except jwt.InvalidTokenError:
            return None