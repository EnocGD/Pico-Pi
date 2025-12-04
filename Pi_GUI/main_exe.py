#!/usr/bin/env python3
import ast
import tkinter as tk
import time
from tkinter import IntVar
from tkinter import ttk
import fitz
from PIL import Image, ImageTk
import pandas as pd
import openpyxl
import s_test
import pico_funciones as pico
import funciones
import pxl


tele_pico=None
vtn_pdf=None
vtn_tabla_mod=None
vtn_menu=None
vtn_instru_menu=None
vtn_multi=None
gen_vtn=None
vtn_lect_mods=None
vtn_tabla_mod=None
vtn_mods=None
vtn_serial=None
vtn_modsq=None
vtn_docs=None
generador=None
modulo=0
###Variables
global tema
multimetro_lect=0
pag_cont=0
pag_total=0
tema="TBD"
indice_txt="Modulos_registrados.txt"
indice_temas_txt="Temas.txt"
mods_listos=["Vacio", "Vacio", "Vacio", "Vacio"]
mem_val={"Tipo":0x0000, "Nombre":0x0040, "Puertos":0x0080}
p1=80
p2=81
p4=82
p3=83
mod_info={}
tabla_ref={}
pi_window_size="480x320+0+0"

###Funciones
"""
 __       _______   ______ .___________.  ______   .______      
|  |     |   ____| /      ||           | /  __  \  |   _  \     
|  |     |  |__   |  ,----'`---|  |----`|  |  |  | |  |_)  |    
|  |     |   __|  |  |         |  |     |  |  |  | |      /     
|  `----.|  |____ |  `----.    |  |     |  `--'  | |  |\  \----.
|_______||_______| \______|    |__|      \______/  | _| `._____|

.______    _______   _______                                    
|   _  \  |       \ |   ____|                                   
|  |_)  | |  .--.  ||  |__                                      
|   ___/  |  |  |  ||   __|                                     
|  |      |  '--'  ||  |                                        
| _|      |_______/ |__|                                        
"""

def cierre_apertura(ventana_actual, ventana_anterior):
    ventana_actual.destroy()
    ventana_anterior.deiconify()
def leer_pdf(ruta,pagina):
    ###Vtn de lector
    def cerrar_pdf():
        global vtn_pdf
        if vtn_instru_menu==None:
            cierre_apertura(vtn_pdf,vtn_tabla_mod)
        else:
            cierre_apertura(vtn_pdf, vtn_instru_menu)
        vtn_pdf=None
    global vtn_pdf
    global lbl_pdf
    vtn_menu.withdraw()
    vtn_pdf=tk.Toplevel(vtn_menu)
    vtn_pdf.geometry(pi_window_size)
    vtn_pdf.title("Lector PDF by CTM")
    vtn_pdf.focus_set()
    vtn_pdf.resizable(False, False)
    vtn_pdf.config(bg="black")
    vtn_pdf.update_idletasks()
    vtn_pdf.attributes('-fullscreen', True)
    vtn_pdf.protocol("WM_DELETE_WINDOW", cerrar_pdf)
    ###Primera hoja
    img_tk = ImageTk.PhotoImage(pdf2img(ruta, pagina))
    lbl_pdf = tk.Label(vtn_pdf, image=img_tk)
    lbl_pdf.image = img_tk
    ###Botones
    btn_atras=tk.Button(vtn_pdf, text="<", command=lambda :pdf_atras(ruta))
    btn_adelante=tk.Button(vtn_pdf, text=">", command=lambda :pdf_adelante(ruta))
    ###Posicionamiento de botones
    btn_atras.place(relx=0.2, rely=0.9, anchor="center")
    btn_adelante.place(relx=0.8, rely=0.9, anchor="center")
    btn_atras.focus_set()
    lbl_pdf.pack()
##############################################################################################################
###Botones PDF
def pdf_adelante(ruta):
    global pag_cont
    if pag_cont==pag_total:
        print("Ya no hay mas padrino")
    else:
        print("Dondevan")
        pag_cont+=1
        img_tk=ImageTk.PhotoImage(pdf2img(ruta, pag_cont))
        lbl_pdf.config(image=img_tk)
        lbl_pdf.image=img_tk
##############################################################################################################
def pdf_atras(ruta):
    global pag_cont
    if pag_cont==0:
        print("No hay mas atras")
    else:
        print("Dondevan")
        pag_cont -= 1
        img_tk = ImageTk.PhotoImage(pdf2img(ruta, pag_cont))
        lbl_pdf.config(image=img_tk)
        lbl_pdf.image = img_tk
##############################################################################################################
###Convertidor de PDF a imagen
def pdf2img(ruta, pag):
    global pag_total
    pdf = fitz.open(ruta)
    if pag_total == 0:
        pag_total = pdf.page_count - 1

    page = pdf[pag]
    target_w, target_h = 480, 340
    pix = page.get_pixmap(matrix=fitz.Matrix(target_w / page.rect.width, target_h / page.rect.height))
    img = Image.frombytes("RGB", [pix.width, pix.height], pix.samples)
    return img
