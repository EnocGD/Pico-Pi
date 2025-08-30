###Prototi´po de ventana para mostrar info de la tarjeta
import tkinter as tk
##Variables
mod="OPAMP"
lecturas=dict()
lecturas={"VCC":"12V",
          "-VCC":"-12V",
          "RefFija": "1.8V",
          "SComp1":"12V",
          "RefVar": "2V",
          "SComp2":"-12V",
          "RefVtn1":"3V",
          "RefVtn2":"4V",
          "CompVtn":"12V"}
###Creacion de ventana


vtn_shoko =tk.Tk()
vtn_shoko.geometry("480x340")
vtn_shoko.title("Modulo ")
vtn_shoko.resizable(False, False)

###Etiquetas
lbl_title=tk.Label(vtn_shoko,
           text= f"Modulo {mod}"
            )
lbl_status=tk.Label(vtn_shoko,
                    text="STATUS")
lbl_temas=tk.Label(vtn_shoko,
                   text="Temas")

###Posicionamiento de widgets
lbl_title.grid(column=18, row=0, rowspan=2, columnspan=18,padx=50)
lbl_status.grid(column=2, row=3, columnspan=2)
lbl_temas.grid(column=50, row=3, columnspan=2)

###Creacion de tabla con parametros
count=0
for i in lecturas.keys():

    ent=tk.Entry(vtn_shoko, width=10,
                 font=("Arial", 12))
    ent.grid(row=(4+count), column=2)
    ent.insert(tk.END,i)
    count=count+1
count=0
for i in lecturas:
    ent=tk.Entry(vtn_shoko, width=6,
                 font=("Arial", 10))
    ent.grid(row=(4+count), column=12)
    ent.insert(tk.END, lecturas[i])
    count=count+1

vtn_shoko.mainloop()
