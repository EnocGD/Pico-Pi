// ============================================
// ARDUINO NANO LEARNING SYSTEM - COMPLETE & SECURE
// ============================================

#include <Servo.h>

// ============================================
// SAFETY CONSTANTS
// ============================================
#define MAX_INPUT_LENGTH 32
#define SERVO_MIN_ANGLE 0
#define SERVO_MAX_ANGLE 180
#define MAX_DISPLAY_NUMBER 9999

// ============================================
// PIN DEFINITIONS
// ============================================
const int SHARED_D3 = 3, SHARED_D4 = 4, SHARED_D5 = 5, SHARED_D6 = 6;
const int SHARED_D7 = 7, SHARED_D8 = 8, SHARED_D9 = 9, SHARED_D10 = 10;
const int SHARED_D11 = 11, SHARED_D12 = 12, SHARED_A0 = A0, SHARED_A1 = A1;
const int MODE_BUTTON = 2, POTENTIOMETER = A2, STATUS_LED = 13;

// ============================================
// CONSTANTS IN FLASH MEMORY
// ============================================
const PROGMEM byte digitPatterns[] = {
  0b11111100, 0b01100000, 0b11011010, 0b11110010, 0b01100110,
  0b10110110, 0b10111110, 0b11100000, 0b11111110, 0b11110110
};

// Sine lookup table for breathing effect (saves float operations)
const PROGMEM byte sineTable[] = {
  128, 131, 134, 137, 140, 143, 146, 149, 152, 155, 158, 162, 165, 167, 170, 173,
  176, 179, 182, 185, 188, 190, 193, 196, 198, 201, 203, 206, 208, 211, 213, 215,
  218, 220, 222, 224, 226, 228, 230, 232, 234, 235, 237, 238, 240, 241, 243, 244,
  245, 246, 248, 249, 250, 250, 251, 252, 253, 253, 254, 254, 254, 255, 255, 255,
  255, 255, 255, 255, 254, 254, 254, 253, 253, 252, 251, 250, 250, 249, 248, 246,
  245, 244, 243, 241, 240, 238, 237, 235, 234, 232, 230, 228, 226, 224, 222, 220,
  218, 215, 213, 211, 208, 206, 203, 201, 198, 196, 193, 190, 188, 185, 182, 179,
  176, 173, 170, 167, 165, 162, 158, 155, 152, 149, 146, 143, 140, 137, 134, 131,
  128, 124, 121, 118, 115, 112, 109, 106, 103, 100, 97, 93, 90, 88, 85, 82,
  79, 76, 73, 70, 67, 65, 62, 59, 57, 54, 52, 49, 47, 44, 42, 40,
  37, 35, 33, 31, 29, 27, 25, 23, 21, 20, 18, 17, 15, 14, 12, 11,
  10, 9, 7, 6, 5, 5, 4, 3, 2, 2, 1, 1, 1, 0, 0, 0,
  0, 0, 0, 0, 1, 1, 1, 2, 2, 3, 4, 5, 5, 6, 7, 9,
  10, 11, 12, 14, 15, 17, 18, 20, 21, 23, 25, 27, 29, 31, 33, 35,
  37, 40, 42, 44, 47, 49, 52, 54, 57, 59, 62, 65, 67, 70, 73, 76,
  79, 82, 85, 88, 90, 93, 97, 100, 103, 106, 109, 112, 115, 118, 121, 124
};

// ============================================
// SYSTEM STATE VARIABLES
// ============================================
int currentPractice = -1;
bool practiceRunning = false;
bool practiceActive = false;
bool showMenu = true;
bool waitingForConfirmation = false;
bool systemShutdown = false;
unsigned long lastButtonPress = 0;
bool lastButtonState = false;

Servo myServo;

struct PracticeState {
  // Practice 1 - Digital GPIO
  bool redLedState = false;
  bool greenLedState = false;
  bool lastInputButtonState = false;
  unsigned long lastBlinkTime = 0;
  int blinkInterval = 500;
  
  // Practice 2 - Analog Input
  int brightness = 0;
  int lastPotValue = -1;
  
  // Practice 3 - PWM & Servo
  int servoPosition = 90;
  int targetServoPosition = 90;
  unsigned long lastServoUpdate = 0;
  
  // Practice 4 - RGB LED
  int redValue = 0, greenValue = 0, blueValue = 0;
  int colorStep = 0;
  int colorMode = 0;
  unsigned long lastColorUpdate = 0;
  
  // Practice 5 - 7-Segment Display
  int displayNumber = 1234;
  int currentDigit = 0;
  unsigned long lastDisplayUpdate = 0;
  bool showLeadingZeros = false;
} state;

// SECURE: Fixed-size input buffer
char inputBuffer[MAX_INPUT_LENGTH + 1];
int inputIndex = 0;

