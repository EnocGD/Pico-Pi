import tkinter as tk
import time

def osc():
    print("osc")

def multi():
    print("Multi")

def doc():
    print("Abrir documentacion")

def conec():
    print("Conexiones")



vtn_main= tk.Tk()
vtn_main.geometry("480x340")
vtn_main.config(bg="bisque2")
vtn_main.title("Menu Principal")
vtn_main.resizable(False, False)
###Widgets vtn_main
btn_osc=tk.Button(vtn_main,
                  text="Abrir \nosciloscopio",
                  command=osc,
                  bg="bisque3")
btn_multi=tk.Button(vtn_main,
                    text="Abrir \nMultimetro",
                    command= multi,
                    bg="bisque3")
btn_doc=tk.Button(vtn_main,
                  text="Revisar\nDocumentacion",
                  command=doc,
                  bg="bisque3")
btn_conec=tk.Button(vtn_main,
                    text="Modulos \nconectados",
                    command=conec,
                    bg="bisque3")
##Dibujitos
osc_icon=tk.PhotoImage(file="osciloscopio.png")
multi_icon=tk.PhotoImage(file="multimetro-digital.png")
board_icon=tk.PhotoImage(file="board.png")
docs_icon=tk.PhotoImage(file="docs.png")
#Configuracion de widgets
btn_multi.config(image=multi_icon,
                 compound=tk.TOP,
                 padx=5, pady=5)
btn_osc.config(image=osc_icon,
               compound=tk.TOP,
               padx=5, pady=5)
btn_doc.config(image=docs_icon,
               compound=tk.TOP,
               padx=5, pady=5)
btn_conec.config(image=board_icon,
                 compound=tk.TOP,
                 padx=5, pady=5)


###Poner widgets
btn_osc.grid(row=0, column=0, padx=50, pady=20)
btn_multi.grid(row=0, column=8,padx=50, pady=20)
btn_doc.grid(row=6, column=0,padx=50, pady=20)
btn_conec.grid(row=6, column=8,padx=50, pady=20)


vtn_main.mainloop()