##############################################################################################################
###Lector de modulos
def menu_aprender():
    vtn_menu.withdraw()
    global lbl_buscar
    global mods_listos
    global vtn_lect_mods
    vtn_lect_mods=tk.Toplevel(vtn_menu)
    vtn_lect_mods.geometry(pi_window_size)
    vtn_lect_mods.title("Leyendo modulongos")
    vtn_lect_mods.resizable(False, False)
    vtn_lect_mods.update_idletasks()
    vtn_lect_mods.attributes('-fullscreen', True)
    vtn_lect_mods.focus_set()
    ###vtn_menu.withdraw()
    lbl_buscar=tk.Label(vtn_lect_mods,
                        text="Leyendo modulos",
                        font=("Arial", 22))
    lbl_buscar.place(relx=0.5, rely=0.5, anchor='center')
    #act_mods("Buscando")
    lbl_buscar.after(1000, act_mods, "Buscando")
##############################################################################################################
###Cuadricula con modulos
def aprender_quad():
    def quad_cierre():
        global vtn_lect_mods
        global vtn_modsq
        vtn_modsq.destroy()
        vtn_menu.deiconify()
        vtn_lect_mods.destroy()
        vtn_lect_mods=None
        vtn_modsq=None
    global vtn_modsq
    global quad_mod
    vtn_lect_mods.withdraw()
    quad_mod=IntVar()
    vtn_modsq=tk.Toplevel(vtn_menu)
    #vtn_lect_mods.destroy()
    vtn_modsq.geometry(pi_window_size)
    vtn_modsq.title("Modulos listos")
    vtn_modsq.resizable(False, False)
    vtn_modsq.update_idletasks()
    vtn_modsq.attributes('-fullscreen', True)
    vtn_modsq.protocol("WM_DELETE_WINDOW", quad_cierre)
    vtn_modsq.focus_set()
    ##Widgets
    rdtbn_mod1=tk.Radiobutton(vtn_modsq, text=f"Slot 1\n{mods_listos[0]}",
                              variable=quad_mod, value=1,font=("Arial", 20))
    rdtbn_mod2=tk.Radiobutton(vtn_modsq, text=f"Slot 2\n{mods_listos[1]}",
                              variable=quad_mod, value=2,font=("Arial", 20))
    rdtbn_mod3=tk.Radiobutton(vtn_modsq, text=f"Slot 3\n{mods_listos[2]}",
                              variable=quad_mod, value=3,font=("Arial", 20))
    rdtbn_mod4=tk.Radiobutton(vtn_modsq, text=f"Slot 4\n{mods_listos[3]}",
                              variable=quad_mod, value=4, font=("Arial", 20))

    practicar(0)
    ###Posicionamiento de radiobutton
    rdtbn_mod1.place(relx=0.25, rely=0.25, anchor='center')
    rdtbn_mod2.place(relx=0.75, rely=0.25, anchor='center')
    rdtbn_mod3.place(relx=0.25, rely=0.75, anchor='center')
    rdtbn_mod4.place(relx=0.75, rely=0.75, anchor='center')
##############################################################################################################
###Seleccion de modulo
def practicar(modulo):
    global mod_info
    mod_info={}
    print(mods_listos)
    serial_t=None
    if modulo!=0:
        modulo = mods_listos[modulo - 1]
        try:

            xl_file=pd.ExcelFile('directorio.xlsx')
            tipos=pd.read_excel(xl_file, sheet_name=modulo).iloc[0]["Tipo de puertos"]
            nombre_ptos=pd.read_excel(xl_file,sheet_name=modulo).iloc[0]["Nombre de puertos"]
            numero_ptos=pd.read_excel(xl_file, sheet_name=modulo).iloc[0]["Numero de puertos"]
            ref_doc=pd.read_excel(xl_file, sheet_name=modulo).iloc[0]["Documento de referencia"]
            mod_info['nombre']=nombre_ptos.split('\n')
            mod_info['tipos']=tipos.split('\n')
            mod_info['numero']=numero_ptos

            mod_info['doc']=ref_doc
            '''for puerto in mod_info['tipos']:
                if puerto=='Serie':
                    serial_t=1
            if serial_t==1:
                serial_terminal()
            else:'''
            print("Si llega aqui")
            if vtn_tabla_mod==None:
                leer_mod(mod_info)
                print(modulo)
            '''if vtn_pdf==None:
                leer_pdf(modulo, 0)'''
        except:
            print("Aqui no es")
    if modulo==0:
        print("nada")
    elif modulo==1:
        print(mods_listos[0])

    elif modulo==2:
        print(mods_listos[1])
    elif modulo==3:
        print(mods_listos[2])
    elif modulo==4:
        print(mods_listos[3])
    vtn_modsq.after(300, practicar, quad_mod.get())
##############################################################################################################
def puertos_i2c():
    funciones.enviar_comando(b'ms')
    mods=funciones.recibir_lectura()
    mods = ast.literal_eval(mods)
    mods_l=list()
    for elements in mods:
        if elements > 70:
            mods_l.append(elements)
    #mods_l=mods_l[0::2]
    return mods_l

