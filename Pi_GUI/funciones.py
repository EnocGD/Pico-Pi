import serial.tools.list_ports

def serial_init():
    print("Serial inicializado")

def enviar_comando(comando):
    serial_init()
    print(comando)

def recibir_lectura():
    print("leer")
    dato=[80, 88]
    return dato

def recibir_lectura_nb():

    print("lectura sin deco")
    return b'bola'

def config_terminal():
    while True:
        puerto = pto_arduino()
        if puerto:
            print(f"Arduino en puerto {puerto}")
            break
        else:
            print("Aun no lo encuentro")
            time.sleep(5)
    ser_device= serial.Serial(puerto, 9600, timeout=1)
    time.sleep(2)
    return ser_device
def arduino_escribe(mensaje, arduino):
    mensaje=mensaje.encode('utf-8')
    arduino.write(mensaje)

def arduino_leer(arduino):
    mensaje=arduino.readline().decode('utf-8')
    return mensaje

def arduino_inter(mensaje, arduino):
    mensaje=mensaje.encode('utf-8')
    arduino.write(mensaje)
    respuesta=arduino.readline().decode('utf-8')
    return respuesta
def arduino_play():
    arduino=config_terminal()
    while True:
        respuesta= arduino.readline().decode()
        print(f"Arduino dice {respuesta}")
        yo=input("Di algo")
        yo=yo.encode('utf-8')
        arduino.write(yo)
    arduino.close()

