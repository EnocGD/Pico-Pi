import tkinter as tk

import time
from tkinter import IntVar

from yaml import compose_all

import pico_funciones as pico


###Variables
global tema
tema="TBD"
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
    act_mods("Buscando")


def aprender_quad():
    vtn_modsq=tk.Toplevel(vtn_menu)
    vtn_lect_mods.destroy()
    vtn_modsq.geometry("480x340")
    vtn_modsq.title("Modulos listos")
    vtn_modsq.resizable(False, False)

    ##Widgets





def act_mods(mods):
    global mods_listos
    if mods== "Buscando":
        lbl_buscar.config(text="Buscando")
        lbl_buscar.after(5000, act_mods, pico.nop())
    elif mods=="Esperando":
        lbl_buscar.config(text="Conectando...")
        mods_listos=pico.mods_disponibles()
        lbl_buscar.after(3000, act_mods, mods_listos[1])
    else:
        lbl_buscar.config(text="Listo")
        aprender_quad()

def tbd2():
    print("calale")

def instru():
    print("Dr profesor patricio")
    global vtn_instru_menu
    ###Creacion de ventana de menu instru
    vtn_instru_menu= tk.Toplevel(vtn_menu)
    vtn_instru_menu.geometry("480x340")
    vtn_instru_menu.title("Menu de instrumentacion")
    vtn_instru_menu.resizable(False, False)

    ###Imagenes
    global multi_icon
    global osc_icon
    global gen_icon
    multi_icon=tk.PhotoImage(file="multimetro-digital.png")
    osc_icon= tk.PhotoImage(file="osciloscopio.png")
    gen_icon=tk.PhotoImage(file="generador.png")
    #Widgets
    btn_multi=tk.Button(vtn_instru_menu,
                        text="Multimetro",
                        command=multimetro,
                        font=("Arial", 12),
                        width=440, height=70)
    btn_osc=tk.Button(vtn_instru_menu,
                      text="Osciloscopio",
                      command=osciloscopio,
                      font=("Arial", 12),
                      width=440, height=70)
    btn_gen= tk.Button(vtn_instru_menu,
                       text="Generador",
                       command=generador,
                       font=("Arial", 12),
                       width=440, height=70)

    lbl_menu=tk.Label(vtn_instru_menu,
                      text="Gestiona",
                      font=("Arial", 14))
    ###Añadir iconos

    btn_multi.config(image=multi_icon,
                     compound=tk.LEFT,
                     padx=5, pady=5)
    btn_gen.config(image= gen_icon,
                   compound=tk.LEFT,
                   padx=5, pady=5)
    btn_osc.config(image=osc_icon,
                   compound=tk.LEFT,
                   padx=5, pady=5)

    lbl_menu.grid(column=15, row=0, columnspan=15, rowspan=1, padx=5, pady=5)
    btn_multi.grid(column=10, row=1, columnspan=30, rowspan=10, padx=5, pady=5)
    btn_gen.grid(column=10, row=12, columnspan=30, rowspan=10, padx=5, pady=5)
    btn_osc.grid(column=10, row=22, columnspan=30, rowspan=10, padx=5, pady=5)