def leer_mod(mod_info):
    def tabla_cierre():
        global vtn_tabla_mod
        global vtn_modsq
        cierre_apertura(vtn_tabla_mod, vtn_menu)
        vtn_modsq.destroy()
        vtn_modsq=None
        vtn_tabla_mod=None
    global index_item
    global tabla
    global vtn_tabla_mod
    vtn_modsq.withdraw()
    vtn_tabla_mod=tk.Toplevel(vtn_modsq)
    vtn_tabla_mod.geometry(pi_window_size)
    vtn_tabla_mod.resizable(False,False)
    vtn_tabla_mod.title("Tabla")
    vtn_tabla_mod.update_idletasks()
    vtn_tabla_mod.attributes('-fullscreen', True)
    vtn_tabla_mod.protocol("WM_DELETE_WINDOW", tabla_cierre)
    vtn_tabla_mod.focus_set()
    lista=list()
    print("Aqui mero")

    for i in range(0,mod_info['numero']):
        lista.append('na')
    columnas=('Puerto', 'Valor')
    nuevo_indice={}
    tabla=ttk.Treeview(vtn_tabla_mod, columns=columnas, show='headings')
    tabla.heading('Puerto', text="Nombre de puerto")
    tabla.heading('Valor', text='Valor')
    print(mod_info)
    for Puerto, Valor in zip(mod_info['nombre'], lista):
        valores_tabla= (Puerto, Valor)
        item_id=tabla.insert(parent='', index='end', values=valores_tabla)
        nuevo_indice[Puerto]=item_id
    index_item=nuevo_indice
    btn_pdf=tk.Button(vtn_tabla_mod, text="Abrir documentacion", command=lambda :leer_pdf(mod_info['doc'],0))
    btn_instru=tk.Button(vtn_tabla_mod, text="Instrumentacion", command=instru)
    actualizar_tabla()
    btn_pdf.place(relx=0.2, rely=0.8, anchor='center')
    btn_instru.place(relx=0.5, rely=0.8, anchor='center')
    if mod_info['tipos'][-1]=='SERIE':
        btn_serial=tk.Button(vtn_tabla_mod, text="Monitor", command=serial_terminal_practica)
        btn_serial.place(relx=0.8, rely=0.8, anchor='center')
    tabla.pack()
    print(index_item)

def actualizar_tabla():
    numero = str(mod_info['numero'])
    funciones.enviar_comando(b'mux')
    time.sleep(0.1)
    tipo=mod_info['tipos'][0]
    nmodulo=str(quad_mod.get()-1)
    funciones.enviar_comando(numero.encode('utf-8'))
    time.sleep(0.05)
    funciones.enviar_comando(tipo.encode('utf-8'))
    time.sleep(0.05)
    funciones.enviar_comando(nmodulo.encode('utf-8'))
    lista=funciones.recibir_lectura().split(' ')
    if len(lista)> len(mod_info['nombre']):
        lista.pop()
    #lista=[1,0,1,0,1,0,1]
    print(type(lista))
    for nombre_puerto, nuevo_valor in zip(mod_info['nombre'], lista):
        try:
            item_id=index_item[nombre_puerto]
            actualizacion=(nombre_puerto, nuevo_valor)
            tabla.item(item_id,values=actualizacion)
        except KeyError:
           print("Algo fallo")
    vtn_tabla_mod.after(500, actualizar_tabla)
###Actualizar modulos
def act_mods(mods):
    '''
    I2C scan para confirmar que modulos hay conectados
    Verificar que existan los archivos correspondientes a los modulos incluidos
    Notificar si hay un modulo sin informacion
    '''
    global mods_listos
    #mods_listos=["Vacio", "Vacio", "Vacio", "Vacio"]
    if mods== "Buscando":
        lbl_buscar.config(text="Buscando")
        lbl_buscar.after(1000, act_mods, pico.nop())
        modulongos=puertos_i2c()
        time.sleep(1)
        for elements in modulongos:
            funciones.enviar_comando(b'fm')
            time.sleep(1)
            funciones.enviar_comando(str(elements).encode('utf-8'))
            if elements==p1:
                mods_listos[0]=(funciones.recibir_lectura_nb().decode('utf-8', errors='ignore')).split("\xff")[0].strip()
            elif elements==p2:
                mods_listos[1] = (funciones.recibir_lectura_nb().decode('utf-8', errors='ignore')).split("\xff")[0].strip()
            elif elements==p3:
                mods_listos[2] = (funciones.recibir_lectura_nb().decode('utf-8', errors='ignore')).split("\xff")[0].strip()
            elif elements==p4:
                mods_listos[3] = (funciones.recibir_lectura_nb().decode('utf-8', errors='ignore')).split("\xff")[0].strip()
            else:
                print("Nadota")
                #aprender_quad()
                #mods_listos.append((funciones.recibir_lectura_nb()).decode('utf-8', errors='ignore')).split("\xff")[0].strip()
            time.sleep(1)
    elif mods=="Esperando":
        lbl_buscar.config(text="Conectando...")


        lbl_buscar.after(3000, act_mods, mods_listos[1])
    else:
        lbl_buscar.config(text="Listo")
        aprender_quad()
##############################################################################################################
###Para ingresar informacion sobre modulos o temas
def tbd2():
    ##Registro de modulos, informacion extra
    pxl.vtn_crear_xl(vtn_menu)
    ##Posible añadir preview
##############################################################################################################
###Buscar archivos

##############################################################################################################
###Crear archivos de referencia, por ahora solo cuenta con indice, falta crear temas y practicas