// ============================================
// SETUP FUNCTION
// ============================================
void setup() {
  Serial.begin(9600);
  
  // Initialize input buffer
  memset(inputBuffer, 0, sizeof(inputBuffer));
  
  pinMode(MODE_BUTTON, INPUT);
  pinMode(STATUS_LED, OUTPUT);
  digitalWrite(STATUS_LED, HIGH);
  
  delay(2000);
  showWelcomeMessage();
  waitForUserReady();
  showMainMenu();
}

// ============================================
// MAIN LOOP
// ============================================
void loop() {
  // SECURE: Check for shutdown flag
  if (systemShutdown) {
    performSafeShutdown();
    return; // Exit loop safely
  }
  
  handleModeButton();
  handleSerialInput();
  
  if (practiceRunning && practiceActive && currentPractice >= 0) {
    runCurrentPractice();
  } else if (showMenu) {
    digitalWrite(STATUS_LED, (millis() / 1000) % 2);
  }
}

// ============================================
// SECURE SHUTDOWN SYSTEM
// ============================================
void performSafeShutdown() {
  Serial.println(F("🔴 PERFORMING SAFE SHUTDOWN..."));
  
  // Clean up all resources
  turnOffAllOutputs();
  
  if (myServo.attached()) {
    myServo.detach();
    Serial.println(F("✓ Servo detached"));
  }
  
  // Reset all states
  practiceRunning = false;
  practiceActive = false;
  currentPractice = -1;
  waitingForConfirmation = false;
  
  Serial.println(F("✓ All resources cleaned up"));
  Serial.println(F("✓ System shutdown complete"));
  Serial.println(F("🔴 GOODBYE! Please reset Arduino to restart."));
  
  // Safe infinite loop with status indication
  while (true) {
    digitalWrite(STATUS_LED, (millis() / 2000) % 2); // Slow blink = shutdown
    delay(100);
  }
}

// ============================================
// WELCOME AND MENU SYSTEM
// ============================================
void showWelcomeMessage() {
  Serial.println(F(""));
  Serial.println(F("╔══════════════════════════════════════════════════════════╗"));
  Serial.println(F("║          ARDUINO NANO LEARNING SYSTEM v3.1              ║"));
  Serial.println(F("║         Step-by-Step Educational Experience             ║"));
  Serial.println(F("╚══════════════════════════════════════════════════════════╝"));
  Serial.println(F(""));
  Serial.println(F("🎯 Learn Arduino fundamentals through hands-on practices"));
  Serial.println(F("🔧 Hardware: Arduino Nano + breadboard components"));
  Serial.println(F("📚 5 practices: GPIO, Analog, PWM, RGB, 7-Segment"));
  Serial.println(F(""));
}

void showMainMenu() {
  showMenu = true;
  practiceRunning = false;
  practiceActive = false;
  currentPractice = -1;
  
  Serial.println(F(""));
  Serial.println(F("═══════════════ PRACTICE MENU ═══════════════"));
  Serial.println(F(""));
  Serial.println(F("  1 → Digital GPIO Control (Beginner, ~10min)"));
  Serial.println(F("      LEDs, buttons, basic I/O"));
  Serial.println(F(""));
  Serial.println(F("  2 → Analog Input Processing (Beginner, ~8min)"));
  Serial.println(F("      Potentiometer, ADC, PWM"));
  Serial.println(F(""));
  Serial.println(F("  3 → PWM & Servo Control (Intermediate, ~12min)"));
  Serial.println(F("      Servo motors, smooth control"));
  Serial.println(F(""));
  Serial.println(F("  4 → RGB LED Color Mixing (Intermediate, ~15min)"));
  Serial.println(F("      Color theory, animations"));
  Serial.println(F(""));
  Serial.println(F("  5 → 7-Segment Display (Advanced, ~20min)"));
  Serial.println(F("      Multiplexing, timing"));
  Serial.println(F(""));
  Serial.println(F("═══════════════════════════════════════════════════════"));
  Serial.println(F(""));
  Serial.println(F("Commands: 1-5 (start), 'info' (system), 'salir' (exit)"));
  Serial.print(F("👉 Selection: "));
}

// ============================================
// USER CONFIRMATION SYSTEM
// ============================================
void waitForUserReady() {
  Serial.println(F(""));
  Serial.println(F("┌─────────────────────────────────────────────────────────┐"));
  Serial.println(F("│                    SYSTEM READY                        │"));
  Serial.println(F("└─────────────────────────────────────────────────────────┘"));
  Serial.println(F(""));
  Serial.println(F("Press ENTER to continue or type 'salir' to exit..."));
  
  waitingForConfirmation = true;
  clearInputBuffer();
  
  while (waitingForConfirmation && !systemShutdown) {
    if (Serial.available()) {
      if (readSecureInput()) {
        String input = String(inputBuffer);
        input.trim();
        input.toLowerCase();
        
        if (input == "salir") {
          systemShutdown = true;
          return;
        }
        
        waitingForConfirmation = false;
        Serial.println(F("Starting Arduino learning..."));
        delay(1000);
      }
    }
  }
}

