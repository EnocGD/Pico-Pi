// ============================================
// PRACTICE 2: ANALOG INPUT - POTENTIOMETER LED CONTROL
// ============================================

// Pin definitions
const int POTENTIOMETER = A2;
const int LED_BRIGHTNESS = 6;  // Must be PWM capable pin
const int STATUS_LED = 13;

// Variables
int potValue = 0;
int ledBrightness = 0;
int lastPotValue = -1;
unsigned long lastUpdate = 0;
const int UPDATE_INTERVAL = 50; // Update every 50ms

// Calibration values
int potMin = 1023;
int potMax = 0;
bool calibrationMode = false;
unsigned long calibrationStart = 0;
const unsigned long CALIBRATION_TIME = 5000; // 5 seconds

void setup() {
  Serial.begin(9600);
  
  // Configure pins
  pinMode(LED_BRIGHTNESS, OUTPUT);
  pinMode(STATUS_LED, OUTPUT);
  
  Serial.println("=== PRACTICE 2: ANALOG INPUT CONTROL ===");
  Serial.println("Potentiometer controls LED brightness");
  Serial.println("Send 'c' to start calibration mode");
  Serial.println("ADC Range: 0-1023, PWM Range: 0-255");
  Serial.println();
  
  // Initial readings
  potValue = analogRead(POTENTIOMETER);
  Serial.print("Initial potentiometer reading: ");
  Serial.println(potValue);
  
  digitalWrite(STATUS_LED, HIGH);
}

void loop() {
  unsigned long currentTime = millis();
  
  // Handle serial commands
  handleSerialCommands();
  
  // Handle calibration mode
  if (calibrationMode) {
    handleCalibration(currentTime);
    return; // Skip normal operation during calibration
  }
  
  // Read and process potentiometer
  if (currentTime - lastUpdate >= UPDATE_INTERVAL) {
    readAndProcessPotentiometer();
    lastUpdate = currentTime;
  }
  
  // Status LED heartbeat
  digitalWrite(STATUS_LED, (currentTime / 1000) % 2);
}

void handleSerialCommands() {
  if (Serial.available()) {
    char command = Serial.read();
    
    switch (command) {
      case 'c':
      case 'C':
        startCalibration();
        break;
      case 's':
      case 'S':
        printDetailedStatus();
        break;
      case 'r':
      case 'R':
        resetCalibration();
        break;
    }
  }
}

void readAndProcessPotentiometer() {
  potValue = analogRead(POTENTIOMETER);
  
  // Apply calibration if available
  int calibratedValue = potValue;
  if (potMax > potMin) {
    calibratedValue = map(potValue, potMin, potMax, 0, 1023);
    calibratedValue = constrain(calibratedValue, 0, 1023);
  }
  
  // Convert to PWM value (0-255)
  ledBrightness = map(calibratedValue, 0, 1023, 0, 255);
  
  // Update LED
  analogWrite(LED_BRIGHTNESS, ledBrightness);
  
  // Print values if changed significantly
  if (abs(potValue - lastPotValue) > 5) {
    Serial.print("Pot Raw: ");
    Serial.print(potValue);
    Serial.print(" | Calibrated: ");
    Serial.print(calibratedValue);
    Serial.print(" | PWM: ");
    Serial.print(ledBrightness);
    Serial.print(" | Brightness: ");
    Serial.print(map(ledBrightness, 0, 255, 0, 100));
    Serial.println("%");
    
    lastPotValue = potValue;
  }
}

void startCalibration() {
  calibrationMode = true;
  calibrationStart = millis();
  potMin = 1023;
  potMax = 0;
  
  Serial.println("\n=== CALIBRATION MODE ===");
  Serial.println("Turn potentiometer to full range for 5 seconds...");
  Serial.println("Move from minimum to maximum position");
}

void handleCalibration(unsigned long currentTime) {
  int currentReading = analogRead(POTENTIOMETER);
  
  // Update min/max values
  if (currentReading < potMin) potMin = currentReading;
  if (currentReading > potMax) potMax = currentReading;
  
  // Show progress
  static unsigned long lastCalibrationUpdate = 0;
  if (currentTime - lastCalibrationUpdate >= 200) {
    Serial.print("Current: ");
    Serial.print(currentReading);
    Serial.print(" | Min: ");
    Serial.print(potMin);
    Serial.print(" | Max: ");
    Serial.print(potMax);
    Serial.print(" | Time left: ");
    Serial.print((CALIBRATION_TIME - (currentTime - calibrationStart)) / 1000);
    Serial.println("s");
    
    lastCalibrationUpdate = currentTime;
  }
  
  // Check if calibration time is up
  if (currentTime - calibrationStart >= CALIBRATION_TIME) {
    calibrationMode = false;
    
    Serial.println("\n=== CALIBRATION COMPLETE ===");
    Serial.print("Calibrated range: ");
    Serial.print(potMin);
    Serial.print(" to ");
    Serial.println(potMax);
    
    if (potMax - potMin < 100) {
      Serial.println("Warning: Calibration range is small. Try again with fuller range.");
    }
  }
}

void resetCalibration() {
  potMin = 1023;
  potMax = 0;
  Serial.println("Calibration reset to default values");
}

void printDetailedStatus() {
  Serial.println("\n=== DETAILED STATUS ===");
  Serial.print("Raw ADC Value: ");
  Serial.print(potValue);
  Serial.print(" (");
  Serial.print((potValue / 1023.0) * 5.0, 2);
  Serial.println("V)");
  
  Serial.print("PWM Value: ");
  Serial.print(ledBrightness);
  Serial.print(" (");
  Serial.print((ledBrightness / 255.0) * 100, 1);
  Serial.println("%)");
  
  Serial.print("Calibration Range: ");
  Serial.print(potMin);
  Serial.print(" to ");
  Serial.println(potMax);
  
  Serial.print("ADC Resolution: 10-bit (");
  Serial.print(5.0 / 1024, 4);
  Serial.println("V per step)");
  
  Serial.print("PWM Frequency: ~490Hz on pin ");
  Serial.println(LED_BRIGHTNESS);
  Serial.println();
}