##############################################################################################################
"""
 __  .__   __.      _______.___________..______       __    __  .___  ___.    
|  | |  \ |  |     /       |           ||   _  \     |  |  |  | |   \/   |    
|  | |   \|  |    |   (----`---|  |----`|  |_)  |    |  |  |  | |  \  /  |    
|  | |  . `  |     \   \       |  |     |      /     |  |  |  | |  |\/|  |    
|  | |  |\   | .----)   |      |  |     |  |\  \----.|  `--'  | |  |  |  |    
|__| |__| \__| |_______/       |__|     | _| `._____| \______/  |__|  |__|    

 _______ .__   __. .___________.    ___       ______  __    ______   .__   __.
|   ____||  \ |  | |           |   /   \     /      ||  |  /  __  \  |  \ |  |
|  |__   |   \|  | `---|  |----`  /  ^  \   |  ,----'|  | |  |  |  | |   \|  |
|   __|  |  . `  |     |  |      /  /_\  \  |  |     |  | |  |  |  | |  . `  |
|  |____ |  |\   |     |  |     /  _____  \ |  `----.|  | |  `--'  | |  |\   |
|_______||__| \__|     |__|    /__/     \__\ \______||__|  \______/  |__| \__|
"""
###Menu de instrumentacion
def instru():
    def ins_cierre():
        global vtn_instru_menu
        cierre_apertura(vtn_instru_menu, vtn_menu)
        vtn_instru_menu=None
    print("Dr profesor patricio")
    global vtn_instru_menu
    vtn_menu.withdraw()
    ###Creacion de ventana de menu instru
    vtn_instru_menu= tk.Toplevel(vtn_menu)
    vtn_instru_menu.title("Menu de instrumentacion")
    vtn_instru_menu.geometry(pi_window_size)
    vtn_instru_menu.resizable(False, False)
    vtn_instru_menu.update_idletasks()
    vtn_instru_menu.attributes('-fullscreen', True)
    vtn_instru_menu.protocol("WM_DELETE_WINDOW", ins_cierre)
    vtn_instru_menu.focus_set()
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
                      text="Elige un instrumento",
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
    ###Posicionar widgets
    lbl_menu.grid(column=15, row=0, columnspan=15, rowspan=1, padx=5, pady=5)
    btn_multi.grid(column=10, row=1, columnspan=30, rowspan=10, padx=5, pady=5)
    btn_gen.grid(column=10, row=12, columnspan=30, rowspan=10, padx=5, pady=5)
    btn_osc.grid(column=10, row=22, columnspan=30, rowspan=10, padx=5, pady=5)
##############################################################################################################
###Multimetro
def multimetro():
    def cerrar_multi():
        global vtn_multi
        cierre_apertura(vtn_multi, vtn_instru_menu)
        vtn_multi=None
    vtn_instru_menu.withdraw()
    print("Hola aqui va el multimetro")
    global lbl_lectura
    global vtn_multi
    global gen_vtn
    global var
    if vtn_multi==None:
        vtn_multi=tk.Toplevel(vtn_instru_menu)
        vtn_multi.geometry(pi_window_size)
        vtn_multi.title("Multimetro")
        vtn_multi.resizable(False, False)
        vtn_multi.update_idletasks()
        vtn_multi.attributes('-fullscreen', True)
        vtn_multi.focus_set()
        vtn_multi.protocol("WM_DELETE_WINDOW", cerrar_multi)
        var = IntVar()
        ###Widgets
        rb_Voltaje = tk.Radiobutton(vtn_multi, text="Voltaje",
                                    variable=var, value=1)
        rb_Corriente = tk.Radiobutton(vtn_multi, text="Corriente",
                                      variable=var, value=2)
        rb_resistencia = tk.Radiobutton(vtn_multi, text="Resistencia",
                                        variable=var, value=3)
        rb_continuidad = tk.Radiobutton(vtn_multi, text="Continuidad",
                                        variable=var, value=4)
        lbl_lectura = tk.Label(vtn_multi, text="Lectura", font=("Arial", 30))

        btn_gen = tk.Button(vtn_multi, text="Generador", command=generador)
        btn_osc = tk.Button(vtn_multi, text="Osciloscopio", command=osciloscopio)
        btn_docs = tk.Button(vtn_multi, text="Apuntes", command=docs)
        ###Colocar widgets
        lbl_lectura.place(relx=0.5, rely=0.5, anchor='center')
        rb_Voltaje.place(relx=0.2, rely=0.8, anchor='center')
        rb_resistencia.place(relx=0.4, rely=0.8, anchor='center')
        rb_Corriente.place(relx=0.6, rely=0.8, anchor='center')
        rb_continuidad.place(relx=0.8, rely=0.8, anchor="center")
        btn_gen.place(relx=0.95, rely=0.1, anchor='ne')
        btn_osc.place(relx=0.95, rely=0.2, anchor='ne')
        btn_docs.place(relx=0.95, rely=0.3, anchor='ne')
        multi_lectura(4)
    else:
        vtn_multi.deiconify()
        if gen_vtn!=None:
            gen_vtn.withdraw()

