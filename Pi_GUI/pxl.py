###Excel test
from secrets import token_bytes

import pandas as pd
import tkinter as tk
from tkinter import ttk
window_size="640x480"
xl_name="modulos.xlsx"
index_item={}

def crear_xl():
    nombre_mod=ent_nombre.get()
    numero_pts=int(ent_n_pts.get())
    tipo=txt_tipo.get("1.0", tk.END)
    nombre_ptos=txt_nombre_ptos.get("1.0", tk.END)
    max_v=float(ent_max_v.get())
    min_v=float(ent_min_v.get())
    docs=ent_doc.get()
    ###DataFrame
    datos=pd.DataFrame({"Numero de puertos":[numero_pts],
                        "Tipo de puertos":[tipo],
                        "Nombre de puertos":[nombre_ptos],
                        "Maximo Nivel de Voltaje":[max_v],
                        "Minimo Nivel de Voltaje":[min_v],
                        "Documento de referencia":[docs]})
    try:
        with pd.ExcelWriter('directorio.xlsx', mode='a', if_sheet_exists='replace') as writer:
            datos.to_excel(writer, sheet_name=nombre_mod, index=False)
    except:
        datos.to_excel('directorio.xlsx', sheet_name=nombre_mod, index=False)
    print("Archivo creado con exito")

'''
Creacion de excel con pandas
directorio=openpyxl.Workbook()
data={'Modulo':['Digitales','Analogico', 'Micros'],
      'Puertos':[4,6,1],
      "Tipo de Puerto":['DDDD','ADAAAA','Terminal'],
      "Mayor Nivel de Voltaje":[5,12,'na'],
      "Menor Nivel de Voltaje":[0,-12,'na']}
df=pd.DataFrame(data)
df.to_excel('primero.xlsx', index=False)
print("Creado")
'''
#df=pd.read_excel('primero.xlsx')
def leer_mod_xl(modulo):
    mod_info={}
    try:
        xl_file=pd.ExcelFile('directorio.xlsx')
        tipos=pd.read_excel(xl_file, sheet_name=modulo).iloc[0]["Tipo de puertos"]
        nombre_ptos=pd.read_excel(xl_file,sheet_name=modulo).iloc[0]["Nombre de puertos"]
        numero_ptos=pd.read_excel(xl_file, sheet_name=modulo).iloc[0]["Numero de puertos"]
        mod_info['nombre']=nombre_ptos.split('\n')
        mod_info['tipos']=tipos.split('\n')
        mod_info['numero']=int(numero_ptos)
        return mod_info
    except:
        print("No existe el archivo o el modulo no esta registrado")
        return None

def cambio():
    cambiar='COUT'
    nv=1
    try:
        item_id=index_item[cambiar]
        valor_nuevo=(cambiar, nv)
        arbol_vista.item(item_id, values=valor_nuevo)
    except KeyError:
        print("Hay un detalle")


def arbolito(headers, n_elementos):
    global index_item
    global arbol_vista
    arbolon=tk.Tk()
    arbolon.title("Prueba de arbol")
    arbolon.geometry("480x320")
    lista=list()
    for i in range(0,n_elementos):
        lista.append('na')
    columnas=('Puerto', 'Valor')
    nuevo_indice={}
    btn_act=tk.Button(arbolon, text="Actualizar", command=cambio)
    arbol_vista=ttk.Treeview(arbolon, columns=columnas, show='headings')
    arbol_vista.heading('Puerto', text="Nombre de puerto")
    arbol_vista.heading('Valor', text='Valor')
    for Puerto, Valor in zip(headers, lista):
        valores_tabla= (Puerto, Valor)
        item_id=arbol_vista.insert(parent='', index='end', values=valores_tabla)
        nuevo_indice[Puerto]=item_id
    index_item=nuevo_indice
    arbol_vista.pack()
    btn_act.pack()
    print(index_item)
    arbolon.mainloop()

def vtn_crear_xl(vtn_main):
    xl_vtn=tk.Toplevel(vtn_main)
    vtn_main.withdraw()
    xl_vtn.geometry(window_size)
    xl_vtn.title("Excel prueba")
    global ent_nombre
    global ent_n_pts
    global txt_tipo
    global ent_max_v
    global ent_min_v
    global ent_doc
    global txt_nombre_ptos
    lbl_mod=tk.Label(xl_vtn,text="Nombre del modulo")
    ent_nombre=tk.Entry(xl_vtn)

    lbl_n_pts=tk.Label(xl_vtn, text="Numero de puertos")
    ent_n_pts=tk.Entry(xl_vtn)

    lbl_nombre_ptos=tk.Label(xl_vtn, text="Nombre de los puertos")
    txt_nombre_ptos=tk.Text(xl_vtn, width=30, height=5)
    lbl_tipo=tk.Label(xl_vtn, text="Tipo de puertos")
    txt_tipo=tk.Text(xl_vtn, width=30, height=5)

    lbl_max_v=tk.Label(xl_vtn, text="Maximo nivel de voltaje")
    ent_max_v=tk.Entry(xl_vtn)

    lbl_min_v=tk.Label(xl_vtn, text="Minimo nivel de voltaje")
    ent_min_v=tk.Entry(xl_vtn)

    lbl_doc=tk.Label(xl_vtn, text="Archivo de referencia")
    ent_doc=tk.Entry(xl_vtn)

    btn_crea=tk.Button(xl_vtn, text="Crear archivo", command=crear_xl)

    lbl_mod.pack()
    ent_nombre.pack()
    lbl_n_pts.pack()
    ent_n_pts.pack()
    lbl_nombre_ptos.pack()
    txt_nombre_ptos.pack()
    lbl_tipo.pack()
    txt_tipo.pack()
    lbl_max_v.pack()
    ent_max_v.pack()
    lbl_min_v.pack()
    ent_min_v.pack()
    lbl_doc.pack()
    ent_doc.pack()
    btn_crea.pack()
    xl_vtn.mainloop()


def vtn_externa():
    vtn_aux=tk.Tk()
    vtn_aux.title("Crear excel")
    vtn_aux.geometry("480x340")
    vtn_aux.resizable(False,False)
    btn_fuga=tk.Button(vtn_aux, text="Vamos", command=lambda: vtn_crear_xl(vtn_aux))
    btn_fuga.pack()
    vtn_aux.mainloop()

