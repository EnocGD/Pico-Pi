
def leer_mods():
    print("Buscar mods")
    lectura={"Nombre":"OPAMP",
             "Puertos": 9,
             "VCC": "12V",
             "-VCC": "-12V",
             "RefFija": "1.8V",
             "SComp1": "12V",
             "RefVar": "2V",
             "SComp2": "-12V",
             "RefVtn1": "3V",
             "RefVtn2": "4V",
             "CompVtn": "12V",
             "status": "Listo"
             }
    return lectura


def mods_disponibles():
    modulongos=["Listo","OPAMP","COMBINACIONALES","VACIO", "VACIO"]
    return modulongos

def escribe(Mensaje):
    print(Mensaje)

def nop():
    return "Espera"