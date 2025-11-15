"""
MicroPython INA219 Current/Power/Voltage Monitor Driver
Compatible with TinkerGrid Telemetry System
"""

import time
from micropython import const

# INA219 Register Addresses
_REG_CONFIG = const(0x00)
_REG_SHUNTVOLTAGE = const(0x01)
_REG_BUSVOLTAGE = const(0x02)
_REG_POWER = const(0x03)
_REG_CURRENT = const(0x04)
_REG_CALIBRATION = const(0x05)

# Configuration Register Bits
_CONFIG_RESET = const(0x8000)
_CONFIG_BVOLTAGERANGE_MASK = const(0x2000)
_CONFIG_BVOLTAGERANGE_16V = const(0x0000)
_CONFIG_BVOLTAGERANGE_32V = const(0x2000)

_CONFIG_GAIN_MASK = const(0x1800)
_CONFIG_GAIN_1_40MV = const(0x0000)
_CONFIG_GAIN_2_80MV = const(0x0800)
_CONFIG_GAIN_4_160MV = const(0x1000)
_CONFIG_GAIN_8_320MV = const(0x1800)

_CONFIG_BADCRES_MASK = const(0x0780)
_CONFIG_BADCRES_9BIT = const(0x0000)
_CONFIG_BADCRES_10BIT = const(0x0080)
_CONFIG_BADCRES_11BIT = const(0x0100)
_CONFIG_BADCRES_12BIT = const(0x0180)
_CONFIG_BADCRES_12BIT_2S_1060US = const(0x0480)
_CONFIG_BADCRES_12BIT_4S_2130US = const(0x0500)
_CONFIG_BADCRES_12BIT_8S_4260US = const(0x0580)
_CONFIG_BADCRES_12BIT_16S_8510US = const(0x0600)
_CONFIG_BADCRES_12BIT_32S_17MS = const(0x0680)
_CONFIG_BADCRES_12BIT_64S_34MS = const(0x0700)
_CONFIG_BADCRES_12BIT_128S_69MS = const(0x0780)

_CONFIG_SADCRES_MASK = const(0x0078)
_CONFIG_SADCRES_9BIT_1S_84US = const(0x0000)
_CONFIG_SADCRES_10BIT_1S_148US = const(0x0008)
_CONFIG_SADCRES_11BIT_1S_276US = const(0x0010)
_CONFIG_SADCRES_12BIT_1S_532US = const(0x0018)
_CONFIG_SADCRES_12BIT_2S_1060US = const(0x0048)
_CONFIG_SADCRES_12BIT_4S_2130US = const(0x0050)
_CONFIG_SADCRES_12BIT_8S_4260US = const(0x0058)
_CONFIG_SADCRES_12BIT_16S_8510US = const(0x0060)
_CONFIG_SADCRES_12BIT_32S_17MS = const(0x0068)
_CONFIG_SADCRES_12BIT_64S_34MS = const(0x0070)
_CONFIG_SADCRES_12BIT_128S_69MS = const(0x0078)

_CONFIG_MODE_MASK = const(0x0007)
_CONFIG_MODE_POWERDOWN = const(0x0000)
_CONFIG_MODE_SVOLT_TRIGGERED = const(0x0001)
_CONFIG_MODE_BVOLT_TRIGGERED = const(0x0002)
_CONFIG_MODE_SANDBVOLT_TRIGGERED = const(0x0003)
_CONFIG_MODE_ADCOFF = const(0x0004)
_CONFIG_MODE_SVOLT_CONTINUOUS = const(0x0005)
_CONFIG_MODE_BVOLT_CONTINUOUS = const(0x0006)
_CONFIG_MODE_SANDBVOLT_CONTINUOUS = const(0x0007)


