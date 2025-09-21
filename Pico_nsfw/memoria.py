####Memoria
from machine import Pin, I2C
import time

smb= I2C(0, scl=Pin(1), sda=Pin(0), freq=100000)
mem_val={"Tipo":0x0000, "Nombre":0x0040, "Puertos":0x0080}

def escribir_m(direccion, dato, mem):
    msb= direccion>>8 & 0xFF
    lsb= direccion & 0xFF
    
    smb.writeto(mem, bytes([msb, lsb]) + dato)
    time.sleep(10)

def leer_mem(direccion, mem):
    msb= direccion>>8 & 0xFF
    lsb= direccion&0xFF
    smb.writeto(mem, bytes([msb, lsb]))
    return smb.readfrom(mem, 15)

###print(smb.scan())
def limpiar_pag(direccion, mem):
    for i in range(0,63):
        escribir_m(direccion, '_', mem)

def crear_modulo(mem, nombre, puertos):
    
    escribir_m(mem_val["Tipo"], 'Memoria', mem)
    escribir_m(mem_val["Puertos"], puertos, mem)
    escribir_m(mem_val["Nombre"], nombre, mem)




