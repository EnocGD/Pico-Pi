import ast
import tkinter as tk
import time
from tkinter import IntVar
from tkinter import ttk
import fitz
from PIL import Image, ImageTk
import pandas as pd
import openpyxl

import pico_funciones as pico

import funciones
vtn_pdf=None
vtn_tabla_mod=None
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
p2=88
p3=96
p4=104
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
def leer_pdf(ruta,pagina):
    ###Vtn de lector
    global vtn_pdf
    global lbl_pdf
    vtn_pdf=tk.Toplevel(vtn_menu)
    vtn_pdf.geometry(pi_window_size)
    vtn_pdf.title("Lector PDF by CTM")
    vtn_pdf.attributes('-fullscreen', True)
    vtn_pdf.focus_set()
    vtn_pdf.resizable(False, False)
    vtn_pdf.config(bg="black")
    ###Primera hoja
    img_tk = ImageTk.PhotoImage(pdf2img("DummyPracticePPTX.pdf", 0))
    lbl_pdf = tk.Label(vtn_pdf, image=img_tk)
    lbl_pdf.image = img_tk
    ###Botones
    btn_atras=tk.Button(vtn_pdf, text="Atras", command=pdf_atras)
    btn_adelante=tk.Button(vtn_pdf, text="Adelante", command=pdf_adelante)
    ###Posicionamiento de botones
    btn_atras.place(relx=0.2, rely=0.9, anchor="center")
    btn_adelante.place(relx=0.8, rely=0.9, anchor="center")
    lbl_pdf.pack()
##############################################################################################################
###Botones PDF
def pdf_adelante():
    global pag_cont
    if pag_cont==pag_total:
        print("Ya no hay mas padrino")
    else:
        print("Dondevan")
        pag_cont+=1
        img_tk=ImageTk.PhotoImage(pdf2img("DummyPracticePPTX.pdf", pag_cont))
        lbl_pdf.config(image=img_tk)
        lbl_pdf.image=img_tk
##############################################################################################################
def pdf_atras():
    global pag_cont
    if pag_cont==0:
        print("No hay mas atras")
    else:
        print("Dondevan")
        pag_cont -= 1
        img_tk = ImageTk.PhotoImage(pdf2img("DummyPracticePPTX.pdf", pag_cont))
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
    global lbl_buscar
    global mods_listos
    global vtn_lect_mods

    vtn_lect_mods=tk.Toplevel(vtn_menu)
    vtn_lect_mods.geometry(pi_window_size)
    vtn_lect_mods.title("Leyendo modulongos")
    vtn_lect_mods.resizable(False, False)
    vtn_lect_mods.attributes('-fullscreen', True)
    vtn_lect_mods.focus_set()
    ###vtn_menu.withdraw()
    lbl_buscar=tk.Label(vtn_lect_mods,
                        text="Buscando...",
                        font=("Arial", 22))
    lbl_buscar.place(relx=0.5, rely=0.5, anchor='center')
    act_mods("Buscando")
