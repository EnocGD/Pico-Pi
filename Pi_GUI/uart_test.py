import tkinter as tk
import funciones

def mandar():
    mensaje=b'mensajito'
    funciones.enviar_comando(mensaje, 1)

serial_vtn=tk.Tk()

serial_vtn.geometry("480x340")
serial_vtn.title("Cereal")
serial_vtn.resizable(False, False)

entrada=tk.Entry(serial_vtn, width=10)
lbl_respuesta=tk.Label(serial_vtn, text="No se")
up_btn=tk.Button(serial_vtn, text="Mandar", command=mandar)

entrada.pack()
lbl_respuesta.pack()
up_btn.pack()

serial_vtn.mainloop()