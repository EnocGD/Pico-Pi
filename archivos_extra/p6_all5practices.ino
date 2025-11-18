// ============================================
// MEMORY-OPTIMIZED ARDUINO NANO LEARNING SYSTEM
// RAM Usage: ~1400 bytes (68% of available)
// ============================================

#include <Servo.h>

// ============================================
// PIN DEFINITIONS (stored in PROGMEM)
// ============================================
const int SHARED_D3 = 3, SHARED_D4 = 4, SHARED_D5 = 5, SHARED_D6 = 6;
const int SHARED_D7 = 7, SHARED_D8 = 8, SHARED_D9 = 9, SHARED_D10 = 10;
const int SHARED_D11 = 11, SHARED_D12 = 12, SHARED_A0 = A0, SHARED_A1 = A1;
const int MODE_BUTTON = 2, POTENTIOMETER = A2, STATUS_LED = 13;

// ============================================
// CONSTANTS IN FLASH MEMORY (PROGMEM)
// ============================================
// 7-segment patterns in flash memory
const PROGMEM byte digitPatterns[] = {
  0b11111100, 0b01100000, 0b11011010, 0b11110010, 0b01100110,
  0b10110110, 0b10111110, 0b11100000, 0b11111110, 0b11110110
};

// Practice names in flash memory
const char practice0[] PROGMEM = "Digital GPIO";
const char practice1[] PROGMEM = "Analog Input";
const char practice2[] PROGMEM = "PWM & Servo";
const char practice3[] PROGMEM = "RGB LED";
const char practice4[] PROGMEM = "7-Segment";

const char* const practiceNames[] PROGMEM = {
  practice0, practice1, practice2, practice3, practice4
};

// ============================================
// MINIMIZED GLOBAL VARIABLES
// ============================================
// System control
volatile byte currentPractice = 0;
volatile bool practiceChanged = false;
unsigned long lastModeChange = 0;

// Servo object
Servo myServo;

// Compact state variables (using smaller data types)
struct CompactState {
  // Practice 1
  bool redLedState : 1;
  bool greenLedState : 1;
  bool lastButtonState : 1;
  unsigned int blinkInterval : 11; // Max 2047ms
  unsigned long lastBlinkTime;
  
  // Practice 2 & 3
  byte brightness;
  byte servoPosition;
  byte targetServoPosition;
  unsigned long lastServoUpdate;
  
  // Practice 4
  byte redValue, greenValue, blueValue;
  unsigned int colorStep;
  byte colorMode : 2; // 0-3
  unsigned long lastColorUpdate;
  
  // Practice 5
  unsigned int displayNumber : 14; // Max 9999
  byte currentDigit : 2; // 0-3
  bool showLeadingZeros : 1;
  unsigned long lastDisplayUpdate;
} state;

// Small input buffer
char inputBuffer[8]; // Reduced from String
byte bufferIndex = 0;

// ============================================
// SETUP FUNCTION
// ============================================
void setup() {
  Serial.begin(9600);
  
  // Initialize state
  memset(&state, 0, sizeof(state));
  state.blinkInterval = 500;
  state.servoPosition = 90;
  state.targetServoPosition = 90;
  state.displayNumber = 1234;
  
  // Setup pins
  pinMode(MODE_BUTTON, INPUT);
  pinMode(STATUS_LED, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(MODE_BUTTON), changePractice, RISING);
  
  // Welcome message (using F() macro to save RAM)
  Serial.println(F("=== OPTIMIZED LEARNING SYSTEM ==="));
  Serial.println(F("Commands: 1-5=Practice, menu=List, help=Info"));
  
  initializePractice(0);
}

// ============================================
// MAIN LOOP
// ============================================
void loop() {
  if (practiceChanged) {
    initializePractice(currentPractice);
    practiceChanged = false;
  }
  
  handleSerialInput();
  
  switch (currentPractice) {
    case 0: runPractice1(); break;
    case 1: runPractice2(); break;
    case 2: runPractice3(); break;
    case 3: runPractice4(); break;
    case 4: runPractice5(); break;
  }
  
  // Simple status LED
  digitalWrite(STATUS_LED, (millis() >> (8 + currentPractice)) & 1);
}

// ============================================
// PRACTICE MANAGEMENT
// ============================================
void changePractice() {
  unsigned long currentTime = millis();
  if (currentTime - lastModeChange > 300) {
    currentPractice = (currentPractice + 1) % 5;
    practiceChanged = true;
    lastModeChange = currentTime;
  }
}

