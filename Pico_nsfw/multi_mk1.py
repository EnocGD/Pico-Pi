# main.py for Raspberry Pi Pico - Multimeter with Mode Cycling, Continuity, and Improved Resistance Range

from machine import Pin, I2C, ADC
import time
from ssd1306 import SSD1306_I2C
from ina219 import INA219

# --- Hardware Configuration ---
I2C_SDA_PIN = 0
I2C_SCL_PIN = 1
ADC_PIN = 26       # Pico GP26 (ADC0) for resistance measurement
I2C_BUS_ID = 0
BUTTON_PIN = 16    # GPIO pin connected to your push button
DEBOUNCE_DELAY_MS = 200 # Milliseconds for button debouncing
BUZZER_PIN = 17    # GPIO pin to control the NPN transistor for the buzzer

###Parametros
OLED_WIDTH = 128
OLED_HEIGHT = 64
OLED_ADDR = 0x3C
INA219_ADDR = 0x40

R_REF = 55000.0    # Your measured reference resistor in Ohms (55 kOhm)
V_SUPPLY = 3.3     # Pico's 3.3V OUT for the voltage divider
CONTINUITY_THRESHOLD_OHMS = 3.0 # Resistance threshold for continuity beep
OPEN_CIRCUIT_THRESHOLD_OHMS = 500000.0 # 500 kOhm

# --- ADC Averaging Configuration ---
NUM_ADC_SAMPLES = 100 # Number of samples to average for ADC readings

# --- Measurement Modes ---
MEASUREMENT_MODES = [
    ("Bus Voltage", "V", 2),
    ("Current", "mA", 2),
    ("Power", "mW", 2),
    ("Resistance", "Ohm", 1), # Unit will be adjusted in code (Ohm, kOhm, MOhm)
    ("Continuity", "", 0)     # New mode for continuity
]
current_mode_index = 0
last_button_press_time = 0

# --- Initialize Hardware ---
print("Initializing I2C bus...")
try:
    i2c = I2C(I2C_BUS_ID, sda=Pin(I2C_SDA_PIN), scl=Pin(I2C_SCL_PIN), freq=400000)
    print(f"I2C initialized on bus {I2C_BUS_ID} (SDA: GP{I2C_SDA_PIN}, SCL: GP{I2C_SCL_PIN})")
except Exception as e:
    print(f"Error initializing I2C: {e}")
    print("Please check your I2C pin assignments and wiring.")
    while True:
        time.sleep(1)

print("Scanning I2C addresses...")
devices = i2c.scan()
print(f"Found I2C devices at addresses: {[hex(d) for d in devices]}")

if OLED_ADDR in devices:
    print(f"OLED detected at I2C address: 0x{OLED_ADDR:x}")
else:
    print(f"OLED NOT FOUND at expected address 0x{OLED_ADDR:x}. Please check wiring.")
    while True:
        time.sleep(1)

if INA219_ADDR in devices:
    print(f"INA219 detected at I2C address: 0x{INA219_ADDR:x}")
else:
    print(f"INA219 NOT FOUND at expected address 0x{INA219_ADDR:x}. Please check wiring.")
    while True:
        time.sleep(1)

print("Initializing OLED display...")
try:
    oled = SSD1306_I2C(OLED_WIDTH, OLED_HEIGHT, i2c, addr=OLED_ADDR)
    oled.fill(0)
    oled.show()
    print("OLED initialized successfully!")
except Exception as e:
    print(f"Error initializing OLED: {e}")
    print("This could be due to incorrect dimensions or a bad connection.")
    while True:
        time.sleep(1)

print("Initializing INA219 sensor...")
try:
    ina219 = INA219(i2c, addr=INA219_ADDR)
    print("INA219 initialized successfully!")
except Exception as e:
    print(f"Error initializing INA219: {e}")
    print("This could be due to incorrect I2C address or a bad connection.")
    while True:
        time.sleep(1)

# Initialize ADC for Resistance Measurement
adc = ADC(Pin(ADC_PIN))

# Initialize Button Pin with Internal Pull-Up
button = Pin(BUTTON_PIN, Pin.IN, Pin.PULL_UP)

# Initialize Buzzer Pin
buzzer = Pin(BUZZER_PIN, Pin.OUT)
buzzer.value(0) # Ensure buzzer is off initially

# --- Helper Function for Displaying Single Measurement ---
def display_single_measurement(label, value_str, unit_str):
    oled.fill(0) # Clear display
    oled.text(label, 0, 0, 2) # Label at top-left, size 2
    oled.text(value_str, 0, 20, 3) # Value in large font (size 3)
    oled.text(unit_str, 0, 48, 2) # Unit below value, size 2
    oled.show()