void waitForUserConfirmation(String message) {
  Serial.println(F(""));
  Serial.println(F("┌─────────────────────────────────────────────────────────┐"));
  Serial.print(F("│ "));
  int padding = (55 - message.length()) / 2;
  for (int i = 0; i < padding; i++) Serial.print(F(" "));
  Serial.print(message);
  for (int i = 0; i < (55 - message.length() - padding); i++) Serial.print(F(" "));
  Serial.println(F(" │"));
  Serial.println(F("└─────────────────────────────────────────────────────────┘"));
  Serial.println(F(""));
  Serial.println(F("Press ENTER to continue, 'menu' for main menu, or 'salir' to exit..."));
  
  waitingForConfirmation = true;
  clearInputBuffer();
  
  while (waitingForConfirmation && !systemShutdown) {
    if (Serial.available()) {
      if (readSecureInput()) {
        String input = String(inputBuffer);
        input.trim();
        input.toLowerCase();
        
        if (input == "salir") {
          systemShutdown = true;
          return;
        }
        if (input == "menu") {
          exitCurrentPractice();
          showMainMenu();
          return;
        }
        
        waitingForConfirmation = false;
        Serial.println(F("Continuing..."));
        delay(500);
      }
    }
  }
}

// ============================================
// SECURE INPUT HANDLING
// ============================================
void clearInputBuffer() {
  memset(inputBuffer, 0, sizeof(inputBuffer));
  inputIndex = 0;
}

bool readSecureInput() {
  char inChar = Serial.read();
  
  if (inChar == '\n' || inChar == '\r') {
    if (inputIndex > 0) {
      inputBuffer[inputIndex] = '\0'; // Null terminate
      return true;
    }
    return false;
  } else if (inChar >= ' ' && inputIndex < MAX_INPUT_LENGTH) {
    inputBuffer[inputIndex++] = inChar;
    if (!practiceRunning || waitingForConfirmation) {
      Serial.print(inChar);
    }
  }
  // SECURE: Ignore characters if buffer is full
  
  return false;
}

void handleSerialInput() {
  while (Serial.available()) {
    if (readSecureInput()) {
      processInput(String(inputBuffer));
      clearInputBuffer();
    }
  }
}

void processInput(String input) {
  input.trim();
  input.toLowerCase();
  
  // SECURE: Global exit command with proper cleanup
  if (input == "salir") {
    systemShutdown = true;
    return;
  }
  
  if (!practiceRunning) {
    // Menu mode with validation
    if (input == "1" && validatePracticeStart(0)) startPractice(0);
    else if (input == "2" && validatePracticeStart(1)) startPractice(1);
    else if (input == "3" && validatePracticeStart(2)) startPractice(2);
    else if (input == "4" && validatePracticeStart(3)) startPractice(3);
    else if (input == "5" && validatePracticeStart(4)) startPractice(4);
    else if (input == "info") showSystemInfo();
    else {
      Serial.println();
      Serial.println(F("❌ Invalid. Use: 1-5, 'info', or 'salir'"));
      Serial.print(F("👉 Selection: "));
    }
  } else if (!waitingForConfirmation) {
    handlePracticeCommand(input);
  }
}

// SECURE: Validate practice can start
bool validatePracticeStart(int practiceNum) {
  if (practiceNum < 0 || practiceNum > 4) {
    Serial.println(F("❌ Invalid practice number"));
    return false;
  }
  
  // Check available memory
  if (getFreeRAM() < 200) {
    Serial.println(F("❌ Insufficient memory"));
    return false;
  }
  
  return true;
}

void handleModeButton() {
  bool currentButtonState = digitalRead(MODE_BUTTON);
  unsigned long currentTime = millis();
  
  if (currentButtonState && !lastButtonState && 
      (currentTime - lastButtonPress > 300)) {
    
    if (practiceRunning) {
      Serial.println(F("\n🔴 HARDWARE BUTTON - EMERGENCY EXIT"));
      exitCurrentPractice();
      showMainMenu();
    }
    
    lastButtonPress = currentTime;
  }
  
  lastButtonState = currentButtonState;
}