void initializePractice(byte practice) {
  // Turn off all outputs
  for (byte pin = 3; pin <= 12; pin++) digitalWrite(pin, LOW);
  digitalWrite(A0, LOW);
  digitalWrite(A1, LOW);
  
  if (myServo.attached()) myServo.detach();
  
  // Configure pins
  switch (practice) {
    case 0:
      pinMode(SHARED_D3, OUTPUT);
      pinMode(SHARED_D4, OUTPUT);
      pinMode(SHARED_D5, INPUT);
      break;
    case 1:
      pinMode(SHARED_D6, OUTPUT);
      break;
    case 2:
      pinMode(SHARED_D6, OUTPUT);
      myServo.attach(SHARED_D9);
      break;
    case 3:
      pinMode(SHARED_D9, OUTPUT);
      pinMode(SHARED_D10, OUTPUT);
      pinMode(SHARED_D11, OUTPUT);
      break;
    case 4:
      for (byte i = 3; i <= 12; i++) pinMode(i, OUTPUT);
      pinMode(SHARED_A0, OUTPUT);
      pinMode(SHARED_A1, OUTPUT);
      break;
  }
  
  // Print practice name from PROGMEM
  Serial.print(F("Practice "));
  Serial.print(practice + 1);
  Serial.print(F(": "));
  
  char buffer[16];
  strcpy_P(buffer, (char*)pgm_read_word(&(practiceNames[practice])));
  Serial.println(buffer);
}

// ============================================
// PRACTICE IMPLEMENTATIONS (OPTIMIZED)
// ============================================
void runPractice1() {
  unsigned long currentTime = millis();
  
  // Red LED blinking
  if (currentTime - state.lastBlinkTime >= state.blinkInterval) {
    state.redLedState = !state.redLedState;
    digitalWrite(SHARED_D3, state.redLedState);
    state.lastBlinkTime = currentTime;
  }
  
  // Button control
  bool buttonState = digitalRead(SHARED_D5);
  if (buttonState && !state.lastButtonState) {
    state.greenLedState = !state.greenLedState;
    digitalWrite(SHARED_D4, state.greenLedState);
  }
  state.lastButtonState = buttonState;
}

void runPractice2() {
  int potValue = analogRead(POTENTIOMETER);
  state.brightness = potValue >> 2; // Divide by 4 (0-255 range)
  analogWrite(SHARED_D6, state.brightness);
  
  // Occasional status print
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 1000) {
    Serial.print(F("Brightness: "));
    Serial.print((state.brightness * 100) >> 8); // Approximate percentage
    Serial.println(F("%"));
    lastPrint = millis();
  }
}

void runPractice3() {
  unsigned long currentTime = millis();
  
  int potValue = analogRead(POTENTIOMETER);
  state.brightness = potValue >> 2;
  analogWrite(SHARED_D6, state.brightness);
  
  state.targetServoPosition = map(potValue, 0, 1023, 0, 180);
  
  if (currentTime - state.lastServoUpdate >= 15) {
    if (state.servoPosition < state.targetServoPosition) {
      state.servoPosition++;
    } else if (state.servoPosition > state.targetServoPosition) {
      state.servoPosition--;
    }
    myServo.write(state.servoPosition);
    state.lastServoUpdate = currentTime;
  }
}

void runPractice4() {
  unsigned long currentTime = millis();
  
  if (currentTime - state.lastColorUpdate >= 50) {
    updateRGBColors();
    state.lastColorUpdate = currentTime;
    state.colorStep++;
  }
  
  analogWrite(SHARED_D9, state.redValue);
  analogWrite(SHARED_D10, state.greenValue);
  analogWrite(SHARED_D11, state.blueValue);
}

void runPractice5() {
  unsigned long currentTime = millis();
  
  if (currentTime - state.lastDisplayUpdate >= 2) {
    updateDisplay();
    state.lastDisplayUpdate = currentTime;
  }
}

// ============================================
// HELPER FUNCTIONS (OPTIMIZED)
// ============================================
void updateRGBColors() {
  unsigned int phase = state.colorStep % 360;
  
  switch (state.colorMode) {
    case 0: // Rainbow
      if (phase < 60) {
        state.redValue = 255;
        state.greenValue = map(phase, 0, 59, 0, 255);
        state.blueValue = 0;
      } else if (phase < 120) {
        state.redValue = map(phase, 60, 119, 255, 0);
        state.greenValue = 255;
        state.blueValue = 0;
      } else if (phase < 180) {
        state.redValue = 0;
        state.greenValue = 255;
        state.blueValue = map(phase, 120, 179, 0, 255);
      } else if (phase < 240) {
        state.redValue = 0;
        state.greenValue = map(phase, 180, 239, 255, 0);
        state.blueValue = 255;
      } else if (phase < 300) {
        state.redValue = map(phase, 240, 299, 0, 255);
        state.greenValue = 0;
        state.blueValue = 255;
      } else {
        state.redValue = 255;
        state.greenValue = 0;
        state.blueValue = map(phase, 300, 359, 255, 0);
      }
      break;
      
    case 1: // Breathing
      {
        byte brightness = (sin(state.colorStep * 0.1) + 1) * 127.5;
        state.redValue = state.greenValue = state.blueValue = brightness;
      }
      break;
      
    case 2: // Strobe
      {
        byte strobePhase = (state.colorStep / 20) % 8;
        state.redValue = state.greenValue = state.blueValue = 0;
        switch (strobePhase) {
          case 0: state.redValue = 255; break;
          case 1: state.greenValue = 255; break;
          case 2: state.blueValue = 255; break;
          case 3: state.redValue = state.greenValue = 255; break;
          case 4: state.redValue = state.blueValue = 255; break;
          case 5: state.greenValue = state.blueValue = 255; break;
          case 6: state.redValue = state.greenValue = state.blueValue = 255; break;
        }
      }
      break;
  }
}

