import serial
import time

def serial_init():
    ser= serial.Serial('/dev/serial0', 9600, timeout=1)
    return ser
    
def enviar_comando(comando):
    ser=serial_init()
    ser.write(comando)
    ser.close()

def recibir_lectura():
	ser=serial_init()
	while True:
		print("Hola")
		if ser.in_waiting > 0:
			data = ser.readline().decode('utf-8')
			if data:
				print("Recibido:", data)
				break
		time.sleep(0.05)
	ser.close()
	return data
	
def recibir_lectura_nb():
	ser=serial_init()
	while True:
		print("Hola")
		if ser.in_waiting > 0:
			data = ser.readline()
			if data:
				print("Recibido:", data)
				break
		time.sleep(0.05)
	ser.close()
	return data
		