class INA219:
    """INA219 Current/Power/Voltage Monitor"""
    
    def __init__(self, i2c, addr=0x40, shunt_ohms=0.1):
        """
        Initialize INA219
        
        Args:
            i2c: I2C bus object
            addr: I2C address (default 0x40)
            shunt_ohms: Shunt resistor value in ohms (default 0.1)
        """
        self.i2c = i2c
        self.addr = addr
        self.shunt_ohms = shunt_ohms
        
        # Configuration values
        self._current_lsb = 0.0001  # Current LSB in A (0.1mA)
        self._power_lsb = 0.002     # Power LSB in W (2mW)
        self._cal_value = 0
        
        # Initialize the sensor
        self._initialize()
    
    def _initialize(self):
        """Initialize the INA219 with default configuration"""
        # Reset the device
        self._write_register(_REG_CONFIG, _CONFIG_RESET)
        time.sleep_ms(10)
        
        # Configure for 32V, ±320mV gain, 12-bit resolution, continuous mode
        config = (_CONFIG_BVOLTAGERANGE_32V |
                 _CONFIG_GAIN_8_320MV |
                 _CONFIG_BADCRES_12BIT |
                 _CONFIG_SADCRES_12BIT_1S_532US |
                 _CONFIG_MODE_SANDBVOLT_CONTINUOUS)
        
        self._write_register(_REG_CONFIG, config)
        
        # Set calibration for current measurement
        # Cal = 0.04096 / (Current_LSB * Rshunt)
        # Current_LSB = Maximum Expected Current / 32767
        # For 3.2A max: Current_LSB = 3.2/32767 = 0.0001A
        self._cal_value = int(0.04096 / (self._current_lsb * self.shunt_ohms))
        self._write_register(_REG_CALIBRATION, self._cal_value)
    
    def _write_register(self, reg, value):
        """Write 16-bit value to register"""
        data = bytearray(3)
        data[0] = reg
        data[1] = (value >> 8) & 0xFF  # MSB
        data[2] = value & 0xFF         # LSB
        self.i2c.writeto(self.addr, data)
    
    def _read_register(self, reg):
        """Read 16-bit value from register"""
        self.i2c.writeto(self.addr, bytearray([reg]))
        data = self.i2c.readfrom(self.addr, 2)
        return (data[0] << 8) | data[1]
    
    def _read_signed_register(self, reg):
        """Read 16-bit signed value from register"""
        value = self._read_register(reg)
        if value > 32767:
            value -= 65536
        return value
    
    def get_shunt_voltage(self):
        """
        Get shunt voltage in volts
        
        Returns:
            float: Shunt voltage in volts
        """
        raw_value = self._read_signed_register(_REG_SHUNTVOLTAGE)
        return raw_value * 0.00001  # LSB = 10µV
    
    def get_bus_voltage(self):
        """
        Get bus voltage in volts
        
        Returns:
            float: Bus voltage in volts
        """
        raw_value = self._read_register(_REG_BUSVOLTAGE)
        # Shift right 3 bits and multiply by LSB (4mV)
        return (raw_value >> 3) * 0.004
    
    def get_current(self):
        """
        Get current in amperes
        
        Returns:
            float: Current in amperes
        """
        raw_value = self._read_signed_register(_REG_CURRENT)
        return raw_value * self._current_lsb
    
    def get_power(self):
        """
        Get power in watts
        
        Returns:
            float: Power in watts
        """
        raw_value = self._read_signed_register(_REG_POWER)
        return raw_value * self._power_lsb
    
    def get_supply_voltage(self):
        """
        Get supply voltage (bus voltage + shunt voltage)
        
        Returns:
            float: Supply voltage in volts
        """
        return self.get_bus_voltage() + self.get_shunt_voltage()
    
    def set_calibration_32V_2A(self):
        """Set calibration for 32V, 2A max"""
        self._current_lsb = 0.0001  # 0.1mA per bit
        self._power_lsb = 0.002     # 2mW per bit
        self._cal_value = 4096
        self._write_register(_REG_CALIBRATION, self._cal_value)
    
    def set_calibration_32V_1A(self):
        """Set calibration for 32V, 1A max"""
        self._current_lsb = 0.00005  # 0.05mA per bit
        self._power_lsb = 0.001      # 1mW per bit
        self._cal_value = 8192
        self._write_register(_REG_CALIBRATION, self._cal_value)
    
    def set_calibration_16V_400mA(self):
        """Set calibration for 16V, 400mA max"""
        self._current_lsb = 0.00002  # 0.02mA per bit
        self._power_lsb = 0.0004     # 0.4mW per bit
        self._cal_value = 20480
        
        # Also set bus voltage range to 16V
        config = (_CONFIG_BVOLTAGERANGE_16V |
                 _CONFIG_GAIN_1_40MV |
                 _CONFIG_BADCRES_12BIT |
                 _CONFIG_SADCRES_12BIT_1S_532US |
                 _CONFIG_MODE_SANDBVOLT_CONTINUOUS)
        
        self._write_register(_REG_CONFIG, config)
        self._write_register(_REG_CALIBRATION, self._cal_value)
    
    def sleep(self):
        """Put the INA219 into power-down mode"""
        config = self._read_register(_REG_CONFIG)
        config &= ~_CONFIG_MODE_MASK
        config |= _CONFIG_MODE_POWERDOWN
        self._write_register(_REG_CONFIG, config)
    
    def wake(self):
        """Wake the INA219 from power-down mode"""
        config = self._read_register(_REG_CONFIG)
        config &= ~_CONFIG_MODE_MASK
        config |= _CONFIG_MODE_SANDBVOLT_CONTINUOUS
        self._write_register(_REG_CONFIG, config)
    
    def reset(self):
        """Reset the INA219 to default settings"""
        self._write_register(_REG_CONFIG, _CONFIG_RESET)
        time.sleep_ms(10)
        self._initialize()
    
    def is_ready(self):
        """
        Check if conversion is ready
        
        Returns:
            bool: True if conversion is ready
        """
        # Read bus voltage register and check CNVR bit (bit 1)
        bus_voltage = self._read_register(_REG_BUSVOLTAGE)
        return bool(bus_voltage & 0x0002)
    
    def get_all_measurements(self):
        """
        Get all measurements in one call
        
        Returns:
            dict: Dictionary with all measurements
        """
        return {
            'bus_voltage': self.get_bus_voltage(),
            'shunt_voltage': self.get_shunt_voltage(),
            'current': self.get_current(),
            'power': self.get_power(),
            'supply_voltage': self.get_supply_voltage()
        }