def multimetro():
    print("Hola aqui va el multimetro")
    global lbl_lectura
    global var
    vtn_multi=tk.Toplevel(vtn_instru_menu)
    vtn_multi.geometry("480x340")
    vtn_multi.title("Multimetro")
    vtn_multi.resizable(False, False)

    var=IntVar()

    ###Widgets
    rb_Voltaje=tk.Radiobutton(vtn_multi, text="Voltaje",
                              variable=var, value=1)
    rb_Corriente=tk.Radiobutton(vtn_multi, text="Corriente",
                                variable=var, value=2)
    rb_resistencia=tk.Radiobutton(vtn_multi, text="Resistencia",
                                  variable=var, value=3)
    lbl_lectura=tk.Label(vtn_multi, text="Lectura", font=("Arial",30))

    btn_gen= tk.Button(vtn_multi, text="Generador", command=generador)
    btn_osc=tk.Button(vtn_multi, text="Osciloscopio", command=osciloscopio)
    btn_docs= tk.Button(vtn_multi, text="Apuntes", command=docs)

    ###Colocar widgets
    lbl_lectura.place(relx=0.5, rely=0.5, anchor='center')
    rb_Voltaje.place(relx=0.25, rely=0.8, anchor= 'center')
    rb_resistencia.place(relx=0.5, rely=0.8, anchor= 'center')
    rb_Corriente.place(relx=0.75, rely=0.8, anchor= 'center')
    btn_gen.place(relx=0.95, rely=0.1, anchor='ne')
    btn_osc.place(relx=0.95, rely=0.2, anchor='ne')
    btn_docs.place(relx=0.95, rely=0.3, anchor='ne')
    multi_lectura(4)


def multi_lectura(boton):
    if boton==1:
        lbl_lectura.config(text="Voltaje")

    elif boton == 3:
        lbl_lectura.config(text="Resistencia")
    elif boton==2:
        lbl_lectura.config(text="Corriente")
    else:
        lbl_lectura.config(text="Elige que medir")
    lbl_lectura.after(500, multi_lectura, var.get())
    print(var)

def osciloscopio():
    print("Hola, soy scoopy")
    vtn_osc=tk.Toplevel(vtn_instru_menu)
    vtn_osc.geometry("480x340")
    vtn_osc.title("Como usar scoopy")
    vtn_osc.resizable(False, False)
    txt_usame=tk.Text(vtn_osc, width=45, height=15)

    btn_multi=tk.Button(vtn_osc, text="Multimetro", command=multimetro)
    btn_gen=tk.Button(vtn_osc, text="Generador", command=generador)
    btn_docs=tk.Button(vtn_osc, text="Apuntes", command=docs)

    btn_gen.place(relx=0.95, rely=0.1, anchor='ne')
    btn_multi.place(relx=0.95, rely=0.2, anchor='ne')
    btn_docs.place(relx=0.95, rely=0.3, anchor='ne')
    txt_usame.grid(column=0, row=0)
    txt_usame.insert("1.0","Soy la documentacion")

def generador():
    ###La amplitud no se puede controlar
    global lbl_generador
    global frec_gen
    global signal
    global flecha_arriba
    global flecha_abajo
    print("Hola soy un generador")
    gen_vtn=tk.Toplevel(vtn_instru_menu)
    gen_vtn.geometry("480x340")
    gen_vtn.title("Generador de funciones")
    gen_vtn.resizable(False, False)

    flecha_arriba=tk.PhotoImage(file="angulo-pequeno-hacia-arriba.png")
    flecha_abajo=tk.PhotoImage(file="angulo-hacia-abajo.png")

    ##Widgets
    lbl_generador= tk.Label(gen_vtn,
                            text="Generador",
                            font=("Arial", 20))
    btn_sin_sel=tk.Button(gen_vtn, text="Sin", command=seno)
    btn_tri_sel=tk.Button(gen_vtn, text="Triangular", command=triangular)
    btn_sqr_sel=tk.Button(gen_vtn, text="Cuadrada", command=sqr_w)
    lbl_tmp=tk.Label(gen_vtn, text="aguantala", font=("Arial", 10))
    btn_multi=tk.Button(gen_vtn, text="Multimetro", command=multimetro, width=10)
    btn_osc=tk.Button(gen_vtn, text="Osciloscopio", command=osciloscopio, width=10)
    btn_docs=tk.Button(gen_vtn, text="Apuntes", command=docs, width=10)
    btn_subir=tk.Button(gen_vtn, command=frec_aumentar)
    btn_bajar=tk.Button(gen_vtn, command=frec_bajar)
    btn_subir.config(image=flecha_arriba, compound=tk.TOP)
    btn_bajar.config(image=flecha_abajo, compound=tk.TOP)
    btn_sin_sel.config(width=10)
    btn_sqr_sel.config(width=10)
    btn_tri_sel.config(width=10)

    frec_gen=1000
    signal="Senoidal"
    ##Poner widgets
    lbl_generador.grid(column=10, row=5, columnspan=20, rowspan=10)
    btn_sqr_sel.grid(column=0, row= 30, columnspan=10, rowspan=2, padx=2)
    btn_sin_sel.grid(column=10, row=30, columnspan=10, rowspan=2, padx=2)
    btn_tri_sel.grid(column=20, row=30, columnspan=10, rowspan=2, padx=2)
    btn_subir.grid(column=0, row=35, columnspan=2)
    btn_bajar.grid(column=3, row=35, columnspan=2)
    btn_multi.place(relx=0.95, rely=0.1, anchor="ne")
    btn_osc.place(relx=0.95, rely=0.2, anchor="ne")
    btn_docs.place(relx=0.95, rely=0.3, anchor="ne")


