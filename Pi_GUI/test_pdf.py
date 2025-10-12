###Prueba abriendo pdf
import fitz
from PIL import Image, ImageTk
import tkinter as tk


pag_cont=0
pag_total=0
def pdf2img(ruta, pag):
    global pag_total
    pdf=fitz.open(ruta)
    if pag_total==0:
        pag_total=pdf.page_count - 1

    page=pdf[pag]
    target_w, target_h= 480, 340
    pix=page.get_pixmap(matrix=fitz.Matrix(target_w/page.rect.width, target_h/page.rect.height))
    img=Image.frombytes("RGB", [pix.width, pix.height],pix.samples )
    return img


def adelante():
    global pag_cont
    if pag_cont==pag_total:
        print("Ya no hay mas padrino")
    else:
        print("Dondevan")
        pag_cont+=1
        img_tk=ImageTk.PhotoImage(pdf2img("DummyPracticePPTX.pdf", pag_cont))
        lbl_img.config(image=img_tk)
        lbl_img.image=img_tk
def atras():
    global pag_cont
    if pag_cont==0:
        print("No hay mas atras")
    else:
        print("Dondevan")
        pag_cont -= 1
        img_tk = ImageTk.PhotoImage(pdf2img("DummyPracticePPTX.pdf", pag_cont))
        lbl_img.config(image=img_tk)
        lbl_img.image = img_tk


vtn_pdf=tk.Tk()
vtn_pdf.geometry("480x340")
vtn_pdf.title("PDF")
img_tk=ImageTk.PhotoImage(pdf2img("DummyPracticePPTX.pdf", 0))
lbl_img=tk.Label(vtn_pdf, image=img_tk)
lbl_img.image=img_tk
btn_adelante=tk.Button(vtn_pdf, text="Siguiente", command=adelante)
btn_atras=tk.Button(vtn_pdf, text="Atras", command=atras)
lbl_img.pack()
btn_adelante.place(relx=0.25, rely=0.9, anchor="center")
btn_atras.place(relx=0.75, rely=0.9, anchor="center")

vtn_pdf.mainloop()