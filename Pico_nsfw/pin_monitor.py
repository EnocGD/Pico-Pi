##Pin Monitor
from machine import ADC
from machine import Pin
import utime
import sys
pin_array_size=4
aIn0 = ADC(0)
mtr_clk=Pin(22,Pin.OUT)
clk_rst=Pin(21, Pin.OUT)
conv=3300/65535

def adcRead():
    mv=aIn0.read_u16()
    mv=mv*conv
    return mv

def a2d (mv):
    if mv<50:
        dV=0
    elif mv>50 and mv<3000 :
        dV='unknown'
    else:
        dV=1
    return dV


def mntr_clk(stop, pList,type):
    clk_rst.value(0)
    for i in range(0, stop):
        if type== 1:
            pList[i]=a2d(adcRead())
        else:
            pList[i]=adcRead()
        mtr_clk.value(1)
        utime.sleep_ms(1)
        mtr_clk.value(0)
        utime.sleep_ms(1)
    clk_rst.value(1)
    utime.sleep_ms(1)

def list_builder(sze):
    eList=list()
    for i in range (0, sze):
        eList.append(0)
    return eList

nanoPinList=list_builder(pin_array_size)
while True:
    mntr_clk(pin_array_size,nanoPinList,1)
    print(nanoPinList)
    utime.sleep_ms(100)