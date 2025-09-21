import tkinter as tk

import time

import pico_funciones as pico

###Funciones

#Modificar para mostrar inicializacion de la pico y del sistema, Modificar con try, except
def act_status(cont,cin):

    if cin<cont:
        cin+=1
        lbl_status.config(text=str(cin))
        lbl_status.after(1000,act_status,cont,cin)
    else:
        lbl_status.config(text="Listo")
        crear_btn_inicio()
def menu_aprender():
    global lbl_buscar
    global mods_listos
    global vtn_lect_mods

    vtn_lect_mods=tk.Toplevel(vtn_menu)
    vtn_lect_mods.geometry("480x340")
    vtn_lect_mods.title("Leyendo modulongos")
    vtn_lect_mods.resizable(False, False)
    vtn_menu.withdraw()
    lbl_buscar=tk.Label(vtn_lect_mods,
                        text="Buscando...",
                        font=("Arial", 22))
    lbl_buscar.place(relx=0.5, rely=0.5, anchor='center')
    act_mods("No")


def aprender_quad():
    vtn_modsq=tk.Toplevel(vtn_menu)
    vtn_lect_mods.destroy()
    vtn_modsq.geometry("480x340")
    vtn_modsq.title("Modulos listos")
    vtn_modsq.resizable(False, False)

    ##Widgets
    lbl_slot1=tk.Label(vtn_modsq,
                       text="SLOT 1",
                       font=("Arial", 14))
    lbl_slot2=tk.Label(vtn_modsq,
                       text="SLOT 2",
                       font=("Arial", 14))
    lbl_slot3=tk.Label(vtn_modsq,
                       text="SLOT 3",
                       font=("Arial", 14))
    lbl_slot4=tk.Label(vtn_modsq,
                       text="SLOT 4",
                       font=("Arial", 14))
    lbl_t1=tk.Label(vtn_modsq,
                       text=mods_listos[1],
                       font=("Arial", 10))
    lbl_t2=tk.Label(vtn_modsq,
                       text=mods_listos[2],
                       font=("Arial", 10))
    lbl_t3=tk.Label(vtn_modsq,
                       text=mods_listos[3],
                       font=("Arial", 10))
    lbl_t4=tk.Label(vtn_modsq,
                       text=mods_listos[4],
                       font=("Arial", 10))

    ##Colocar widgets
    lbl_slot1.grid(column=0, row=0, columnspan=16,rowspan=2,padx=5,pady=5)
    lbl_t1.grid(column=0, row=3, columnspan=16, rowspan=2, padx=5)
    lbl_slot2.grid(column=40, row=0, columnspan=16, rowspan=2,padx=150, pady=5)
    lbl_t2.grid(column=40, row=3, columnspan=16, rowspan=2, padx=150)
    lbl_slot3.grid(column=0, row=8, columnspan=16, rowspan=2, padx=5, pady=5)
    lbl_t3.grid(column=0, row=11, columnspan=16, rowspan=2, padx=5)
    lbl_slot4.grid(column=40, row=8, columnspan=16, rowspan=2, padx=150, pady=5)
    lbl_t4.grid(column=40, row=11, columnspan=16, rowspan=2,padx=150)


def act_mods(mods):
    global mods_listos
    if mods== "No":
        lbl_buscar.config(text="Buscando")
        lbl_buscar.after(5000, act_mods, pico.nop())
    elif mods=="Nadota":
        lbl_buscar.config(text="Actualizando")
        mods_listos=pico.mods_disponibles()
        lbl_buscar.after(3000, act_mods, mods_listos[1])
    else:
        lbl_buscar.config(text="Listo")
        aprender_quad()

def tbd2():
    print("calale")

def instru():
    print("Dr profesor patricio")

def docs():
    print("Aqui va la documentacion")


def bienvenida():
    global main_vtn
    global lbl_status
    main_vtn = tk.Tk()
    stats = "Cargando pico"
    ##Imagenes
    logo = tk.PhotoImage(file="logo.png")

    main_vtn.geometry("480x340")
    main_vtn.title("Bienvenida")
    main_vtn.resizable(False, False)
    ###Etiquetas
    lbl_bienvenido = tk.Label(main_vtn,
                              text="BIENVENIDO",
                              font=("Arial", 16))
    lbl_status = tk.Label(main_vtn,
                          text=stats,
                          font=("Arial", 12))
    lbl_blogo = tk.Label(image=logo)
    ###Posicionamiento de widgets
    lbl_bienvenido.grid(column=10, row=1, columnspan=3, rowspan=3, padx=180)
    lbl_blogo.grid(column=10, row=5, padx=90)
    lbl_status.grid(column=10, row=100, padx=100)

    act_status(10, 0)

    main_vtn.mainloop()

def crear_btn_inicio():
    btn_inicio=tk.Button(main_vtn,
                         text="Iniciar",
                         command=inicio)
    btn_inicio.grid(column=10, row=102)

def inicio():
    print("Continuamooos")
    crear_main_vtn()

def crear_main_vtn():
    main_vtn.destroy()
    global vtn_menu
    vtn_menu=tk.Tk()

    vtn_menu.title("Menu principal")
    vtn_menu.geometry("480x340")
    vtn_menu.resizable(False, False)
    ##Imagenes
    ajalas_icon = tk.PhotoImage(file="ajalas.png")
    docs_icon= tk.PhotoImage(file="docs.png")
    aprender_icon= tk.PhotoImage(file="aprender.png")
    instru_icon=tk.PhotoImage(file="medicion.png")

    ##Widgets
    btn_aprender=tk.Button(vtn_menu,
                       text="Aprendamos",
                       command=menu_aprender,width=220, height=150)
    btn_tbd2=tk.Button(vtn_menu,
                       text="TBD2",
                       command=tbd2,width=220, height=150)
    btn_ins=tk.Button(vtn_menu,
                      text="Instrumentacion",
                      command=instru,width=220, height=150)
    btn_docs=tk.Button(vtn_menu,
                       text="Documentacion",
                       command=docs,width=220, height=150)
    ##Ponerle iconos
    btn_ins.config(image=instru_icon,
                   compound=tk.TOP,
                   padx=5, pady=5)
    btn_aprender.config(image=aprender_icon,
                        compound=tk.TOP,
                        padx=5, pady=5)
    btn_docs.config(image=docs_icon,
                    compound=tk.TOP,
                    padx=5, pady=5)
    btn_tbd2.config(image=ajalas_icon,
                    compound=tk.TOP,
                    padx=5, pady=5)


    ##Colocar botones de gomita
    btn_aprender.grid(column=6, row=0, columnspan=6,rowspan=2, padx=5, pady=5)
    btn_ins.grid(column=18, row=0, columnspan=6, rowspan=2, pady=5)
    btn_docs.grid(column=6, row=4, columnspan=6, rowspan=2, padx=5)
    btn_tbd2.grid(column=18, row=4, columnspan=6, rowspan=2)

    vtn_menu.mainloop()

bienvenida()
