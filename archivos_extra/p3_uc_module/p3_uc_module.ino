// ============================================
// PRACTICE 3: PWM OUTPUT - LED & SERVO CONTROL
// ============================================

#include <Servo.h>

// Pin definitions
const int POTENTIOMETER = A2;
const int LED_BRIGHTNESS = 6;
const int SERVO_PIN = 9;
const int STATUS_LED = 13;

// Objects
Servo myServo;

// Variables
int potValue = 0;
int ledBrightness = 0;
int servoPosition = 90; // Start at center position
int targetServoPosition = 90;
unsigned long lastUpdate = 0;
unsigned long lastServoUpdate = 0;
const int UPDATE_INTERVAL = 20; // 20ms for smooth operation
const int SERVO_UPDATE_INTERVAL = 15; // Servo update rate

// Smoothing variables
const int SMOOTHING_FACTOR = 5; // Higher = more smoothing
int smoothedPotValue = 0;
bool firstReading = true;

// Demo mode variables
bool demoMode = false;
unsigned long demoStartTime = 0;
int demoStep = 0;

void setup() {
  Serial.begin(9600);
  
  // Configure pins
  pinMode(LED_BRIGHTNESS, OUTPUT);
  pinMode(STATUS_LED, OUTPUT);
  
  // Initialize servo
  myServo.attach(SERVO_PIN);
  myServo.write(90); // Center position
  
  Serial.println("=== PRACTICE 3: PWM OUTPUT CONTROL ===");
  Serial.println("Potentiometer controls both LED brightness and servo position");
  Serial.println("Commands:");
  Serial.println("  'd' - Toggle demo mode");
  Serial.println("  's' - Show status");
  Serial.println("  'c' - Center servo");
  Serial.println();
  
  // Initial reading
  potValue = analogRead(POTENTIOMETER);
  smoothedPotValue = potValue;
  
  Serial.print("Initial potentiometer reading: ");
  Serial.println(potValue);
  
  digitalWrite(STATUS_LED, HIGH);
  delay(1000); // Allow servo to reach initial position
}

void loop() {
  unsigned long currentTime = millis();
  
  // Handle serial commands
  handleSerialCommands();
  
  // Handle demo mode
  if (demoMode) {
    handleDemoMode(currentTime);
  } else {
    // Normal operation - read potentiometer
    if (currentTime - lastUpdate >= UPDATE_INTERVAL) {
      readAndProcessInputs();
      lastUpdate = currentTime;
    }
  }
  
  // Update servo position smoothly
  if (currentTime - lastServoUpdate >= SERVO_UPDATE_INTERVAL) {
    updateServoPosition();
    lastServoUpdate = currentTime;
  }
  
  // Status LED heartbeat
  digitalWrite(STATUS_LED, (currentTime / 500) % 2);
}

void handleSerialCommands() {
  if (Serial.available()) {
    char command = Serial.read();
    
    switch (command) {
      case 'd':
      case 'D':
        toggleDemoMode();
        break;
      case 's':
      case 'S':
        printStatus();
        break;
      case 'c':
      case 'C':
        centerServo();
        break;
      case 't':
      case 'T':
        testServoRange();
        break;
    }
  }
}

void readAndProcessInputs() {
  // Read potentiometer
  potValue = analogRead(POTENTIOMETER);
  
  // Apply smoothing
  if (firstReading) {
    smoothedPotValue = potValue;
    firstReading = false;
  } else {
    smoothedPotValue = ((smoothedPotValue * (SMOOTHING_FACTOR - 1)) + potValue) / SMOOTHING_FACTOR;
  }
  
  // Map to LED brightness (0-255)
  ledBrightness = map(smoothedPotValue, 0, 1023, 0, 255);
  analogWrite(LED_BRIGHTNESS, ledBrightness);
  
  // Map to servo position (0-180 degrees)
  targetServoPosition = map(smoothedPotValue, 0, 1023, 0, 180);
  
  // Print values periodically
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint >= 500) {
    Serial.print("Pot: ");
    Serial.print(potValue);
    Serial.print(" | Smoothed: ");
    Serial.print(smoothedPotValue);
    Serial.print(" | LED: ");
    Serial.print(ledBrightness);
    Serial.print(" | Servo Target: ");
    Serial.print(targetServoPosition);
    Serial.print("° | Current: ");
    Serial.print(servoPosition);
    Serial.println("°");
    
    lastPrint = millis();
  }
}