##############################################################################################################
###Selector de opciones del multimetro
def multi_lectura(boton):
    global multimetro_lect
    global btn_anterior
    btn_anterior=4
    funciones.enviar_comando(b'metter')
    time.sleep(1)
    if multimetro_lect==0:
        funciones.enviar_comando(b's')
        time.sleep(0.5)
        funciones.enviar_comando(b'r')
        time.sleep(0.5)
        multimetro_lect=1
    if btn_anterior== boton:
        print("No")
    else:
        lbl_lectura.config(text="Cargando...")
        btn_anterior=boton
    if boton==1:
        funciones.enviar_comando(b'v')
        time.sleep(0.1)
        lectura=funciones.recibir_lectura()
        lbl_lectura.config(text=lectura)

    elif boton == 3:
        funciones.enviar_comando(b'o')
        time.sleep(0.1)
        lectura=funciones.recibir_lectura()
        lbl_lectura.config(text=lectura)
    elif boton==2:

        funciones.enviar_comando(b'i')
        time.sleep(0.1)
        lectura=funciones.recibir_lectura()
        lbl_lectura.config(text=lectura)
    elif boton==4:
        lbl_lectura.config(text="Continuidad")
        funciones.enviar_comando(b'c')
    else:
        lbl_lectura.config(text="Elige que medir")
    lbl_lectura.after(100, multi_lectura, var.get())
##############################################################################################################
###Informacion del osciloscopio, probablemente se elimine y se use la ventana de pdf
def osciloscopio():
    print("Hola, soy scoopy")
    scopy_pdf='DummyPractice.pdf'
    leer_pdf(scopy_pdf, 0)
##############################################################################################################

def generador():
    def apagar_gen():
        global generador
        if generador==None:
            seno()
            generador=1
        funciones.enviar_comando("a".encode('utf-8'))
    def cerrar_generador():
        global gen_vtn
        cierre_apertura(gen_vtn, vtn_instru_menu)
        gen_vtn=None
    vtn_instru_menu.withdraw()
    ###La amplitud no se puede controlar
    global lbl_generador
    global frec_gen
    global signal
    global flecha_arriba
    global flecha_abajo
    global on_off
    global sin
    global tri
    global quad
    global gen_vtn
    global escala
    escala=IntVar()
    print("Hola soy un generador")
    if gen_vtn==None:
        gen_vtn=tk.Toplevel(vtn_instru_menu)
        gen_vtn.geometry(pi_window_size)
        gen_vtn.title("Calibre 50")
        gen_vtn.resizable(False, False)
        gen_vtn.update_idletasks()
        gen_vtn.attributes('-fullscreen', True)
        gen_vtn.focus_set()
        gen_vtn.protocol("WM_DELETE_WINDOW", cerrar_generador)
        ###Iconos
        flecha_arriba=tk.PhotoImage(file="angulo-pequeno-hacia-arriba.png")
        flecha_abajo=tk.PhotoImage(file="angulo-hacia-abajo.png")
        on_off=tk.PhotoImage(file="boton-de-encendido.png")
        sin=tk.PhotoImage(file="sin.png")
        tri=tk.PhotoImage(file="triangulo-de-onda.png")
        quad=tk.PhotoImage(file="cuadrada_sen.png")
        ##Widgets
        lbl_generador= tk.Label(gen_vtn,
                                text="Generador",
                                font=("Arial", 20))
        lbl_frecuencia_UD=tk.Label(gen_vtn,
                                   text="Ajuste de frecuencia",
                                   font=("Arial", 14))
        btn_sin_sel=tk.Button(gen_vtn, text="Sin", command=seno)
        btn_tri_sel=tk.Button(gen_vtn, text="Triangular", command=triangular)
        btn_sqr_sel=tk.Button(gen_vtn, text="Cuadrada", command=sqr_w)
        lbl_tmp=tk.Label(gen_vtn, text="aguantala", font=("Arial", 10))
        btn_multi=tk.Button(gen_vtn, text="Multimetro", command=multimetro, width=10)
        btn_osc=tk.Button(gen_vtn, text="Osciloscopio", command=osciloscopio, width=10)
        btn_docs=tk.Button(gen_vtn, text="Apuntes", command=docs, width=10)
        rbtn_100=tk.Radiobutton(gen_vtn, text="x100Hz", variable=escala, value=100)
        rbtn_1k=tk.Radiobutton(gen_vtn, text="x1kHZ", variable=escala, value=1000)
        rbtn_10k = tk.Radiobutton(gen_vtn, text="x10kHz", variable=escala, value=10000)
        btn_subir=tk.Button(gen_vtn, command=frec_aumentar)
        btn_bajar=tk.Button(gen_vtn, command=frec_bajar)
        btn_apagar_gen = tk.Button(gen_vtn, command=apagar_gen)
        ###Añadir iconos
        btn_apagar_gen.config(image=on_off, compound=tk.LEFT)
        btn_subir.config(image=flecha_arriba, compound=tk.TOP)
        btn_bajar.config(image=flecha_abajo, compound=tk.TOP)
        btn_sin_sel.config(image=sin, compound=tk.LEFT)
        btn_sqr_sel.config(image=quad, compound=tk.LEFT)
        btn_tri_sel.config(image=tri, compound=tk.LEFT)
        frec_gen=1000
        signal="Senoidal"
        ##Poner widgets
        #lbl_generador.grid(column=10, row=5, columnspan=20, rowspan=10)
        #btn_sqr_sel.grid(column=0, row= 30, columnspan=10, rowspan=2, padx=2)
        #btn_sin_sel.grid(column=10, row=30, columnspan=10, rowspan=2, padx=2)
        #btn_tri_sel.grid(column=20, row=30, columnspan=10, rowspan=2, padx=2)
        #btn_subir.grid(column=0, row=35, columnspan=2)
        #btn_bajar.grid(column=3, row=35, columnspan=2)
        lbl_generador.place(relx=0.45, rely=0.25, anchor='center')
        btn_apagar_gen.place(relx=0.1, rely=0.25, anchor='center')
        lbl_frecuencia_UD.place(relx=0.35, rely=0.35, anchor='center')
        rbtn_100.place(relx=0.1, rely=0.45, anchor='center')
        rbtn_1k.place(relx=0.3, rely=0.45, anchor="center")
        rbtn_10k.place(relx=0.5, rely=0.45, anchor='center')
        btn_subir.place(relx=0.65, rely=0.45, anchor='center')
        btn_bajar.place(relx=0.7, rely=0.45, anchor='center')
        btn_sqr_sel.place(relx=0.2, rely=0.6, anchor='w')
        btn_sin_sel.place(relx=0.2, rely=0.7, anchor='w')
        btn_tri_sel.place(relx=0.2, rely=0.8, anchor='w')
        btn_multi.place(relx=0.2, rely=0.1, anchor="center")
        btn_osc.place(relx=0.5, rely=0.1, anchor="center")
        btn_docs.place(relx=0.75, rely=0.1, anchor="center")
    else:
        gen_vtn.deiconify()
        if vtn_multi!=None:
            vtn_multi.withdraw()
