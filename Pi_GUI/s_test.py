import serial
import serial.tools.list_ports
import time

###Funcion para localizar puerto de arduino
def pto_arduino():
    puerto = serial.tools.list_ports.comports()
    for pto in puerto:
        if "Arduino" in pto.description or "CH340" in pto.description:
            return pto.device
        else:
            return None

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

def arduino_play():
    arduino=config_terminal()
    while True:
        respuesta= arduino.readline().decode().strip()
        print(f"Arduino dice {respuesta}")
        yo=input("Di algo")
        arduino.write(yo)


'''arduino.write(b"Bolas")

respuesta = arduino.readline().decode().strip()
arduino.close()'''
arduino_play()