// ============================================
// PRACTICE MANAGEMENT
// ============================================
void startPractice(int practiceNum) {
  Serial.println();
  
  currentPractice = practiceNum;
  practiceRunning = true;
  practiceActive = false;
  showMenu = false;
  
  showPracticeIntroduction(practiceNum);
  if (systemShutdown) return;
  
  waitForUserConfirmation("READY TO START");
  if (systemShutdown) return;
  
  turnOffAllOutputs();
  
  if (!configurePinsForPractice(practiceNum)) {
    Serial.println(F("❌ Pin configuration failed"));
    exitCurrentPractice();
    showMainMenu();
    return;
  }
  
  showHardwareSetup(practiceNum);
  if (systemShutdown) return;
  
  waitForUserConfirmation("HARDWARE CONNECTED");
  if (systemShutdown) return;
  
  showPracticeHeader(practiceNum);
  initializePracticeState(practiceNum);
  
  waitForUserConfirmation("START PRACTICE");
  if (systemShutdown) return;
  
  practiceActive = true;
  
  Serial.println(F(""));
  Serial.println(F("🟢 PRACTICE ACTIVE! Watch hardware..."));
  Serial.println(F("Commands available or 'salir' to exit"));
  Serial.println(F(""));
}

void showPracticeIntroduction(int practice) {
  Serial.println(F(""));
  Serial.println(F("╔═══════════════════════════════════════════════════════════╗"));
  
  switch (practice) {
    case 0:
      Serial.println(F("║                 PRACTICE 1: DIGITAL GPIO                 ║"));
      Serial.println(F("╚═══════════════════════════════════════════════════════════╝"));
      Serial.println(F(""));
      Serial.println(F("🎯 Learn: Digital I/O, LEDs, buttons"));
      Serial.println(F("🔧 Skills: pinMode(), digitalWrite(), digitalRead()"));
      Serial.println(F("⚡ Concepts: HIGH/LOW states, pull-down resistors"));
      break;
      
    case 1:
      Serial.println(F("║               PRACTICE 2: ANALOG INPUT                   ║"));
      Serial.println(F("╚═══════════════════════════════════════════════════════════╝"));
      Serial.println(F(""));
      Serial.println(F("🎯 Learn: ADC, potentiometers, PWM output"));
      Serial.println(F("🔧 Skills: analogRead(), analogWrite(), map()"));
      Serial.println(F("⚡ Concepts: 10-bit ADC, voltage dividers"));
      break;
      
    case 2:
      Serial.println(F("║              PRACTICE 3: PWM & SERVO                     ║"));
      Serial.println(F("╚═══════════════════════════════════════════════════════════╝"));
      Serial.println(F(""));
      Serial.println(F("🎯 Learn: Servo control, smooth movements"));
      Serial.println(F("🔧 Skills: Servo library, position control"));
      Serial.println(F("⚡ Concepts: PWM signals, servo timing"));
      break;
      
    case 3:
      Serial.println(F("║               PRACTICE 4: RGB COLORS                     ║"));
      Serial.println(F("╚═══════════════════════════════════════════════════════════╝"));
      Serial.println(F(""));
      Serial.println(F("🎯 Learn: Color mixing, animations"));
      Serial.println(F("🔧 Skills: RGB control, color algorithms"));
      Serial.println(F("⚡ Concepts: Color theory, state machines"));
      break;
      
    case 4:
      Serial.println(F("║              PRACTICE 5: 7-SEGMENT                       ║"));
      Serial.println(F("╚═══════════════════════════════════════════════════════════╝"));
      Serial.println(F(""));
      Serial.println(F("🎯 Learn: Display multiplexing, timing"));
      Serial.println(F("🔧 Skills: Multi-digit displays, patterns"));
      Serial.println(F("⚡ Concepts: Persistence of vision, binary"));
      break;
  }
  
  Serial.println(F(""));
  delay(2000);
}