void updateServoPosition() {
  // Move servo gradually towards target position
  if (servoPosition < targetServoPosition) {
    servoPosition++;
  } else if (servoPosition > targetServoPosition) {
    servoPosition--;
  }
  
  // Constrain to valid range
  servoPosition = constrain(servoPosition, 0, 180);
  
  // Update servo
  myServo.write(servoPosition);
}

void toggleDemoMode() {
  demoMode = !demoMode;
  
  if (demoMode) {
    Serial.println("Demo mode ON - Automatic sweep");
    demoStartTime = millis();
    demoStep = 0;
  } else {
    Serial.println("Demo mode OFF - Manual control");
  }
}

void handleDemoMode(unsigned long currentTime) {
  // Create automatic sweep pattern
  unsigned long demoTime = currentTime - demoStartTime;
  
  // Different demo patterns
  switch (demoStep % 3) {
    case 0: // Sine wave
      {
        float angle = (demoTime / 50.0) * 0.1; // Slow sine wave
        int value = 512 + (int)(511 * sin(angle));
        simulatePotValue(value);
      }
      break;
      
    case 1: // Sawtooth
      {
        int value = (demoTime / 10) % 1024;
        simulatePotValue(value);
      }
      break;
      
    case 2: // Step pattern
      {
        int step = (demoTime / 2000) % 4;
        int value = step * 341; // 0, 341, 682, 1023
        simulatePotValue(value);
      }
      break;
  }
  
  // Change demo pattern every 10 seconds
  if (demoTime > 10000) {
    demoStep++;
    demoStartTime = currentTime;
    Serial.print("Demo pattern ");
    Serial.println(demoStep % 3);
  }
}

void simulatePotValue(int value) {
  // Simulate potentiometer reading for demo mode
  smoothedPotValue = constrain(value, 0, 1023);
  
  // Update outputs
  ledBrightness = map(smoothedPotValue, 0, 1023, 0, 255);
  analogWrite(LED_BRIGHTNESS, ledBrightness);
  
  targetServoPosition = map(smoothedPotValue, 0, 1023, 0, 180);
}

void centerServo() {
  Serial.println("Centering servo...");
  targetServoPosition = 90;
  
  // Force immediate centering
  while (servoPosition != 90) {
    if (servoPosition < 90) servoPosition++;
    else servoPosition--;
    
    myServo.write(servoPosition);
    delay(15);
  }
  
  Serial.println("Servo centered at 90°");
}

void testServoRange() {
  Serial.println("Testing servo full range...");
  
  // Move to 0°
  Serial.println("Moving to 0°");
  for (int pos = servoPosition; pos >= 0; pos--) {
    myServo.write(pos);
    delay(15);
  }
  delay(1000);
  
  // Move to 180°
  Serial.println("Moving to 180°");
  for (int pos = 0; pos <= 180; pos++) {
    myServo.write(pos);
    delay(15);
  }
  delay(1000);
  
  // Return to center
  Serial.println("Returning to center");
  for (int pos = 180; pos >= 90; pos--) {
    myServo.write(pos);
    delay(15);
  }
  
  servoPosition = 90;
  targetServoPosition = 90;
  Serial.println("Servo test complete");
}

void printStatus() {
  Serial.println("\n=== STATUS REPORT ===");
  Serial.print("Potentiometer: ");
  Serial.print(potValue);
  Serial.print(" (Raw) | ");
  Serial.print(smoothedPotValue);
  Serial.println(" (Smoothed)");
  
  Serial.print("LED Brightness: ");
  Serial.print(ledBrightness);
  Serial.print("/255 (");
  Serial.print((ledBrightness / 255.0) * 100, 1);
  Serial.println("%)");
  
  Serial.print("Servo Position: ");
  Serial.print(servoPosition);
  Serial.print("° (Target: ");
  Serial.print(targetServoPosition);
  Serial.println("°)");
  
  Serial.print("Demo Mode: ");
  Serial.println(demoMode ? "ON" : "OFF");
  
  Serial.print("PWM Frequency: ~490Hz (LED), ~50Hz (Servo)");
  Serial.println();
}