##############################################################################################################
###Funciones del generador
def seno():
    print("Saca un seno")
    global  signal
    global generador
    generador=1
    frec=str(frec_gen)
    frec=frec.encode('utf-8')
    signal="Senoidal"
    texto=f"Senoidal {frec_gen}Hz"
    lbl_generador.config(text=texto)
    funciones.enviar_comando(b'awg')
    time.sleep(1)
    funciones.enviar_comando(b'w')
    time.sleep(1)
    funciones.enviar_comando(b'0')
    time.sleep(1)
    funciones.enviar_comando(b'f')
    time.sleep(1)
    funciones.enviar_comando(frec)

def triangular():
    global signal
    global generador
    generador=1
    signal="Triangular"
    texto=f"Triangular {frec_gen}Hz"
    print("Soy un triangulo :)")
    lbl_generador.config(text=texto)
    frec=str(frec_gen)
    frec=frec.encode('utf-8')
    funciones.enviar_comando(b'awg')
    time.sleep(1)
    funciones.enviar_comando(b'w')
    time.sleep(1)
    funciones.enviar_comando(b'1')
    time.sleep(1)
    funciones.enviar_comando(b'f')
    time.sleep(1)
    funciones.enviar_comando(frec)

def sqr_w():
    global generador
    generador=1
    frec=str(frec_gen)
    frec=frec.encode('utf-8')
    global signal
    signal="Cuadrada"
    print("Cuadrado")
    texto=f"Cuadrada {frec_gen}Hz"
    lbl_generador.config(text=texto)
    funciones.enviar_comando(b'awg')
    time.sleep(1)
    funciones.enviar_comando(b'w')
    time.sleep(1)
    funciones.enviar_comando(b'2')
    time.sleep(1)
    funciones.enviar_comando(b'f')
    time.sleep(1)
    funciones.enviar_comando(frec)

def frec_aumentar():
    global frec_gen

    frec_gen+= escala.get()
    frec=str(frec_gen)
    frec=frec.encode('utf-8')
    texto=f"{signal} {frec_gen}Hz"
    lbl_generador.config(text=texto)
    funciones.enviar_comando(b'f')
    time.sleep(1)
    funciones.enviar_comando(frec)

def frec_bajar():
    global frec_gen
    frec_gen-=escala.get()
    frec=str(frec_gen)
    frec=frec.encode('utf-8')
    texto=f"{signal} {frec_gen}Hz"
    lbl_generador.config(text=texto)
    funciones.enviar_comando(b'f')
    time.sleep(1)
    funciones.enviar_comando(frec)
##############################################################################################################
###Menu de documentacion
def docs():
    '''
    Indice de temas y practicas
    Combobox o lista
    Modificar, podria hacer con dos ventanas, una de eleccion de tema y despues ir al lector PDF
    '''
    def cerrar_doc():
        global vtn_docs
        cierre_apertura(vtn_docs, vtn_menu)
        vtn_docs=None

    def abrir_archivo(event):
        try:
            item_sel=documentos.selection()[0]
        except IndexError:
            return
        print(item_sel)
        direccion=pd.read_excel(file_xl, sheet_name=item_sel).iloc[0]["Documento de referencia"]
        print(direccion)
        leer_pdf(direccion, 0)

    def sel_item(event):
        elemento_actual = documentos.focus()
        children_id = documentos.get_children()
        if not children_id:
            return
        first_item = children_id[0]
        u_item = children_id[-1]
        if elemento_actual == u_item:
            print(first_item)
            documentos.selection_set(first_item)
            documentos.focus(first_item)
            documentos.see(first_item)
            return "break"
    global vtn_docs
    global documentos
    vtn_menu.withdraw()
    vtn_docs=tk.Toplevel(vtn_menu)
    vtn_docs.geometry("480x320")
    vtn_docs.title("Documentos")
    vtn_docs.resizable(False,False)
    vtn_docs.update_idletasks()
    vtn_docs.attributes('-fullscreen', True)
    documentos=ttk.Treeview(vtn_docs, takefocus=1)
    documentos.bind('<Return>', abrir_archivo)
    documentos.bind('<Down>', sel_item)
    lbl_info=tk.Label(vtn_docs, text="Aqui encontraras"
                                     " informacion sobre"
                                     " las practicas")
    lbl_info.place(relx=0.5, rely=0.2, anchor='center')
    documentos.place(relx=0.5,rely=0.6, anchor='center')
    vtn_docs.protocol("WM_DELETE_WINDOW", cerrar_doc)
    documentos.focus_set()
    referencia='directorio.xlsx'
    try:
        file_xl=pd.ExcelFile(referencia)
        practicas=file_xl.sheet_names
    except FileNotFoundError:
        print("No se encontro archivo de referencia")
    except Exception as e:
        print("Error al procesar archivo")
    for elements in practicas:
        documentos.insert(parent="", index="end", iid=elements, text=elements)
    documentos.selection_set(practicas[0])
    documentos.focus(practicas[0])
    documentos.see(practicas[0])
    documentos.focus_set()