void showHardwareSetup(int practice) {
  Serial.println(F(""));
  Serial.println(F("🔧 HARDWARE SETUP:"));
  Serial.println(F(""));
  
  switch (practice) {
    case 0:
      Serial.println(F("Components: 2x LEDs, 2x 220Ω, 1x button, 1x 10kΩ"));
      Serial.println(F(""));
      Serial.println(F("Connections:"));
      Serial.println(F("Red LED:    D3 → 220Ω → LED+ → LED- → GND"));
      Serial.println(F("Green LED:  D4 → 220Ω → LED+ → LED- → GND"));
      Serial.println(F("Button:     D5 ← Button ← 10kΩ ← GND"));
      Serial.println(F("            Button other terminal → 5V"));
      break;
      
    case 1:
      Serial.println(F("Components: 1x 10kΩ pot, 1x LED, 1x 220Ω"));
      Serial.println(F(""));
      Serial.println(F("Connections:"));
      Serial.println(F("Pot: Terminal1→5V, Middle→A2, Terminal3→GND"));
      Serial.println(F("LED: D6 → 220Ω → LED+ → LED- → GND"));
      break;
      
    case 2:
      Serial.println(F("Components: 1x servo, 1x LED, 1x 220Ω, 1x pot"));
      Serial.println(F(""));
      Serial.println(F("Connections:"));
      Serial.println(F("Servo: Orange→D9, Red→5V, Brown→GND"));
      Serial.println(F("LED: D6 → 220Ω → LED+ → LED- → GND"));
      Serial.println(F("Pot: Same as Practice 2"));
      break;
      
    case 3:
      Serial.println(F("Components: 1x RGB LED, 3x 220Ω"));
      Serial.println(F(""));
      Serial.println(F("Connections:"));
      Serial.println(F("RGB Red:   D9 → 220Ω → Red pin"));
      Serial.println(F("RGB Green: D10 → 220Ω → Green pin"));
      Serial.println(F("RGB Blue:  D11 → 220Ω → Blue pin"));
      Serial.println(F("RGB Common: Common cathode → GND"));
      break;
      
    case 4:
      Serial.println(F("Components: 4-digit 7-seg, 8x 220Ω, 4x transistors"));
      Serial.println(F(""));
      Serial.println(F("Connections:"));
      Serial.println(F("Segments: D3-D10 → 220Ω → Segments A-G,DP"));
      Serial.println(F("Digits: D11,D12,A0,A1 → 1kΩ → Transistors"));
      Serial.println(F("⚠️ Complex wiring - check diagram"));
      break;
  }
  
  Serial.println(F(""));
  Serial.println(F("⚠️ Check connections before continuing!"));
  Serial.println(F(""));
  delay(2000);
}

void showPracticeHeader(int practice) {
  Serial.println(F("═══════════════════════════════════════════════════════"));
  
  switch (practice) {
    case 0:
      Serial.println(F("           🚀 PRACTICE 1 ACTIVE"));
      Serial.println(F("═══════════════════════════════════════════════════════"));
      Serial.println(F("Commands: 'f'=fast, 'n'=normal, 's'=slow, 'status'"));
      break;
      
    case 1:
      Serial.println(F("           🚀 PRACTICE 2 ACTIVE"));
      Serial.println(F("═══════════════════════════════════════════════════════"));
      Serial.println(F("Commands: 'status' (turn potentiometer)"));
      break;
      
    case 2:
      Serial.println(F("           🚀 PRACTICE 3 ACTIVE"));
      Serial.println(F("═══════════════════════════════════════════════════════"));
      Serial.println(F("Commands: 'center', 'test', 'status'"));
      break;
      
    case 3:
      Serial.println(F("           🚀 PRACTICE 4 ACTIVE"));
      Serial.println(F("═══════════════════════════════════════════════════════"));
      Serial.println(F("Commands: 'mode', 'faster', 'slower', 'status'"));
      break;
      
    case 4:
      Serial.println(F("           🚀 PRACTICE 5 ACTIVE"));
      Serial.println(F("═══════════════════════════════════════════════════════"));
      Serial.println(F("Commands: number(0-9999), 'clear', 'zeros', 'test'"));
      break;
  }
  
  Serial.println(F("Global: 'pause', 'menu', 'salir'"));
  Serial.println(F("═══════════════════════════════════════════════════════"));
}

void exitCurrentPractice() {
  Serial.println();
  Serial.println(F("🔴 EXITING PRACTICE"));
  
  turnOffAllOutputs();
  
  // SECURE: Always detach servo
  if (myServo.attached()) {
    myServo.detach();
    Serial.println(F("✓ Servo detached"));
  }
  
  practiceRunning = false;
  practiceActive = false;
  currentPractice = -1;
  waitingForConfirmation = false;
  
  Serial.println(F("✓ Cleanup complete"));
  delay(1000);
}

void turnOffAllOutputs() {
  for (int pin = 3; pin <= 12; pin++) {
    digitalWrite(pin, LOW);
  }
  digitalWrite(A0, LOW);
  digitalWrite(A1, LOW);
}

// SECURE: Return success/failure status
bool configurePinsForPractice(int practice) {
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
      myServo.write(90);
      break;
      
    case 3:
      pinMode(SHARED_D9, OUTPUT);
      pinMode(SHARED_D10, OUTPUT);
      pinMode(SHARED_D11, OUTPUT);
      break;
      
    case 4:
      for (int i = 3; i <= 12; i++) {
        pinMode(i, OUTPUT);
        digitalWrite(i, LOW);
      }
      pinMode(SHARED_A0, OUTPUT);
      pinMode(SHARED_A1, OUTPUT);
      break;
      
    default:
      return false;
  }
  
  return true;
}

void initializePracticeState(int practice) {
  switch (practice) {
    case 0:
      state.blinkInterval = 500;
      Serial.println(F("🔴 Red LED blinks, button controls green"));
      break;
      
    case 1:
      Serial.println(F("🎛️ Turn potentiometer to control LED"));
      break;
      
    case 2:
      state.servoPosition = 90;
      state.targetServoPosition = 90;
      Serial.println(F("🎛️ Potentiometer controls LED and servo"));
      break;
      
    case 3:
      state.colorMode = 0;
      state.colorStep = 0;
      Serial.println(F("🌈 Rainbow animation starting"));
      break;
      
    case 4:
      state.displayNumber = 1234;
      state.currentDigit = 0;
      Serial.println(F("🔢 Showing: 1234"));
      break;
  }
}

