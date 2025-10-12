###Archivo de prueba para manejo  de imagenes

import tkinter as tk
from tkinter import filedialog
from PIL import Image, ImageTk

vtn=tk.Tk()
vtn.title("Prueba")
image=Image.open("piolin.jpeg")
photo=ImageTk.PhotoImage(image)
lbl=tk.Label(vtn, image=photo)
lbl.image=photo
lbl_txt=tk.Label(vtn, text="Noloshe", font=("Arial",50))
lbl_txt.pack()
lbl.pack(after=lbl_txt)

vtn.mainloop()