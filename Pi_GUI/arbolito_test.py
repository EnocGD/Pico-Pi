import tkinter as tk
import pandas as pd
import openpyxl

from tkinter import ttk
def item_sel(event):
    sel=tablita.selection()
    pdfile=pd.ExcelFile('directorio.xlsx')
    direccion=pd.read_excel(pdfile,sheet_name=sel[0]).iloc[0]["Documento de referencia"]
    print(sel[0])
    print(direccion)
def sel_item(event):
    global tablita
    elemento_actual=tablita.focus()
    children_id=tablita.get_children()
    if not children_id:
        return
    first_item=children_id[0]
    u_item=children_id[-1]
    if elemento_actual==u_item:
        print(first_item)
        tablita.selection_set(first_item)
        tablita.focus(first_item)
        tablita.see(first_item)
        return "break"
modulo_test=["Digitales", "OPAMP"]
vtn_arbol=tk.Tk()
vtn_arbol.title("Prueba de interraccion con arbolito")
vtn_arbol.geometry("640x480")
tablita=ttk.Treeview(vtn_arbol,takefocus=1)
tablita.bind('<Return>', item_sel)
tablita.bind('<Down>',sel_item)
tablita.pack()
for elements in modulo_test:
    tablita.insert(parent="", index="end", iid=elements, text=elements)
tablita.selection_set(modulo_test[0])
tablita.focus(modulo_test[0])
tablita.see(modulo_test[0])
tablita.focus_set()

vtn_arbol.mainloop()