// ============================================
// PRACTICE EXECUTION
// ============================================
void runCurrentPractice() {
  switch (currentPractice) {
    case 0: runPractice1(); break;
    case 1: runPractice2(); break;
    case 2: runPractice3(); break;
    case 3: runPractice4(); break;
    case 4: runPractice5(); break;
  }
  
  updateStatusLED();
}

void runPractice1() {
  unsigned long currentTime = millis();
  
  // Red LED blinking
  if (currentTime - state.lastBlinkTime >= state.blinkInterval) {
    state.redLedState = !state.redLedState;
    digitalWrite(SHARED_D3, state.redLedState);
    state.lastBlinkTime = currentTime;
    
    Serial.print(F("🔴 Red: "));
    Serial.println(state.redLedState ? F("ON") : F("OFF"));
  }
  
  // Button for green LED
  bool currentButtonState = digitalRead(SHARED_D5);
  if (currentButtonState && !state.lastInputButtonState) {
    state.greenLedState = !state.greenLedState;
    digitalWrite(SHARED_D4, state.greenLedState);
    
    Serial.print(F("🟢 Button! Green: "));
    Serial.println(state.greenLedState ? F("ON") : F("OFF"));
  }
  state.lastInputButtonState = currentButtonState;
}

void runPractice2() {
  int potValue = analogRead(POTENTIOMETER);
  state.brightness = map(potValue, 0, 1023, 0, 255);
  analogWrite(SHARED_D6, state.brightness);
  
  if (abs(potValue - state.lastPotValue) > 10) {
    Serial.print(F("📊 Pot:"));
    Serial.print(potValue);
    Serial.print(F(" PWM:"));
    Serial.print(state.brightness);
    Serial.print(F(" ("));
    Serial.print(map(potValue, 0, 1023, 0, 100));
    Serial.println(F("%)"));
    
    state.lastPotValue = potValue;
  }
}

void runPractice3() {
  unsigned long currentTime = millis();
  
  int potValue = analogRead(POTENTIOMETER);
  
  // LED brightness
  state.brightness = map(potValue, 0, 1023, 0, 255);
  analogWrite(SHARED_D6, state.brightness);
  
  // Servo position with bounds checking
  state.targetServoPosition = map(potValue, 0, 1023, SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);
  
  // SECURE: Smooth servo movement with bounds
  if (currentTime - state.lastServoUpdate >= 15) {
    if (state.servoPosition < state.targetServoPosition && state.servoPosition < SERVO_MAX_ANGLE) {
      state.servoPosition++;
    } else if (state.servoPosition > state.targetServoPosition && state.servoPosition > SERVO_MIN_ANGLE) {
      state.servoPosition--;
    }
    
    // SECURE: Clamp servo position
    state.servoPosition = constrain(state.servoPosition, SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);
    
    if (myServo.attached()) {
      myServo.write(state.servoPosition);
    }
    state.lastServoUpdate = currentTime;
  }
  
  // Status update
  static unsigned long lastPrint = 0;
  if (currentTime - lastPrint > 1000) {
    Serial.print(F("📊 LED:"));
    Serial.print(state.brightness);
    Serial.print(F(" Servo:"));
    Serial.print(state.servoPosition);
    Serial.println(F("°"));
    lastPrint = currentTime;
  }
}

void runPractice4() {
  unsigned long currentTime = millis();
  
  if (currentTime - state.lastColorUpdate >= 50) {
    updateRGBColors();
    state.lastColorUpdate = currentTime;
    state.colorStep++;
    
    static int lastPrintStep = 0;
    if (state.colorStep - lastPrintStep >= 100) {
      Serial.print(F("🌈 RGB:("));
      Serial.print(state.redValue);
      Serial.print(F(","));
      Serial.print(state.greenValue);
      Serial.print(F(","));
      Serial.print(state.blueValue);
      Serial.println(F(")"));
      lastPrintStep = state.colorStep;
    }
  }
  
  analogWrite(SHARED_D9, state.redValue);
  analogWrite(SHARED_D10, state.greenValue);
  analogWrite(SHARED_D11, state.blueValue);
}

void runPractice5() {
  unsigned long currentTime = millis();
  
  if (currentTime - state.lastDisplayUpdate >= 2) {
    updateSevenSegmentDisplay();
    state.lastDisplayUpdate = currentTime;
  }
}