##############################################################################################################

######################################################################################################3
def serial_terminal_practica():
    def enviar():
        global comandos
        mensaje=cb_practicas.get()
        match mensaje:
            case '1':
                comandos=['rapido', 'normal', 'lento', 'estado', 'ayuda', 'salir']

            case '2':
                #Imprimir mensaje de terminal
                comandos=['estado', 'ayuda', 'salir']
            case '3':
                #Imprimir desde terminal
                comandos=['centrar', 'probar', 'estado', 'ayuda', 'salir']
            case '4':
                #Imprimir desde terminal
                comandos=['modo', 'rojo', 'verde', 'azul', 'blanco', 'apagar', 'probar', 'estado',
                          'ayuda', 'salir']
            case '5':
                comandos=['establecer 14', 'limpiar', 'ceros', 'probar', 'contar', 'diagnostico',
                          'estado', 'ayuda', 'salir']
            case 'salir':
                comandos=['1', '2', '3', '4', '5', 'info']
            case 'info':
                comandos=['1', '2', '3', '4', '5', 'info']
            case _:
                comandos=comandos
        mensaje=mensaje+'\n'
        serial_caja.insert(tk.END, mensaje)
        s_test.arduino_escribe(mensaje, arduino)
        cb_practicas['values']=comandos
        cb_practicas.set('Elige una opcion...')


    def act_terminal(arduino):
        mensaje=s_test.arduino_leer(arduino)
        if mensaje:
            serial_caja.insert(tk.END, f"{mensaje}\n")
            serial_caja.see(tk.END)
        serial_caja.after(100, act_terminal,arduino)

    def ciclo_opciones(event):
        print("Lo presionaste")
        cb=event.widget
        current_index=cb_practicas.current()
        list_size=len(cb['values'])
        if current_index== (list_size - 1):
            cb_practicas.current(0)
        else:
            cb_practicas.current(current_index+1)

        return "break"
    def serial_cierre():
        global vtn_serial
        cierre_apertura(vtn_serial, vtn_tabla_mod)
        vtn_serial=None

    practicas=['1', '2', '3', '4', '5', 'info']
    global comandos
    comandos=[]
    vtn_menu.withdraw()
    vtn_serial=tk.Toplevel(vtn_menu)
    vtn_serial.geometry(pi_window_size)
    vtn_serial.title("Monitor serial")
    vtn_serial.resizable(False, False)
    vtn_serial.update_idletasks()
    vtn_serial.attributes('-fullscreen', True)
    arduino=s_test.config_terminal()
    vtn_serial.protocol("WM_DELETE_WINDOW", serial_cierre)
    serial_caja = tk.Text(vtn_serial, width=55, height=15, takefocus=0)
    cb_practicas=ttk.Combobox(vtn_serial, values=practicas)
    cb_practicas.set("Elige una practica...")
    cb_practicas.bind('<Down>', ciclo_opciones)
    btn_enviar=tk.Button(vtn_serial,text="->", command=enviar)
    act_terminal(arduino)
    serial_caja.pack()
    cb_practicas.pack()
    cb_practicas.focus_set()
    btn_enviar.pack(after=cb_practicas)


def serial_terminal():
    def enviar():
        global comandos

        mensaje=cb_practicas.get()
        match mensaje:
            case '1':
                comandos=['rapido', 'normal', 'lento', 'estado', 'ayuda', 'salir']

            case '2':
                #Imprimir mensaje de terminal
                comandos=['estado', 'ayuda', 'salir']
            case '3':
                #Imprimir desde terminal
                comandos=['centrar', 'probar', 'estado', 'ayuda', 'salir']
            case '4':
                #Imprimir desde terminal
                comandos=['modo', 'rojo', 'verde', 'azul', 'blanco', 'apagar', 'probar', 'estado',
                          'ayuda', 'salir']
            case '5':
                comandos=['establecer 14', 'limpiar', 'ceros', 'probar', 'contar', 'diagnostico',
                          'estado', 'ayuda', 'salir']
            case 'salir':
                comandos=['1', '2', '3', '4', '5', 'info']
            case _:
                comandos=comandos
        mensaje=mensaje+'\n'
        serial_caja.insert(tk.END, mensaje)
        s_test.arduino_escribe(mensaje, arduino)
        cb_practicas['values']=comandos
        cb_practicas.set('Elige una opcion...')


    def act_terminal(arduino):
        mensaje=s_test.arduino_leer(arduino)
        if mensaje:
            serial_caja.insert(tk.END, f"{mensaje}\n")
            serial_caja.see(tk.END)
        serial_caja.after(100, act_terminal,arduino)

    practicas=['1', '2', '3', '4', '5', 'info']
    global comandos
    comandos=[]
    vtn_serial=tk.Toplevel(vtn_menu)
    vtn_serial.geometry(pi_window_size)
    vtn_serial.title("Monitor serial")
    vtn_serial.resizable(False, False)
    vtn_serial.config(bg='black')
    arduino=s_test.config_terminal()

    serial_caja = tk.Text(vtn_serial, width=55, height=15)
    cb_practicas=ttk.Combobox(vtn_serial, values=practicas)
    cb_practicas.set("Elige una practica...")
    btn_enviar=tk.Button(vtn_serial,text="->", command=enviar)
    act_terminal(arduino)
    serial_caja.pack()
    cb_practicas.pack()
    btn_enviar.pack(after=cb_practicas)