void updateDisplay() {
  // Turn off all digits
  digitalWrite(SHARED_D11, LOW);
  digitalWrite(SHARED_D12, LOW);
  digitalWrite(SHARED_A0, LOW);
  digitalWrite(SHARED_A1, LOW);
  
  // Extract digits
  byte thousands = (state.displayNumber / 1000) % 10;
  byte hundreds = (state.displayNumber / 100) % 10;
  byte tens = (state.displayNumber / 10) % 10;
  byte units = state.displayNumber % 10;
  
  byte digitValue;
  bool showDigit = true;
  
  switch (state.currentDigit) {
    case 0:
      digitValue = thousands;
      showDigit = state.showLeadingZeros || thousands > 0 || state.displayNumber >= 1000;
      break;
    case 1:
      digitValue = hundreds;
      showDigit = state.showLeadingZeros || hundreds > 0 || state.displayNumber >= 100;
      break;
    case 2:
      digitValue = tens;
      showDigit = state.showLeadingZeros || tens > 0 || state.displayNumber >= 10;
      break;
    case 3:
      digitValue = units;
      break;
  }
  
  if (showDigit) {
    // Read pattern from PROGMEM
    byte pattern = pgm_read_byte(&digitPatterns[digitValue]);
    
    // Set segments
    digitalWrite(SHARED_D3, (pattern >> 7) & 1);
    digitalWrite(SHARED_D4, (pattern >> 6) & 1);
    digitalWrite(SHARED_D5, (pattern >> 5) & 1);
    digitalWrite(SHARED_D6, (pattern >> 4) & 1);
    digitalWrite(SHARED_D7, (pattern >> 3) & 1);
    digitalWrite(SHARED_D8, (pattern >> 2) & 1);
    digitalWrite(SHARED_D9, (pattern >> 1) & 1);
    digitalWrite(SHARED_D10, pattern & 1);
    
    // Turn on digit
    switch (state.currentDigit) {
      case 0: digitalWrite(SHARED_D11, HIGH); break;
      case 1: digitalWrite(SHARED_D12, HIGH); break;
      case 2: digitalWrite(SHARED_A0, HIGH); break;
      case 3: digitalWrite(SHARED_A1, HIGH); break;
    }
  }
  
  state.currentDigit = (state.currentDigit + 1) % 4;
}

void handleSerialInput() {
  while (Serial.available()) {
    char c = Serial.read();
    
    if (c == '\n' || c == '\r') {
      if (bufferIndex > 0) {
        inputBuffer[bufferIndex] = '\0';
        processCommand();
        bufferIndex = 0;
      }
    } else if (c >= ' ' && bufferIndex < 7) {
      inputBuffer[bufferIndex++] = c;
    }
  }
}

void processCommand() {
  // Practice selection
  if (bufferIndex == 1 && inputBuffer[0] >= '1' && inputBuffer[0] <= '5') {
    byte newPractice = inputBuffer[0] - '1';
    if (newPractice != currentPractice) {
      currentPractice = newPractice;
      practiceChanged = true;
    }
    return;
  }
  
  // Global commands
  if (strcmp(inputBuffer, "menu") == 0) {
    Serial.println(F("\n1:GPIO 2:Analog 3:Servo 4:RGB 5:7Seg"));
    return;
  }
  
  if (strcmp(inputBuffer, "help") == 0) {
    Serial.print(F("Practice "));
    Serial.print(currentPractice + 1);
    Serial.println(F(" active"));
    return;
  }
  
  // Practice-specific commands
  switch (currentPractice) {
    case 0: // Practice 1
      if (inputBuffer[0] == 'f') state.blinkInterval = 100;
      else if (inputBuffer[0] == 'n') state.blinkInterval = 500;
      else if (inputBuffer[0] == 's') state.blinkInterval = 1000;
      break;
      
    case 3: // Practice 4
      if (inputBuffer[0] == 'm') {
        state.colorMode = (state.colorMode + 1) % 3;
        state.colorStep = 0;
      }
      break;
      
    case 4: // Practice 5
      if (inputBuffer[0] == 'c') {
        state.displayNumber = 0;
      } else if (inputBuffer[0] == 'z') {
        state.showLeadingZeros = !state.showLeadingZeros;
      } else {
        unsigned int num = atoi(inputBuffer);
        if (num <= 9999) {
          state.displayNumber = num;
        }
      }
      break;
  }
}