def docs():
    print("Aqui va la documentacion")
    global lbl_elegir

    if tema=="TBD":
        global docs_var
        docs_var=IntVar()
        vtn_docs_menu=tk.Toplevel(vtn_menu)
        vtn_docs_menu.geometry("480x340")
        vtn_docs_menu.title("Documentacion")
        vtn_docs_menu.resizable(False, False)

        ###Menu de eleccion de tema
        lbl_elegir=tk.Label(vtn_docs_menu, text="Elige si quieres \nrepasar un tema\n o hacer una practica")
        rbtn_practica=tk.Radiobutton(vtn_docs_menu, text="Practicas",
                                     variable=docs_var, value=1)
        rbtn_Temas=tk.Radiobutton(vtn_docs_menu, text="Temas",
                                  variable=docs_var, value=2)

        #Colocar widgets
        lbl_elegir.pack(side="top")
        rbtn_practica.place(relx=0.1, rely=0.2, anchor="center")
        rbtn_Temas.place(relx=0.9, rely=0.2, anchor="center")
        doc_menu(0)

    else:
        opcion=0
        vtn_apunte=tk.Toplevel(vtn_menu)
        vtn_apunte.geometry("480x340")
        vtn_apunte.title(tema)
        ###Widgets
        txt_apunte=tk.Text(vtn_apunte,width=40, height=20)
        btn_atras=tk.Button(vtn_apunte, text="Atras", command=doc_atras)
        btn_adelante=tk.Button(vtn_apunte, text="Siguiente", command=doc_adelante)

        ###Poner widgets
        txt_apunte.grid(column=0, row=0, columnspan=40, rowspan=20)
        btn_atras.grid(column=5, row= 30)
        btn_adelante.grid(column=10, row=30)

        ###Insertar texto
def doc_menu(boton):
    if boton==0:
        print("Aun no")
    elif boton==2:
        lbl_elegir.config(text="Temones")
    elif boton==1:
        lbl_elegir.config(text="Practiconas")
    lbl_elegir.after(300, doc_menu, docs_var.get())


def doc_adelante():
    print("Adelante")

def doc_atras():
    print("Atras")


def seno():
    print("Saca un seno")
    global  signal
    signal="Senoidal"
    texto=f"Senoidal {frec_gen}Hz"
    lbl_generador.config(text=texto)

def triangular():
    global signal
    signal="Triangular"
    texto=f"Triangular {frec_gen}Hz"
    print("Soy un triangulo :)")
    lbl_generador.config(text=texto)

def sqr_w():
    global signal
    signal="Cuadrada"
    print("Cuadrado")
    texto=f"Cuadrada {frec_gen}Hz"
    lbl_generador.config(text=texto)

def frec_aumentar():
    global frec_gen
    frec_gen+= 10
    texto=f"{signal} {frec_gen}Hz"
    lbl_generador.config(text=texto)

def frec_bajar():
    global frec_gen
    frec_gen-=10
    texto=f"{signal} {frec_gen}Hz"
    lbl_generador.config(text=texto)

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
