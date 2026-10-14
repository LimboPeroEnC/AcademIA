from backend.database import Database

db = Database()

print("Host:", db.host)
print("Puerto:", db.port)
print("Base de datos:", db.database)
print("Usuario:", db.user)
print("Contraseña configurada:", bool(db.password))

db.probar_conexion()