# --- Main Loop for Multimeter Functionality ---
print("Starting multimeter loop...")
while True:
    current_time_ms = time.ticks_ms()

    # --- Button Debouncing and Mode Cycling ---
    if button.value() == 0 and time.ticks_diff(current_time_ms, last_button_press_time) > DEBOUNCE_DELAY_MS:
        current_mode_index = (current_mode_index + 1) % len(MEASUREMENT_MODES)
        last_button_press_time = current_time_ms
        # Turn off buzzer immediately on mode change
        buzzer.value(0)
        # Wait for button release to prevent rapid cycling
        while button.value() == 0:
            time.sleep_ms(10)

    # --- Read All Sensor Data ---
    # INA219 Readings (for V/I/P modes)
    bus_voltage = ina219.get_bus_voltage_V()
    current_mA = ina219.get_current_mA()
    power_mW = ina219.get_power_mW()

    # Resistance ADC Reading (for Resistance and Continuity modes)
    # --- ADC Averaging Implementation ---
    sum_adc_value = 0
    for _ in range(NUM_ADC_SAMPLES):
        sum_adc_value += adc.read_u16()
        # time.sleep_us(10) # Uncomment if you find noise is still an issue,
                          # this adds a small delay between each sample
    raw_adc_value = sum_adc_value / NUM_ADC_SAMPLES
    # --- End ADC Averaging Implementation ---

    v_out = (raw_adc_value / 65535.0) * V_SUPPLY

    # --- DEBUGGING PRINTS FOR RESISTANCE ---
    print(f"Raw ADC (Avg): {raw_adc_value:.2f} | V_out (Avg): {v_out:.3f}V")
    # Expected for short (leads touching): Raw ADC near 0, V_out near 0V
    # Expected for open (nothing connected with 1MΩ bleed resistor): Raw ADC ~62121, V_out ~3.128V
    # Check these values with your leads shorted vs. open to pinpoint the issue.

    # Calculate actual resistance for internal use in both R and Continuity modes
    measured_resistance_ohms = 0.0
    # Use a small epsilon to avoid division by zero or very small numbers
    # Also handles V_out very close to V_SUPPLY which can happen with bleed resistor
    if (V_SUPPLY - v_out) > 0.001 and v_out > 0.001: # Ensure denominator and numerator are not zero/too small
        measured_resistance_ohms = R_REF * (v_out / (V_SUPPLY - v_out))
    elif v_out >= (V_SUPPLY - 0.001): # V_out is very close to V_SUPPLY (indicates very high resistance or open)
        measured_resistance_ohms = float('inf')
    else: # v_out is very close to 0 (indicates short)
        measured_resistance_ohms = 0.0

    # --- Prepare Data for Display based on Current Mode ---
    mode_label, mode_unit, mode_decimal_places = MEASUREMENT_MODES[current_mode_index]
    display_value_str = ""
    display_unit_str = mode_unit

    if mode_label == "Bus Voltage":
        display_value_str = f"{bus_voltage:.{mode_decimal_places}f}"
        buzzer.value(0) # Ensure buzzer is off in this mode
    elif mode_label == "Current":
        display_value_str = f"{current_mA:.{mode_decimal_places}f}"
        buzzer.value(0) # Ensure buzzer is off in this mode
    elif mode_label == "Power":
        display_value_str = f"{power_mW:.{mode_decimal_places}f}"
        buzzer.value(0) # Ensure buzzer is off in this mode
    elif mode_label == "Resistance":
        if measured_resistance_ohms < 1000:
            display_value_str = f"{measured_resistance_ohms:.{mode_decimal_places}f}"
            display_unit_str = "Ohm"
        elif measured_resistance_ohms < OPEN_CIRCUIT_THRESHOLD_OHMS: # If it's below our "open" threshold
            display_value_str = f"{measured_resistance_ohms/1000:.{mode_decimal_places}f}"
            display_unit_str = "kOhm"
        elif measured_resistance_ohms >= OPEN_CIRCUIT_THRESHOLD_OHMS: # Now "Open" is detected by high value
            display_value_str = "Open"
            display_unit_str = ""
        else: # Handles actual "Short" (0.0)
            display_value_str = "Short"
            display_unit_str = ""
        buzzer.value(0) # Ensure buzzer is off in this mode (unless it's Continuity)

    elif mode_label == "Continuity":
        # Check for continuity
        if measured_resistance_ohms < CONTINUITY_THRESHOLD_OHMS:
            display_value_str = "Beep!"
            display_unit_str = ""
            buzzer.value(1) # Turn buzzer ON
        elif measured_resistance_ohms >= OPEN_CIRCUIT_THRESHOLD_OHMS: # Now "Open" is detected by high value
            display_value_str = "Open"
            display_unit_str = ""
            buzzer.value(0) # Turn buzzer OFF
        else:
            display_value_str = "No Cont."
            display_unit_str = ""
            buzzer.value(0) # Turn buzzer OFF

    # --- Display the selected measurement ---
    display_single_measurement(mode_label, display_value_str, display_unit_str)

    # --- Print all values to serial for debugging ---
    print(f"Mode: {mode_label} | Bus V: {bus_voltage:.2f}V | Current: {current_mA:.2f}mA | Power: {power_mW:.2f}mW | R_meas: {measured_resistance_ohms:.1f} Ohm | Display: {display_value_str} {display_unit_str}")

    time.sleep_ms(500) # Increased delay for easier reading