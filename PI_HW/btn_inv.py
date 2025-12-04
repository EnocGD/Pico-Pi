from gpiozero import Button
from evdev import UInput, ecodes as e
from signal import pause

##shift tab
def f_btnu():
    ui.write(e.EV_KEY, e.KEY_LEFTSHIFT, 1)
    ui.write(e.EV_KEY, e.KEY_TAB, 1)
    ui.syn()

def s_btnu():
    ui.write(e.EV_KEY, e.KEY_LEFTSHIFT, 0)
    ui.write(e.EV_KEY, e.KEY_TAB, 0)
    ui.syn()
##tab
def f_btnd():

    ui.write(e.EV_KEY, e.KEY_TAB, 1)
    ui.syn()

def s_btnd():
    ui.write(e.EV_KEY, e.KEY_TAB, 0)
    ui.syn()
###enter
def f_btnl():
    ui.write(e.EV_KEY, e.KEY_ENTER, 1)
    ui.syn()

def s_btnl():
    ui.write(e.EV_KEY, e.KEY_ENTER, 0)
    ui.syn()

##down
def f_btnr():
    ui.write(e.EV_KEY, e.KEY_DOWN, 1)
    ui.syn()

def s_btnr():
    ui.write(e.EV_KEY, e.KEY_DOWN, 0)
    ui.syn()

##space
def f_btns():
    ui.write(e.EV_KEY, e.KEY_SPACE, 1)
    ui.syn()

def s_btns():
    ui.write(e.EV_KEY, e.KEY_SPACE, 0)
    ui.syn()
##altf4
def f_btne():
    ui.write(e.EV_KEY, e.KEY_LEFTALT, 1)
    ui.write(e.EV_KEY, e.KEY_F4, 1)
    ui.syn()

def s_btne():
    ui.write(e.EV_KEY, e.KEY_LEFTALT, 0)
    ui.write(e.EV_KEY, e.KEY_F4, 0)
    ui.syn()

capabilities = {
    e.EV_KEY: [e.KEY_UP, e.KEY_ENTER, e.KEY_TAB, e.KEY_LEFTSHIFT, e.KEY_SPACE, e.KEY_LEFTALT, e.KEY_F4]}
try:
    ui = UInput(capabilities, name="MIteclado")
except PermissionError:
    print("Suda")
except FileNotFoundError:
    print("No tienes el ev")
print("Si se creo")
btn_up = Button(5, bounce_time=0.1)
btn_right = Button(6,bounce_time=0.1)
btn_down = Button(19, bounce_time=0.1)
btn_left = Button(26,bounce_time=0.1)
btn_enter = Button(20,bounce_time=0.1)
btn_space = Button(21,bounce_time=0.1)

btn_up.when_pressed = f_btnu
btn_up.when_released = s_btnu

btn_down.when_pressed = f_btnd
btn_down.when_released = s_btnd

btn_left.when_pressed = f_btnl
btn_left.when_released = s_btnl

btn_right.when_pressed = f_btnr
btn_right.when_released = s_btnr

btn_enter.when_pressed = f_btne
btn_enter.when_released = s_btne

btn_space.when_pressed = f_btns
btn_space.when_released = s_btns
try:
    pause()
except KeyboardInterrupt:
    print("Fuga")