// ============================================
// PRACTICE COMMANDS
// ============================================
void handlePracticeCommand(String command) {
  if (command == "pause") {
    practiceActive = false;
    Serial.println(F("⏸️ PAUSED - 'resume' to continue"));
    return;
  }
  
  if (command == "resume") {
    practiceActive = true;
    Serial.println(F("▶️ RESUMED"));
    return;
  }
  
  if (command == "menu") {
    exitCurrentPractice();
    showMainMenu();
    return;
  }
  
  // Practice-specific commands
  switch (currentPractice) {
    case 0: // Practice 1
      if (command == "f") {
        state.blinkInterval = 100;
        Serial.println(F("⚡ Fast blink (100ms)"));
      } else if (command == "n") {
        state.blinkInterval = 500;
        Serial.println(F("🔄 Normal blink (500ms)"));
      } else if (command == "s") {
        state.blinkInterval = 1000;
        Serial.println(F("🐌 Slow blink (1000ms)"));
      } else if (command == "status") {
        showPractice1Status();
      } else {
        Serial.println(F("❌ Try: f, n, s, status, pause, salir"));
      }
      break;
      
    case 1: // Practice 2
      if (command == "status") {
        showPractice2Status();
      } else {
        Serial.println(F("❌ Try: status, pause, salir"));
      }
      break;
      
    case 2: // Practice 3
      if (command == "center") {
        state.targetServoPosition = 90;
        Serial.println(F("🎯 Centering servo..."));
      } else if (command == "test") {
        practiceActive = false;
        testServoRange();
        practiceActive = true;
      } else if (command == "status") {
        showPractice3Status();
      } else {
        Serial.println(F("❌ Try: center, test, status, pause, salir"));
      }
      break;
      
    case 3: // Practice 4
      if (command == "mode") {
        state.colorMode = (state.colorMode + 1) % 3;
        state.colorStep = 0;
        Serial.print(F("🎨 Mode: "));
        switch (state.colorMode) {
          case 0: Serial.println(F("Rainbow")); break;
          case 1: Serial.println(F("Breathing")); break;
          case 2: Serial.println(F("Strobe")); break;
        }
      } else if (command == "faster") {
        Serial.println(F("⚡ Faster animation"));
      } else if (command == "slower") {
        Serial.println(F("🐌 Slower animation"));
      } else if (command == "status") {
        showPractice4Status();
      } else {
        Serial.println(F("❌ Try: mode, faster, slower, status, pause, salir"));
      }
      break;
      
    case 4: // Practice 5
      if (command == "clear") {
        state.displayNumber = 0;
        Serial.println(F("🧹 Display cleared"));
      } else if (command == "zeros") {
        state.showLeadingZeros = !state.showLeadingZeros;
        Serial.print(F("0️⃣ Leading zeros: "));
        Serial.println(state.showLeadingZeros ? F("ON") : F("OFF"));
      } else if (command == "test") {
        practiceActive = false;
        testSevenSegmentDisplay();
        practiceActive = true;
      } else {
        long number = command.toInt();
        if ((number >= 0 && number <= MAX_DISPLAY_NUMBER) || command == "0") {
          state.displayNumber = (int)number;
          Serial.print(F("🔢 Showing: "));
          Serial.println(state.displayNumber);
        } else {
          Serial.println(F("❌ Enter 0-9999, clear, zeros, test, pause, salir"));
        }
      }
      break;
  }
}

// ============================================
// HELPER FUNCTIONS
// ============================================
// SECURE: RGB colors without floating point
void updateRGBColors() {
  int phase = state.colorStep % 360;
  
  switch (state.colorMode) {
    case 0: // Rainbow - using integer math only
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
      
    case 1: // Breathing - using lookup table
      {
        int sineIndex = (state.colorStep * 2) % 256;
        int brightness = pgm_read_byte(&sineTable[sineIndex]);
        state.redValue = brightness;
        state.greenValue = brightness;
        state.blueValue = brightness;
      }
      break;
      
    case 2: // Strobe
      {
        int strobePhase = (state.colorStep / 20) % 8;
        state.redValue = state.greenValue = state.blueValue = 0;
        switch (strobePhase) {
          case 0: state.redValue = 255; break;
          case 1: state.greenValue = 255; break;
          case 2: state.blueValue = 255; break;
          case 3: state.redValue = state.greenValue = 255; break;
          case 4: state.redValue = state.blueValue = 255; break;
          case 5: state.greenValue = state.blueValue = 255; break;
          case 6: state.redValue = state.greenValue = state.blueValue = 255; break;
          case 7: break;
        }
      }
      break;
  }
}

