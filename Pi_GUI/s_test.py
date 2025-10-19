import tkinter as tk
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
        arduino.close()

def serial_vtn():
    vtn_ser=tk.Tk()
    vtn_ser.geometry("480x340")
    vtn_ser.title("Serial")
    vtn_ser.resizable(False, False)
    vtn_ser.config(bg="black")

    ###Cuadro de serial
    serial_caja=tk.Text(vtn_ser, width=56, height=20)
    
    serial_caja.place(relx=0.5, rely=0.45, anchor="center")

    vtn_ser.mainloop()

serial_vtn()