##############################################################################################################
###Cuadricula con modulos
def aprender_quad():
    global vtn_modsq
    global quad_mod
    quad_mod=IntVar()
    vtn_modsq=tk.Toplevel(vtn_menu)
    #vtn_lect_mods.destroy()
    vtn_modsq.geometry(pi_window_size)
    vtn_modsq.title("Modulos listos")
    vtn_modsq.resizable(False, False)
    vtn_modsq.attributes('-fullscreen', True)
    vtn_modsq.focus_set()
    ##Widgets
    rdtbn_mod1=tk.Radiobutton(vtn_modsq, text=mods_listos[0],
                              variable=quad_mod, value=1,font=("Arial", 20))
    rdtbn_mod2=tk.Radiobutton(vtn_modsq, text=mods_listos[1],
                              variable=quad_mod, value=2,font=("Arial", 20))
    rdtbn_mod3=tk.Radiobutton(vtn_modsq, text=mods_listos[2],
                              variable=quad_mod, value=3,font=("Arial", 20))
    rdtbn_mod4=tk.Radiobutton(vtn_modsq, text=mods_listos[3],
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
    serial_t=None
    print("avel")
    print(modulo)
    if modulo!=0:
        try:
            xl_file=pd.ExcelFile('directorio.xlsx')
            tipos=pd.read_excel(xl_file, sheet_name=modulo).iloc[0]["Tipo de puertos"]
            nombre_ptos=pd.read_excel(xl_file,sheet_name=modulo).iloc[0]["Nombre de puertos"]
            numero_ptos=pd.read_excel(xl_file, sheet_name=modulo).iloc[0]["Numero de puertos"]
            ref_doc=pd.read_excel(xl_file, sheet_name=modulo).iloc[0]["Documento de referencia"]
            mod_info['nombre']=nombre_ptos.split('\n')
            mod_info['tipos']=tipos.split('\n')
            mod_info['numero']=int(numero_ptos)
            mod_info['doc']=ref_doc
            '''for puerto in mod_info['tipos']:
                if puerto=='Serie':
                    serial_t=1
            if serial_t==1:
                serial_terminal()
            else:'''
            if vtn_tabla_mod==None:
                leer_mod(mod_info)
            '''if vtn_pdf==None:
                leer_pdf(modulo, 0)'''
        except:
            print("Modulo no identificado")
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
    mods_l=mods_l[0::2]
    return mods_l

def leer_mod(mod_info):
    global index_item
    global tabla
    global vtn_tabla_mod
    vtn_tabla_mod=tk.Toplevel(vtn_modsq)
    vtn_tabla_mod.geometry(pi_window_size)
    vtn_tabla_mod.resizable(False,False)
    vtn_tabla_mod.title("Tabla")
    lista=list()
    for i in range(0,mod_info['numero']):
        lista.append('na')
    columnas=('Puerto', 'Valor')
    nuevo_indice={}
    tabla=ttk.Treeview(vtn_tabla_mod, columns=columnas, show='headings')
    tabla.heading('Puerto', text="Nombre de puerto")
    tabla.heading('Valor', text='Valor')
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
    if mod_info['tipos'][0]=='Serie':
        btn_serial=tk.Button(vtn_tabla_mod, text="Monitor", command=serial_terminal)
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
        lbl_buscar.after(5000, act_mods, pico.nop())
        modulongos=puertos_i2c()
        time.sleep(1)
        for elements in modulongos:
            funciones.enviar_comando(b'fm')
            time.sleep(1)
            funciones.enviar_comando(str(elements).encode('utf-8'))
            if elements==p1:
                mods_listos[0]=(funciones.recibir_lectura_nb().decode('utf-8', errors='ignore')).split("\xff")[0]
            elif elements==p2:
                mods_listos[1] = (funciones.recibir_lectura_nb().decode('utf-8', errors='ignore')).split("\xff")[0]
            elif elements==p3:
                mods_listos[2] = (funciones.recibir_lectura_nb().decode('utf-8', errors='ignore')).split("\xff")[0]
            elif elements==p4:
                mods_listos[3] = (funciones.recibir_lectura_nb().decode('utf-8', errors='ignore')).split("\xff")[0]
            else:
                mods_listos.append((funciones.recibir_lectura_nb()).decode('utf-8', errors='ignore')).split("\xff")[0]
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
    print("calale")
    ###Para añadir documentacion
    global f, nombre_entry, lbl_doc_status, mem_nombre_entry, file_entry,puerto_entry
    global doc_var
    doc_var= IntVar()
    try:
        f=open("modulos.txt", "r")
    except:
        f=None
    vtn_docu_crear=tk.Toplevel(vtn_menu)
    vtn_docu_crear.geometry(pi_window_size)
    vtn_docu_crear.title("Control de documentacion")
    ###widgets
    lbl_bienvenida=tk.Label(vtn_docu_crear, text="Bienvenido")
    lbl_nombre=tk.Label(vtn_docu_crear, text="Nombre del modulo")
    lbl_mem_nombre=tk.Label(vtn_docu_crear, text="Identificador en la memoria")
    lbl_puertos=tk.Label(vtn_docu_crear, text="Puertos: ")
    lbl_file=tk.Label(vtn_docu_crear, text="Archivo asociado")
    nombre_entry=tk.Entry(vtn_docu_crear, width=50)
    mem_nombre_entry=tk.Entry(vtn_docu_crear, width=50)
    file_entry=tk.Entry(vtn_docu_crear, width=50)
    puerto_entry=tk.Entry(vtn_docu_crear,width=50)
    btn_buscar=tk.Button(vtn_docu_crear, text="Buscar", command=buscar_ref)
    btn_crear=tk.Button(vtn_docu_crear, text="Crear..", command=crear_ref)
    lbl_doc_status = tk.Label(vtn_docu_crear, text="")
    rdbtn_practica=tk.Radiobutton(vtn_docu_crear, text="Practicas",
                                  variable=doc_var, value=1)
    rdbtn_tema=tk.Radiobutton(vtn_docu_crear, text="Temas",
                              variable=doc_var, value=2)
    ###poner widgets
    lbl_bienvenida.grid(column=5, row=0, columnspan=20, rowspan=2, padx=10)
    lbl_nombre.grid(column=0, row=2,  rowspan=2)
    nombre_entry.grid(column=0, row=4)
    lbl_mem_nombre.grid(column=0, row=6, rowspan=2)
    mem_nombre_entry.grid(column=0, row=8)
    lbl_file.grid(column=0, row=10, rowspan=2)
    file_entry.grid(column=0, row=12)
    lbl_puertos.grid(column=0, row=14)
    puerto_entry.grid(column=0, row=16)

    btn_buscar.place(relx=0.2, rely=0.9, anchor="center")
    btn_crear.place(relx=0.8, rely=0.9, anchor="center")
    rdbtn_tema.place(relx=0.5, rely=0.8, anchor="center")
    rdbtn_practica.place(relx=0.7, rely=0.8, anchor="center")
    lbl_doc_status.place(relx=0.4, rely=0.9, anchor="center")
    ##Posible añadir preview
##############################################################################################################
###Buscar archivos
def buscar_ref():
    print("Ahorita buscamos padre")
    if f:
        mod_g=open("modulos.txt", "r")
        mod_guardados=mod_g.readlines()
        for mods in mod_guardados:
            if mods==nombre_entry.get():
                lbl_doc_status.config(text="Ya existe")
                break
    else:
        lbl_doc_status.config(text="Aun no existe")
##############################################################################################################
###Crear archivos de referencia, por ahora solo cuenta con indice, falta crear temas y practicas
def crear_ref():
    print("gestiono")
    nombre=nombre_entry.get()
    memoria=mem_nombre_entry.get()
    archivo_ref=file_entry.get()
    puerto=puerto_entry.get()
    try:
        f=open("Modulos_registrados.txt", "r")
    except:
        f=None
        print("Aun no esta listo")
    if f:
        f.close()
        indice=open(indice_txt, "a")
        indice.write(f"\n{nombre}")
        indice.close()
        try:
            f=open(f"{nombre}.txt", "w")
        except:
            f=open(f"{nombre}", "x")
        f.writelines(f"{memoria}\n{archivo_ref}\n{puerto}")
        f.close()
    else:
        if (memoria and nombre and archivo_ref):
            indice=open(indice_txt, "x")
            indice.write(nombre)
            indice.close()
            archivo=open(f"{nombre}.txt", "x")
            archivo.writelines(f"{memoria}\n{archivo_ref}\n{puerto}")
            archivo.close()
            lbl_doc_status.config(text="Datos guardados")
        else:
            lbl_doc_status.config(text="Rellena los campos por favor")
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
    print("Dr profesor patricio")
    global vtn_instru_menu
    ###Creacion de ventana de menu instru
    vtn_instru_menu= tk.Toplevel(vtn_menu)
    vtn_instru_menu.geometry(pi_window_size)
    vtn_instru_menu.title("Menu de instrumentacion")
    vtn_instru_menu.resizable(False, False)
    vtn_instru_menu.attributes('-fullscreen', True)
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
    ###Posicionar widgets
    lbl_menu.grid(column=15, row=0, columnspan=15, rowspan=1, padx=5, pady=5)
    btn_multi.grid(column=10, row=1, columnspan=30, rowspan=10, padx=5, pady=5)
    btn_gen.grid(column=10, row=12, columnspan=30, rowspan=10, padx=5, pady=5)
    btn_osc.grid(column=10, row=22, columnspan=30, rowspan=10, padx=5, pady=5)
##############################################################################################################
###Multimetro
def multimetro():
    print("Hola aqui va el multimetro")
    global lbl_lectura
    global var
    vtn_multi=tk.Toplevel(vtn_instru_menu)
    vtn_multi.geometry(pi_window_size)
    vtn_multi.title("Multimetro")
    vtn_multi.resizable(False, False)
    vtn_multi.attributes('-fullscreen', True)
    vtn_multi.focus_set()
    var=IntVar()
    ###Widgets
    rb_Voltaje=tk.Radiobutton(vtn_multi, text="Voltaje",
                              variable=var, value=1)
    rb_Corriente=tk.Radiobutton(vtn_multi, text="Corriente",
                                variable=var, value=2)
    rb_resistencia=tk.Radiobutton(vtn_multi, text="Resistencia",
                                  variable=var, value=3)
    rb_continuidad=tk.Radiobutton(vtn_multi, text="Continuidad",
                                  variable=var, value=4)
    lbl_lectura=tk.Label(vtn_multi, text="Lectura", font=("Arial",30))

    btn_gen= tk.Button(vtn_multi, text="Generador", command=generador)
    btn_osc=tk.Button(vtn_multi, text="Osciloscopio", command=osciloscopio)
    btn_docs= tk.Button(vtn_multi, text="Apuntes", command=docs)
    ###Colocar widgets
    lbl_lectura.place(relx=0.5, rely=0.5, anchor='center')
    rb_Voltaje.place(relx=0.2, rely=0.8, anchor= 'center')
    rb_resistencia.place(relx=0.4, rely=0.8, anchor= 'center')
    rb_Corriente.place(relx=0.6, rely=0.8, anchor= 'center')
    rb_continuidad.place(relx=0.8, rely=0.8, anchor="center")
    btn_gen.place(relx=0.95, rely=0.1, anchor='ne')
    btn_osc.place(relx=0.95, rely=0.2, anchor='ne')
    btn_docs.place(relx=0.95, rely=0.3, anchor='ne')
    multi_lectura(4)
##############################################################################################################
###Selector de opciones del multimetro
def multi_lectura(boton):
    global multimetro_lect
    funciones.enviar_comando(b'metter')
    time.sleep(1)
    if multimetro_lect==0:
        funciones.enviar_comando(b's')
        time.sleep(0.5)
        funciones.enviar_comando(b'r')
        time.sleep(0.5)
        multimetro_lect=1
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
    lbl_lectura.after(500, multi_lectura, var.get())
##############################################################################################################
###Informacion del osciloscopio, probablemente se elimine y se use la ventana de pdf
def osciloscopio():
    print("Hola, soy scoopy")
    vtn_osc=tk.Toplevel(vtn_instru_menu)
    vtn_osc.geometry(pi_window_size)
    vtn_osc.title("Como usar scoopy")
    vtn_osc.resizable(False, False)
    vtn_osc.attributes('-fullscreen', True)
    vtn_osc.focus_set()
    txt_usame=tk.Text(vtn_osc, width=45, height=15)

    btn_multi=tk.Button(vtn_osc, text="Multimetro", command=multimetro)
    btn_gen=tk.Button(vtn_osc, text="Generador", command=generador)
    btn_docs=tk.Button(vtn_osc, text="Apuntes", command=docs)

    btn_gen.place(relx=0.95, rely=0.1, anchor='ne')
    btn_multi.place(relx=0.95, rely=0.2, anchor='ne')
    btn_docs.place(relx=0.95, rely=0.3, anchor='ne')
    txt_usame.grid(column=0, row=0)
    txt_usame.insert("1.0","Soy la documentacion")
##############################################################################################################
###Generador de funciones
def generador():
    ###La amplitud no se puede controlar
    global lbl_generador
    global frec_gen
    global signal
    global flecha_arriba
    global flecha_abajo
    print("Hola soy un generador")
    gen_vtn=tk.Toplevel(vtn_instru_menu)
    gen_vtn.geometry(pi_window_size)
    gen_vtn.title("Calibre 50")
    gen_vtn.resizable(False, False)
    gen_vtn.attributes('-fullscreen', True)
    gen_vtn.focus_set()
    ###Iconos
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
    ###Añadir iconos
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
##############################################################################################################
###Funciones del generador
def seno():
    print("Saca un seno")
    global  signal
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
    frec_gen+= 10
    frec=str(frec_gen)
    frec=frec.encode('utf-8')
    texto=f"{signal} {frec_gen}Hz"
    lbl_generador.config(text=texto)
    funciones.enviar_comando(b'f')
    time.sleep(1)
    funciones.enviar_comando(frec)

def frec_bajar():
    global frec_gen
    frec_gen-=10
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
    print("Aqui va la documentacion")
    global lbl_elegir
    if tema=="TBD":
        global docs_var
        docs_var=IntVar()
        vtn_docs_menu=tk.Toplevel(vtn_menu)
        vtn_docs_menu.geometry(pi_window_size)
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
        vtn_apunte.geometry(pi_window_size)
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
##############################################################################################################
###Rellena el menu con temas o practicas
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

######################################################################################################3
def serial_terminal():
    def enviar():
        mensaje=cb_practicas.get()
        mensaje=mensaje+'\n'
        serial_caja.insert(tk.END, mensaje)
        match mensaje:
            case 'P1':
                comandos=["Cambiar practica"]
            case 'P2':
                #Imprimir mensaje de terminal
                comandos=['c', 's', 'r', "Cambiar practica"]
            case 'P3':
                #Imprimir desde terminal
                comandos=['d', 's', 'c', 't', "Cambiar practica"]
            case 'P4':
                #Imprimir desde terminal
                comandos=['s', '+', '-']
            case _:
                print("Nada")

    practicas=['P1', 'P2', 'P3', 'P4', 'P5']
    vtn_serial=tk.Toplevel(vtn_menu)
    vtn_serial.geometry(pi_window_size)
    vtn_serial.title("Monitor serial")
    vtn_serial.resizable(False, False)
    vtn_serial.config(bg='black')

    serial_caja = tk.Text(vtn_serial, width=55, height=15)
    cb_practicas=ttk.Combobox(vtn_serial, values=practicas)
    cb_practicas.set("Elige una practica...")
    btn_enviar=tk.Button(vtn_serial,text="->", command=enviar)
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
    main_vtn.resizable(False, False)
    main_vtn.attributes('-fullscreen', True)
    main_vtn.focus_set()
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

def act_status(cont, cin):
    '''
    Establecer comunicacion con la Pico por UART
    I2C scan para identificar los componentes en la red i2c, memorias(modulos) y multi
    Verificar archivos correspondientes a los modulos
    '''
    if cin < cont:
        cin += 1
        lbl_status.config(text=str(cin))
        lbl_status.after(1000, act_status, cont, cin)
    else:
        lbl_status.config(text="Listo")
        crear_btn_inicio()

def crear_btn_inicio():
    btn_inicio=tk.Button(main_vtn,
                         text="Iniciar",
                         command=inicio)
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
    vtn_menu.attributes('-fullscreen', True)
    vtn_menu.geometry(pi_window_size)
    vtn_menu.resizable(False, False)

    vtn_menu.focus_set()
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
