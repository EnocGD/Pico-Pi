###Experimetacion gui txt
import tkinter as tk

def guardar():
    titulo=titulo_entry.get()
    titulo=titulo+".txt"
    contenido=contenido_txt.get("1.0", "end-1c")
    f=open(titulo, "x")
    f.writelines(contenido)
    print(titulo)
    print(contenido)
    f.close()

vtn_text=tk.Tk()
vtn_text.geometry("640x480")
vtn_text.title("Prueba de texto")
lbl_titulo=tk.Label(vtn_text, text="Titulo")
lbl_contenido=tk.Label(vtn_text, text="Ingresa lo que quieres escribir")
titulo_entry=tk.Entry(vtn_text, width=50)
contenido_txt=tk.Text(vtn_text, width=50, height=20)
btn_guardar=tk.Button(vtn_text, text="Guardar", command=guardar)

lbl_titulo.grid(column=0, row=0, columnspan=10, rowspan=4, padx=5, pady=2)
titulo_entry.grid(column=0, row=5, columnspan=10, rowspan=4, pady=2)
lbl_contenido.grid(column=0, row=10, columnspan=10, rowspan=4, padx=5, pady=2)
contenido_txt.grid(column=0, row=15, columnspan=10, rowspan=4, pady=2)
btn_guardar.grid(column=10, row=90)

vtn_text.mainloop()