"""
 __  .__   __.  __    ______  __    ______  
|  | |  \ |  | |  |  /      ||  |  /  __  \ 
|  | |   \|  | |  | |  ,----'|  | |  |  |  |
|  | |  . `  | |  | |  |     |  | |  |  |  |
|  | |  |\   | |  | |  `----.|  | |  `--'  |
|__| |__| \__| |__|  \______||__|  \______/ 
"""
def bienvenida():
    global main_vtn
    global lbl_status
    main_vtn = tk.Tk()
    stats = "Cargando pico"
    ##Imagenes
    logo = tk.PhotoImage(file="logo.png")

    main_vtn.geometry(pi_window_size)
    main_vtn.title("Bienvenida")
    main_vtn.config(bg='white')
    main_vtn.resizable(False, False)
    main_vtn.attributes('-fullscreen', True)
    main_vtn.focus_set()
    ###Etiquetas
    lbl_bienvenido = tk.Label(main_vtn,
                              text="BIENVENIDO",
                              font=("Arial", 16),
                              bg='white')
    lbl_status = tk.Label(main_vtn,
                          text=stats,
                          font=("Arial", 12),
                          bg='white')
    lbl_blogo = tk.Label(image=logo)
    ###Posicionamiento de widgets
    lbl_bienvenido.grid(column=10, row=1, columnspan=3, rowspan=3, padx=180)
    lbl_blogo.grid(column=10, row=5, padx=90)
    lbl_status.grid(column=10, row=100, padx=100)

    act_status("Cargando..")

    main_vtn.mainloop()

def act_status(paso):
    '''
    Establecer comunicacion con la Pico por UART
    I2C scan para identificar los componentes en la red i2c, memorias(modulos) y multi
    Verificar archivos correspondientes a los modulos
    '''
    global tele_pico
    lbl_status.config(text=paso)
    if tele_pico==None:
        funciones.enviar_comando(b'hi')
        tele_pico=funciones.recibir_lectura()
        lbl_status.after(1000, act_status, tele_pico)
    else:
        time.sleep(2)
        lbl_status.config(text="Sistema listo...")
        crear_btn_inicio()

def crear_btn_inicio():
    btn_inicio=tk.Button(main_vtn,
                         text="Iniciar",
                         command=inicio,
                         bg='white')
    btn_inicio.grid(column=10, row=102)

def inicio():
    print("Continuamooos")
    crear_main_vtn()
"""
.___  ___.  _______ .__   __.  __    __                                  
|   \/   | |   ____||  \ |  | |  |  |  |                                 
|  \  /  | |  |__   |   \|  | |  |  |  |                                 
|  |\/|  | |   __|  |  . `  | |  |  |  |                                 
|  |  |  | |  |____ |  |\   | |  `--'  |                                 
|__|  |__| |_______||__| \__|  \______/                                  

.______   .______       __  .__   __.   ______  __  .______      ___     
|   _  \  |   _  \     |  | |  \ |  |  /      ||  | |   _  \    /   \    
|  |_)  | |  |_)  |    |  | |   \|  | |  ,----'|  | |  |_)  |  /  ^  \   
|   ___/  |      /     |  | |  . `  | |  |     |  | |   ___/  /  /_\  \  
|  |      |  |\  \----.|  | |  |\   | |  `----.|  | |  |     /  _____  \ 
| _|      | _| `._____||__| |__| \__|  \______||__| | _|    /__/     \__\
"""
def crear_main_vtn():
    main_vtn.withdraw()
    #main_vtn.destroy()
    global vtn_menu
    vtn_menu=tk.Toplevel(main_vtn)
    vtn_menu.title("Menu principal")
    vtn_menu.geometry(pi_window_size)
    vtn_menu.resizable(False, False)
    vtn_menu.update_idletasks()
    vtn_menu.attributes('-fullscreen', True)

    vtn_menu.focus_set()
    ##Imagenes
    ajalas_icon = tk.PhotoImage(file="base-de-datos.png")
    docs_icon= tk.PhotoImage(file="archivo.png")
    aprender_icon= tk.PhotoImage(file="aprendiendo.png")
    instru_icon=tk.PhotoImage(file="voltimetro.png")
    ##Widgets
    btn_aprender=tk.Button(vtn_menu,
                       text="Aprendamos",
                       command=menu_aprender,width=220, height=150)
    btn_tbd2=tk.Button(vtn_menu,
                       text="Registro de modulos",
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
