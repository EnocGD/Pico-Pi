import tkinter as tk


def btn():
    lbl_test.config(text="oi")

def crea():
    otra_vtn=tk.Toplevel(f_vtn)
    otra_vtn.geometry(window_size)
    otra_vtn.title("Nueva")
    otra_vtn.focus_force()
window_size="480x320"

f_vtn=tk.Tk()
f_vtn.geometry(window_size)
f_vtn.title("Mamalon")

lbl_test=tk.Label(f_vtn, text="Bolas")
btn_test=tk.Button(f_vtn, text="calale", command=btn)
btn_crea=tk.Button(f_vtn, text="Nueva", command=crea)

lbl_test.pack()
btn_test.pack()
btn_crea.pack()

f_vtn.mainloop()