# Example usage and test functions
def test_ina219(i2c, addr=0x40):
    """Test function for INA219"""
    print("Testing INA219...")
    
    try:
        ina = INA219(i2c, addr)
        print("INA219 initialized successfully")
        
        # Test measurements
        for i in range(5):
            measurements = ina.get_all_measurements()
            print(f"Test {i+1}:")
            print(f"  Bus Voltage: {measurements['bus_voltage']:.3f} V")
            print(f"  Shunt Voltage: {measurements['shunt_voltage']*1000:.3f} mV")
            print(f"  Current: {measurements['current']*1000:.3f} mA")
            print(f"  Power: {measurements['power']*1000:.3f} mW")
            print(f"  Supply Voltage: {measurements['supply_voltage']:.3f} V")
            print()
            time.sleep(1)
            
    except Exception as e:
        print(f"INA219 test failed: {e}")


if __name__ == "__main__":
    # This will run if the file is executed directly
    from machine import I2C, Pin
    
    # Initialize I2C (adjust pins as needed)
    i2c = I2C(0, sda=Pin(0), scl=Pin(1), freq=400000)
    
    # Scan for devices
    devices = i2c.scan()
    print(f"I2C devices found: {[hex(d) for d in devices]}")
    
    # Test INA219 if found
    if 0x40 in devices:
        test_ina219(i2c, 0x40)
    else:
        print("INA219 not found at address 0x40")
