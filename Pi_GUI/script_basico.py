import tkinter as tk
import funciones
from funciones import enviar_comando

vtn_test=tk.Tk()
vtn_test.geometry("480x340")
vtn_test.title("No")

def r():
    enviar_comando(b'r')
def v():
    enviar_comando(b'v')
def i():
    enviar_comando(b'i')
def p():
    enviar_comando(b'p')
def o():
    enviar_comando(b'o')
def c():
    enviar_comando(b'c')
def metter():
    enviar_comando(b'metter')
def awg():
    enviar_comando(b'awg')
def a():
    enviar_comando(b'w')
def w():
    enviar_comando(b'w')
def w1():
    enviar_comando(b'1')
def w2():
    enviar_comando(b'2')
def w3():
    enviar_comando(b'3')
def w4():
    enviar_comando(b'4')

r_btn=tk.Button(vtn_test, text="Lectura continua", command=r)
v_btn=tk.Button(vtn_test, text="Voltajon", command=v)
i_btn=tk.Button(vtn_test, text="COrriente yo", command=i)
p_btn=tk.Button(vtn_test, text="Potencia", command=p)
o_btn=tk.Button(vtn_test, text="Resistencia", command=o)
c_btn=tk.Button(vtn_test, text="Continuidad", command=c)
metter_btn=tk.Button(vtn_test, text="Multimetro", command=metter)
awg_btn=tk.Button(vtn_test, text="Generador", command=awg)
a_btn=tk.Button(vtn_test, text="Activar salida de generador", command=a)
w_btn=tk.Button(vtn_test, text="Forma de onda", command=w)
w1_btn=tk.Button(vtn_test, text="Senoidal", command=w1)
w2_btn=tk.Button(vtn_test, text="Triangular", command=w2)
w3_btn=tk.Button(vtn_test, text="cuadrado", command=w3)
w4_btn=tk.Button(vtn_test, text="cuad", command=w4)
r_btn.pack()
v_btn.pack()
i_btn.pack()
p_btn.pack()
o_btn.pack()
c_btn.pack()
metter_btn.pack()
awg_btn.pack()
a_btn.pack()
w_btn.pack()
w1_btn.pack()
w2_btn.pack()
w3_btn.pack()
w4_btn.pack()

vtn_test.mainloop()
