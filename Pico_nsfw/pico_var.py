from machine import Pin, I2C, ADC
import time
from ssd1306 import SSD1306_I2C
from ina219 import INA219



###Variables para I2C
I2C_SDA_PIN = 0
I2C_SCL_PIN = 1
I2C_BUS_ID = 0
OLED_ADDR = 0x3C
INA219_ADDR = 0x40

###Boton
BUTTON_PIN = 16    # GPIO pin connected to your push button
DEBOUNCE_DELAY_MS = 200 # Milliseconds for button debouncing
BUZZER_PIN = 17    # GPIO pin to control the NPN transistor for the buzzer

ADC_PIN = 26       # Pico GP26 (ADC0) for resistance measurement
###Parametros de pantalla
OLED_WIDTH = 128
OLED_HEIGHT = 64

###Parametros de referencia para multimetro
R_REF = 55000.0    # Your measured reference resistor in Ohms (55 kOhm)
V_SUPPLY = 3.3     # Pico's 3.3V OUT for the voltage divider
CONTINUITY_THRESHOLD_OHMS = 3.0 # Resistance threshold for continuity beep
OPEN_CIRCUIT_THRESHOLD_OHMS = 500000.0 # 500 kOhm

# --- ADC Averaging Configuration ---
NUM_ADC_SAMPLES = 100 # Number of samples to average for ADC readings

# --- Measurement Modes//Multimetr ---
MEASUREMENT_MODES = [
    ("Bus Voltage", "V", 2),
    ("Current", "mA", 2),
    ("Power", "mW", 2),
    ("Resistance", "Ohm", 1), # Unit will be adjusted in code (Ohm, kOhm, MOhm)
    ("Continuity", "", 0)     # New mode for continuity
]
current_mode_index = 0
last_button_press_time = 0