// SECURE: Fixed display multiplexing
void updateSevenSegmentDisplay() {
  // SECURE: Turn off segments FIRST, then digits
  for (int pin = SHARED_D3; pin <= SHARED_D10; pin++) {
    digitalWrite(pin, LOW);
  }
  
  // Then turn off all digits
  digitalWrite(SHARED_D11, LOW);
  digitalWrite(SHARED_D12, LOW);
  digitalWrite(SHARED_A0, LOW);
  digitalWrite(SHARED_A1, LOW);
  
  // Extract digits with bounds checking
  int displayNum = constrain(state.displayNumber, 0, MAX_DISPLAY_NUMBER);
  int thousands = (displayNum / 1000) % 10;
  int hundreds = (displayNum / 100) % 10;
  int tens = (displayNum / 10) % 10;
  int units = displayNum % 10;
  
  int digitValue;
  bool showDigit = true;
  
  switch (state.currentDigit) {
    case 0:
      digitValue = thousands;
      showDigit = state.showLeadingZeros || thousands > 0 || displayNum >= 1000;
      break;
    case 1:
      digitValue = hundreds;
      showDigit = state.showLeadingZeros || hundreds > 0 || displayNum >= 100;
      break;
    case 2:
      digitValue = tens;
      showDigit = state.showLeadingZeros || tens > 0 || displayNum >= 10;
      break;
    case 3:
      digitValue = units;
      break;
    default:
      return; // Invalid digit
  }
  
  if (showDigit && digitValue >= 0 && digitValue <= 9) {
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
    
    // Turn on current digit
    switch (state.currentDigit) {
      case 0: digitalWrite(SHARED_D11, HIGH); break;
      case 1: digitalWrite(SHARED_D12, HIGH); break;
      case 2: digitalWrite(SHARED_A0, HIGH); break;
      case 3: digitalWrite(SHARED_A1, HIGH); break;
    }
  }
  
  state.currentDigit = (state.currentDigit + 1) % 4;
}

void updateStatusLED() {
  int interval = 200 + (currentPractice * 200);
  digitalWrite(STATUS_LED, (millis() / interval) % 2);
}

// ============================================
// STATUS FUNCTIONS
// ============================================
void showPractice1Status() {
  Serial.println(F("📊 Status: Red LED blink interval: "));
  Serial.print(state.blinkInterval);
  Serial.println(F("ms"));
}

void showPractice2Status() {
  int potValue = analogRead(POTENTIOMETER);
  Serial.print(F("📊 Pot: "));
  Serial.print(potValue);
  Serial.print(F(" PWM: "));
  Serial.println(state.brightness);
}

void showPractice3Status() {
  Serial.print(F("📊 LED: "));
  Serial.print(state.brightness);
  Serial.print(F(" Servo: "));
  Serial.print(state.servoPosition);
  Serial.println(F("°"));
}

void showPractice4Status() {
  Serial.print(F("📊 Mode: "));
  switch (state.colorMode) {
    case 0: Serial.print(F("Rainbow")); break;
    case 1: Serial.print(F("Breathing")); break;
    case 2: Serial.print(F("Strobe")); break;
  }
  Serial.print(F(" RGB: ("));
  Serial.print(state.redValue);
  Serial.print(F(","));
  Serial.print(state.greenValue);
  Serial.print(F(","));
  Serial.print(state.blueValue);
  Serial.println(F(")"));
}

void testServoRange() {
  Serial.println(F("🧪 Testing servo range..."));
  for (int pos = state.servoPosition; pos >= 0; pos--) {
    myServo.write(pos);
    delay(15);
  }
  delay(1000);
  
  for (int pos = 0; pos <= 180; pos++) {
    myServo.write(pos);
    delay(15);
  }
  delay(1000);
  
  for (int pos = 180; pos >= 90; pos--) {
    myServo.write(pos);
    delay(15);
  }
  
  state.servoPosition = 90;
  state.targetServoPosition = 90;
  Serial.println(F("✅ Test complete"));
}

void testSevenSegmentDisplay() {
  Serial.println(F("🧪 Testing display..."));
  for (int i = 0; i <= 9; i++) {
    state.displayNumber = i * 1111;
    Serial.print(F("Showing: "));
    Serial.println(state.displayNumber);
    delay(1000);
  }
  state.displayNumber = 1234;
  Serial.println(F("✅ Test complete"));
}

void showSystemInfo() {
  Serial.println();
  Serial.println(F("═══════════════ SYSTEM INFO ═══════════════"));
  Serial.println(F("🔧 Arduino Nano (ATmega328P)"));
  Serial.println(F("⚡ 16 MHz, 32KB Flash, 2KB RAM"));
  Serial.print(F("⏱️ Uptime: "));
  Serial.print(millis() / 1000);
  Serial.println(F(" seconds"));
  Serial.print(F("🆓 Free RAM: "));
  Serial.print(getFreeRAM());
  Serial.println(F(" bytes"));
  Serial.println(F("═══════════════════════════════════════════"));
  Serial.println();
  Serial.print(F("Selection: "));
}

int getFreeRAM() {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}
