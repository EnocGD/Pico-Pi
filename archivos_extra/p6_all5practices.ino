// ============================================
// ARDUINO NANO LEARNING SYSTEM - STEP-BY-STEP VERSION
// Enhanced with User-Controlled Execution Flow
// ============================================

#include <Servo.h>

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

// ============================================
// SYSTEM STATE VARIABLES
// ============================================
// System control
int currentPractice = -1; // -1 = menu mode, 0-4 = practice mode
bool practiceRunning = false;
bool practiceActive = false; // New: Controls if practice loop is running
bool showMenu = true;
bool waitingForConfirmation = false;
unsigned long lastButtonPress = 0;
bool lastButtonState = false;

// Servo object
Servo myServo;

// Practice state variables
struct PracticeState {
  // Practice 1 - Digital GPIO
  bool redLedState = false;
  bool greenLedState = false;
  bool lastInputButtonState = false;
  unsigned long lastBlinkTime = 0;
  int blinkInterval = 500;
  int blinkMode = 0;
  
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

// Input buffer for commands
String inputBuffer = "";

// ============================================
// SETUP FUNCTION
// ============================================
void setup() {
  Serial.begin(9600);
  
  // Setup pins
  pinMode(MODE_BUTTON, INPUT);
  pinMode(STATUS_LED, OUTPUT);
  digitalWrite(STATUS_LED, HIGH);
  
  // Wait for serial connection and user readiness
  delay(2000);
  
  // Show welcome message
  showWelcomeMessage();
  waitForUserReady();
  showMainMenu();
}

// ============================================
// MAIN LOOP
// ============================================
void loop() {
  // Handle hardware button (exit to menu)
  handleModeButton();
  
  // Handle serial input
  handleSerialInput();
  
  // Run current practice only if active
  if (practiceRunning && practiceActive && currentPractice >= 0) {
    runCurrentPractice();
  } else if (showMenu) {
    // In menu mode - just blink status LED slowly
    digitalWrite(STATUS_LED, (millis() / 1000) % 2);
  }
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
  Serial.println(F("Please take a moment to read the information above."));
  Serial.println(F("This learning system will guide you step-by-step through"));
  Serial.println(F("each practice with clear instructions and confirmations."));
  Serial.println(F(""));
  Serial.println(F("Press ENTER when you're ready to continue..."));
  
  waitingForConfirmation = true;
  while (waitingForConfirmation) {
    if (Serial.available()) {
      char c = Serial.read();
      if (c == '\n' || c == '\r') {
        waitingForConfirmation = false;
        Serial.println(F("Great! Let's begin your Arduino learning journey."));
        delay(1000);
      }
    }
  }
}

void waitForUserConfirmation(String message) {
  Serial.println(F(""));
  Serial.println(F("┌─────────────────────────────────────────────────────────┐"));
  Serial.print(F("│ "));
  // Center the message
  int padding = (55 - message.length()) / 2;
  for (int i = 0; i < padding; i++) Serial.print(F(" "));
  Serial.print(message);
  for (int i = 0; i < (55 - message.length() - padding); i++) Serial.print(F(" "));
  Serial.println(F(" │"));
  Serial.println(F("└─────────────────────────────────────────────────────────┘"));
  Serial.println(F(""));
  Serial.println(F("Press ENTER to continue, or type 'menu' to return to main menu..."));
  
  waitingForConfirmation = true;
  String input = "";
  
  while (waitingForConfirmation) {
    if (Serial.available()) {
      char c = Serial.read();
      if (c == '\n' || c == '\r') {
        if (input.length() > 0) {
          input.trim();
          input.toLowerCase();
          if (input == "menu") {
            exitCurrentPractice();
            showMainMenu();
            return;
          }
        }
        waitingForConfirmation = false;
        Serial.println(F("Continuing..."));
        delay(500);
      } else if (c >= ' ') {
        input += c;
        Serial.print(c); // Echo input
      }
    }
  }
}

void pauseForReading(String title, int delayTime = 2000) {
  Serial.println(F(""));
  Serial.println(F("╔═══════════════════════════════════════════════════════════╗"));
  Serial.print(F("║ "));
  int padding = (57 - title.length()) / 2;
  for (int i = 0; i < padding; i++) Serial.print(F(" "));
  Serial.print(title);
  for (int i = 0; i < (57 - title.length() - padding); i++) Serial.print(F(" "));
  Serial.println(F(" ║"));
  Serial.println(F("╚═══════════════════════════════════════════════════════════╝"));
  Serial.println(F(""));
  Serial.print(F("Please read the above information. Auto-continuing in "));
  
  for (int i = delayTime/1000; i > 0; i--) {
    Serial.print(i);
    Serial.print(F("... "));
    delay(1000);
  }
  Serial.println(F(""));
  Serial.println(F(""));
}

// ============================================
// MENU SYSTEM
// ============================================
void showWelcomeMessage() {
  Serial.println(F(""));
  Serial.println(F("╔══════════════════════════════════════════════════════════╗"));
  Serial.println(F("║          ARDUINO NANO LEARNING SYSTEM v3.0              ║"));
  Serial.println(F("║         Step-by-Step Educational Experience             ║"));
  Serial.println(F("╚══════════════════════════════════════════════════════════╝"));
  Serial.println(F(""));
  Serial.println(F("Welcome to the comprehensive Arduino learning experience!"));
  Serial.println(F(""));
  Serial.println(F("🎯 LEARNING APPROACH:"));
  Serial.println(F("   • Each practice includes detailed explanations"));
  Serial.println(F("   • Step-by-step guided execution"));
  Serial.println(F("   • User confirmation at each major step"));
  Serial.println(F("   • Real-time feedback and status updates"));
  Serial.println(F(""));
  Serial.println(F("🔧 HARDWARE REQUIREMENTS:"));
  Serial.println(F("   • Arduino Nano with USB cable"));
  Serial.println(F("   • Breadboard and jumper wires"));
  Serial.println(F("   • LEDs, resistors, buttons, potentiometer"));
  Serial.println(F("   • Servo motor, RGB LED, 7-segment display"));
  Serial.println(F(""));
  Serial.println(F("📚 WHAT YOU'LL LEARN:"));
  Serial.println(F("   • Digital I/O control and GPIO management"));
  Serial.println(F("   • Analog input processing and ADC concepts"));
  Serial.println(F("   • PWM output and servo motor control"));
  Serial.println(F("   • Color theory and RGB LED programming"));
  Serial.println(F("   • Display multiplexing and timing control"));
  Serial.println(F(""));
}

void showMainMenu() {
  showMenu = true;
  practiceRunning = false;
  practiceActive = false;
  currentPractice = -1;
  
  Serial.println(F(""));
  Serial.println(F("═══════════════ PRACTICE SELECTION MENU ═══════════════"));
  Serial.println(F(""));
  Serial.println(F("Available Learning Practices:"));
  Serial.println(F(""));
  Serial.println(F("  1 → Digital GPIO Control"));
  Serial.println(F("      ⚡ Difficulty: Beginner"));
  Serial.println(F("      ⏱️  Duration: ~10 minutes"));
  Serial.println(F("      🎯 Focus: Basic I/O, LEDs, buttons"));
  Serial.println(F(""));
  Serial.println(F("  2 → Analog Input Processing"));
  Serial.println(F("      ⚡ Difficulty: Beginner"));
  Serial.println(F("      ⏱️  Duration: ~8 minutes"));
  Serial.println(F("      🎯 Focus: ADC, potentiometers, PWM"));
  Serial.println(F(""));
  Serial.println(F("  3 → PWM Output & Servo Control"));
  Serial.println(F("      ⚡ Difficulty: Intermediate"));
  Serial.println(F("      ⏱️  Duration: ~12 minutes"));
  Serial.println(F("      🎯 Focus: Servo motors, smooth control"));
  Serial.println(F(""));
  Serial.println(F("  4 → RGB LED Color Mixing"));
  Serial.println(F("      ⚡ Difficulty: Intermediate"));
  Serial.println(F("      ⏱️  Duration: ~15 minutes"));
  Serial.println(F("      🎯 Focus: Color theory, animations"));
  Serial.println(F(""));
  Serial.println(F("  5 → 7-Segment Display Control"));
  Serial.println(F("      ⚡ Difficulty: Advanced"));
  Serial.println(F("      ⏱️  Duration: ~20 minutes"));
  Serial.println(F("      🎯 Focus: Multiplexing, timing"));
  Serial.println(F(""));
  Serial.println(F("═══════════════════════════════════════════════════════"));
  Serial.println(F(""));
  Serial.println(F("📋 COMMANDS:"));
  Serial.println(F("  • Enter number (1-5) - Start practice"));
  Serial.println(F("  • 'info' - System information"));
  Serial.println(F("  • Hardware button - Emergency return to menu"));
  Serial.println(F(""));
  Serial.print(F("👉 Your selection: "));
}

// ============================================
// INPUT HANDLING
// ============================================
void handleSerialInput() {
  while (Serial.available()) {
    char inChar = Serial.read();
    
    if (inChar == '\n' || inChar == '\r') {
      if (inputBuffer.length() > 0) {
        processInput(inputBuffer);
        inputBuffer = "";
      }
    } else if (inChar >= ' ') {
      inputBuffer += inChar;
      if (!practiceRunning || waitingForConfirmation) {
        Serial.print(inChar); // Echo input in menu mode or when waiting
      }
    }
  }
}

void processInput(String input) {
  input.trim();
  input.toLowerCase();
  
  if (!practiceRunning) {
    // Menu mode - handle practice selection
    if (input == "1") startPractice(0);
    else if (input == "2") startPractice(1);
    else if (input == "3") startPractice(2);
    else if (input == "4") startPractice(3);
    else if (input == "5") startPractice(4);
    else if (input == "info") showSystemInfo();
    else {
      Serial.println();
      Serial.println(F("❌ Invalid selection. Please enter 1-5 or 'info'"));
      Serial.print(F("👉 Your selection: "));
    }
  } else if (!waitingForConfirmation) {
    // Practice mode - handle practice-specific commands
    handlePracticeCommand(input);
  }
}

void handleModeButton() {
  bool currentButtonState = digitalRead(MODE_BUTTON);
  unsigned long currentTime = millis();
  
  // Detect button press with debouncing
  if (currentButtonState && !lastButtonState && 
      (currentTime - lastButtonPress > 300)) {
    
    if (practiceRunning) {
      // Exit current practice
      Serial.println(F("\n🔴 HARDWARE BUTTON PRESSED - EMERGENCY EXIT"));
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
  practiceActive = false; // Start paused
  showMenu = false;
  
  // Show practice introduction
  showPracticeIntroduction(practiceNum);
  
  // Wait for user to confirm they're ready
  waitForUserConfirmation("READY TO START PRACTICE");
  
  // Turn off all outputs
  turnOffAllOutputs();
  
  // Configure pins for practice
  configurePinsForPractice(practiceNum);
  
  // Show hardware setup instructions
  showHardwareSetup(practiceNum);
  
  // Wait for hardware confirmation
  waitForUserConfirmation("HARDWARE SETUP COMPLETE");
  
  // Show practice header and commands
  showPracticeHeader(practiceNum);
  
  // Practice-specific initialization
  initializePracticeState(practiceNum);
  
  // Final confirmation before starting
  waitForUserConfirmation("START PRACTICE EXECUTION");
  
  // Now activate the practice
  practiceActive = true;
  
  Serial.println(F(""));
  Serial.println(F("🟢 PRACTICE IS NOW ACTIVE!"));
  Serial.println(F("Watch the hardware and serial output..."));
  Serial.println(F("Type commands or press hardware button to exit."));
  Serial.println(F(""));
}

void showPracticeIntroduction(int practice) {
  pauseForReading("PRACTICE INTRODUCTION", 3000);
  
  switch (practice) {
    case 0:
      Serial.println(F("🎯 PRACTICE 1: DIGITAL GPIO CONTROL"));
      Serial.println(F(""));
      Serial.println(F("📖 WHAT YOU'LL LEARN:"));
      Serial.println(F("   • How to configure Arduino pins as inputs/outputs"));
      Serial.println(F("   • Understanding digitalWrite() and digitalRead()"));
      Serial.println(F("   • Working with LEDs and current-limiting resistors"));
      Serial.println(F("   • Button input with pull-down resistors"));
      Serial.println(F("   • Basic timing and non-blocking delays"));
      Serial.println(F(""));
      Serial.println(F("🔬 CONCEPTS COVERED:"));
      Serial.println(F("   • pinMode() function and pin configuration"));
      Serial.println(F("   • Digital HIGH (5V) and LOW (0V) states"));
      Serial.println(F("   • Current flow and Ohm's law basics"));
      Serial.println(F("   • Pull-up and pull-down resistor theory"));
      Serial.println(F("   • Edge detection and button debouncing"));
      break;
      
    case 1:
      Serial.println(F("🎯 PRACTICE 2: ANALOG INPUT PROCESSING"));
      Serial.println(F(""));
      Serial.println(F("📖 WHAT YOU'LL LEARN:"));
      Serial.println(F("   • How Analog-to-Digital Conversion (ADC) works"));
      Serial.println(F("   • Reading variable voltages with analogRead()"));
      Serial.println(F("   • Understanding potentiometers as voltage dividers"));
      Serial.println(F("   • Converting digital values to PWM output"));
      Serial.println(F("   • Real-time sensor data processing"));
      Serial.println(F(""));
      Serial.println(F("🔬 CONCEPTS COVERED:"));
      Serial.println(F("   • 10-bit ADC resolution (0-1023 values)"));
      Serial.println(F("   • Voltage divider circuits"));
      Serial.println(F("   • PWM (Pulse Width Modulation) theory"));
      Serial.println(F("   • map() function for value scaling"));
      Serial.println(F("   • Analog vs digital signal processing"));
      break;
      
    case 2:
      Serial.println(F("🎯 PRACTICE 3: PWM OUTPUT & SERVO CONTROL"));
      Serial.println(F(""));
      Serial.println(F("📖 WHAT YOU'LL LEARN:"));
      Serial.println(F("   • Advanced PWM concepts and applications"));
      Serial.println(F("   • Servo motor control principles"));
      Serial.println(F("   • Simultaneous control of multiple outputs"));
      Serial.println(F("   • Smooth value transitions and interpolation"));
      Serial.println(F("   • Library usage and object-oriented programming"));
      Serial.println(F(""));
      Serial.println(F("🔬 CONCEPTS COVERED:"));
      Serial.println(F("   • PWM frequency and duty cycle"));
      Serial.println(F("   • Servo control signals (1-2ms pulses)"));
      Serial.println(F("   • Position feedback and control loops"));
      Serial.println(F("   • Multi-tasking with millis() timing"));
      Serial.println(F("   • Hardware abstraction with libraries"));
      break;
      
    case 3:
      Serial.println(F("🎯 PRACTICE 4: RGB LED COLOR MIXING"));
      Serial.println(F(""));
      Serial.println(F("📖 WHAT YOU'LL LEARN:"));
      Serial.println(F("   • RGB color theory and additive mixing"));
      Serial.println(F("   • Creating smooth color animations"));
      Serial.println(F("   • Non-blocking programming techniques"));
      Serial.println(F("   • Mathematical functions in programming"));
      Serial.println(F("   • State machines and mode switching"));
      Serial.println(F(""));
      Serial.println(F("🔬 CONCEPTS COVERED:"));
      Serial.println(F("   • Color spaces (RGB, HSV)"));
      Serial.println(F("   • Trigonometric functions (sin, cos)"));
      Serial.println(F("   • Animation timing and frame rates"));
      Serial.println(F("   • Multiple PWM channel coordination"));
      Serial.println(F("   • User interface design patterns"));
      break;
      
    case 4:
      Serial.println(F("🎯 PRACTICE 5: 7-SEGMENT DISPLAY CONTROL"));
      Serial.println(F(""));
      Serial.println(F("📖 WHAT YOU'LL LEARN:"));
      Serial.println(F("   • Display multiplexing techniques"));
      Serial.println(F("   • Time-critical programming"));
      Serial.println(F("   • Binary number representation"));
      Serial.println(F("   • Transistor switching circuits"));
      Serial.println(F("   • Real-time display refresh systems"));
      Serial.println(F(""));
      Serial.println(F("🔬 CONCEPTS COVERED:"));
      Serial.println(F("   • Persistence of vision principles"));
      Serial.println(F("   • Common cathode/anode configurations"));
      Serial.println(F("   • Bit manipulation and binary patterns"));
      Serial.println(F("   • Interrupt-free timing systems"));
      Serial.println(F("   • Memory-efficient data storage"));
      break;
  }
  
  pauseForReading("INTRODUCTION COMPLETE", 2000);
}

void showHardwareSetup(int practice) {
  pauseForReading("HARDWARE SETUP INSTRUCTIONS", 3000);
  
  Serial.println(F("🔧 REQUIRED COMPONENTS:"));
  Serial.println(F(""));
  
  switch (practice) {
    case 0:
      Serial.println(F("   📦 Components needed:"));
      Serial.println(F("      • 2x LEDs (Red, Green)"));
      Serial.println(F("      • 2x 220Ω resistors"));
      Serial.println(F("      • 1x Push button"));
      Serial.println(F("      • 1x 10kΩ resistor"));
      Serial.println(F("      • Breadboard and jumper wires"));
      Serial.println(F(""));
      Serial.println(F("   🔌 Connections:"));
      Serial.println(F("      Red LED:    D3 → 220Ω → LED+ → LED- → GND"));
      Serial.println(F("      Green LED:  D4 → 220Ω → LED+ → LED- → GND"));
      Serial.println(F("      Button:     D5 ← Button ← 10kΩ ← GND"));
      Serial.println(F("                  Button other terminal → 5V"));
      break;
      
    case 1:
      Serial.println(F("   📦 Components needed:"));
      Serial.println(F("      • 1x 10kΩ Potentiometer"));
      Serial.println(F("      • 1x LED"));
      Serial.println(F("      • 1x 220Ω resistor"));
      Serial.println(F(""));
      Serial.println(F("   🔌 Connections:"));
      Serial.println(F("      Potentiometer: Terminal 1 → 5V"));
      Serial.println(F("                     Terminal 2 (middle) → A2"));
      Serial.println(F("                     Terminal 3 → GND"));
      Serial.println(F("      LED:           D6 → 220Ω → LED+ → LED- → GND"));
      break;
      
    case 2:
      Serial.println(F("   📦 Components needed:"));
      Serial.println(F("      • 1x Servo motor (SG90)"));
      Serial.println(F("      • 1x LED + 220Ω resistor"));
      Serial.println(F("      • 1x 10kΩ Potentiometer"));
      Serial.println(F(""));
      Serial.println(F("   🔌 Connections:"));
      Serial.println(F("      Servo:         Signal (Orange) → D9"));
      Serial.println(F("                     Power (Red) → 5V"));
      Serial.println(F("                     Ground (Brown) → GND"));
      Serial.println(F("      LED:           D6 → 220Ω → LED+ → LED- → GND"));
      Serial.println(F("      Potentiometer: Same as Practice 2"));
      break;
      
    case 3:
      Serial.println(F("   📦 Components needed:"));
      Serial.println(F("      • 1x RGB LED (Common Cathode)"));
      Serial.println(F("      • 3x 220Ω resistors"));
      Serial.println(F(""));
      Serial.println(F("   🔌 Connections:"));
      Serial.println(F("      RGB Red:    D9 → 220Ω → Red pin"));
      Serial.println(F("      RGB Green:  D10 → 220Ω → Green pin"));
      Serial.println(F("      RGB Blue:   D11 → 220Ω → Blue pin"));
      Serial.println(F("      RGB Common: Common Cathode → GND"));
      break;
      
    case 4:
      Serial.println(F("   📦 Components needed:"));
      Serial.println(F("      • 1x 7SEG-MPX4-CC (4-digit 7-segment)"));
      Serial.println(F("      • 8x 220Ω resistors"));
      Serial.println(F("      • 4x NPN transistors (2N2222)"));
      Serial.println(F("      • 4x 1kΩ resistors"));
      Serial.println(F(""));
      Serial.println(F("   🔌 Connections:"));
      Serial.println(F("      Segments:   D3-D10 → 220Ω → Segments A-G,DP"));
      Serial.println(F("      Digits:     D11,D12,A0,A1 → 1kΩ → Transistor"));
      Serial.println(F("      ⚠️  Complex wiring - refer to detailed diagram"));
      break;
  }
  
  Serial.println(F(""));
  Serial.println(F("⚠️  SAFETY REMINDERS:"));
  Serial.println(F("   • Double-check all connections"));
  Serial.println(F("   • Ensure correct LED polarity"));
  Serial.println(F("   • Verify resistor values"));
  Serial.println(F("   • No short circuits between power rails"));
  Serial.println(F(""));
  
  pauseForReading("SETUP INSTRUCTIONS COMPLETE", 3000);
}

void showPracticeHeader(int practice) {
  Serial.println(F("═══════════════════════════════════════════════════════"));
  
  switch (practice) {
    case 0:
      Serial.println(F("           🚀 PRACTICE 1 EXECUTION"));
      Serial.println(F("═══════════════════════════════════════════════════════"));
      Serial.println(F("Available Commands:"));
      Serial.println(F("• 'f' - Fast blink mode (100ms)"));
      Serial.println(F("• 'n' - Normal blink mode (500ms)"));
      Serial.println(F("• 's' - Slow blink mode (1000ms)"));
      Serial.println(F("• 'status' - Show current status"));
      Serial.println(F("• 'pause' - Pause practice execution"));
      break;
      
    case 1:
      Serial.println(F("           🚀 PRACTICE 2 EXECUTION"));
      Serial.println(F("═══════════════════════════════════════════════════════"));
      Serial.println(F("Available Commands:"));
      Serial.println(F("• 'status' - Show current readings"));
      Serial.println(F("• 'pause' - Pause practice execution"));
      break;
      
    case 2:
      Serial.println(F("           🚀 PRACTICE 3 EXECUTION"));
      Serial.println(F("═══════════════════════════════════════════════════════"));
      Serial.println(F("Available Commands:"));
      Serial.println(F("• 'center' - Center servo at 90°"));
      Serial.println(F("• 'test' - Test servo full range"));
      Serial.println(F("• 'status' - Show current positions"));
      Serial.println(F("• 'pause' - Pause practice execution"));
      break;
      
    case 3:
      Serial.println(F("           🚀 PRACTICE 4 EXECUTION"));
      Serial.println(F("═══════════════════════════════════════════════════════"));
      Serial.println(F("Available Commands:"));
      Serial.println(F("• 'mode' - Change color animation mode"));
      Serial.println(F("• 'faster' - Increase animation speed"));
      Serial.println(F("• 'slower' - Decrease animation speed"));
      Serial.println(F("• 'status' - Show current color values"));
      Serial.println(F("• 'pause' - Pause practice execution"));
      break;
      
    case 4:
      Serial.println(F("           🚀 PRACTICE 5 EXECUTION"));
      Serial.println(F("═══════════════════════════════════════════════════════"));
      Serial.println(F("Available Commands:"));
      Serial.println(F("• Enter number (0-9999) - Display the number"));
      Serial.println(F("• 'clear' - Clear display"));
      Serial.println(F("• 'zeros' - Toggle leading zeros"));
      Serial.println(F("• 'test' - Test all segments"));
      Serial.println(F("• 'pause' - Pause practice execution"));
      break;
  }
  
  Serial.println(F("═══════════════════════════════════════════════════════"));
}

void exitCurrentPractice() {
  Serial.println();
  Serial.println(F("🔴 EXITING PRACTICE"));
  Serial.println(F("═══════════════════════════════════════════════════════"));
  Serial.println(F("Cleaning up resources..."));
  
  // Clean up resources
  turnOffAllOutputs();
  if (myServo.attached()) {
    myServo.detach();
    Serial.println(F("✓ Servo detached"));
  }
  
  practiceRunning = false;
  practiceActive = false;
  currentPractice = -1;
  waitingForConfirmation = false;
  
  Serial.println(F("✓ All outputs turned off"));
  Serial.println(F("✓ Practice state reset"));
  Serial.println(F("Returning to main menu..."));
  
  delay(1500);
}

void turnOffAllOutputs() {
  for (int pin = 3; pin <= 12; pin++) {
    digitalWrite(pin, LOW);
  }
  digitalWrite(A0, LOW);
  digitalWrite(A1, LOW);
}

void configurePinsForPractice(int practice) {
  switch (practice) {
    case 0: // Practice 1
      pinMode(SHARED_D3, OUTPUT);  // Red LED
      pinMode(SHARED_D4, OUTPUT);  // Green LED
      pinMode(SHARED_D5, INPUT);   // Button
      Serial.println(F("✓ Pins configured for Digital GPIO"));
      break;
      
    case 1: // Practice 2
      pinMode(SHARED_D6, OUTPUT);  // LED brightness
      Serial.println(F("✓ Pins configured for Analog Input"));
      break;
      
    case 2: // Practice 3
      pinMode(SHARED_D6, OUTPUT);  // LED brightness
      myServo.attach(SHARED_D9);   // Servo
      myServo.write(90);
      Serial.println(F("✓ Pins configured for PWM & Servo"));
      break;
      
    case 3: // Practice 4
      pinMode(SHARED_D9, OUTPUT);  // RGB Red
      pinMode(SHARED_D10, OUTPUT); // RGB Green
      pinMode(SHARED_D11, OUTPUT); // RGB Blue
      Serial.println(F("✓ Pins configured for RGB LED"));
      break;
      
    case 4: // Practice 5
      for (int i = 3; i <= 12; i++) {
        pinMode(i, OUTPUT);
        digitalWrite(i, LOW);
      }
      pinMode(SHARED_A0, OUTPUT);
      pinMode(SHARED_A1, OUTPUT);
      Serial.println(F("✓ Pins configured for 7-Segment Display"));
      break;
  }
}

void initializePracticeState(int practice) {
  switch (practice) {
    case 0:
      state.blinkInterval = 500;
      state.blinkMode = 0;
      Serial.println(F("🔴 Red LED will blink every 500ms"));
      Serial.println(F("🟢 Press button to toggle green LED"));
      break;
      
    case 1:
      Serial.println(F("🎛️  Turn potentiometer to see LED brightness change"));
      Serial.println(F("📊 Watch serial output for real-time values"));
      break;
      
    case 2:
      state.servoPosition = 90;
      state.targetServoPosition = 90;
      Serial.println(F("🎛️  Potentiometer controls both LED and servo"));
      Serial.println(F("🔄 Servo will move smoothly to target position"));
      break;
      
    case 3:
      state.colorMode = 0;
      state.colorStep = 0;
      Serial.println(F("🌈 Starting rainbow color animation"));
      Serial.println(F("✨ Watch the RGB LED cycle through colors"));
      break;
      
    case 4:
      state.displayNumber = 1234;
      state.currentDigit = 0;
      Serial.println(F("🔢 Initial display: 1234"));
      Serial.println(F("⌨️  Type numbers to see them displayed"));
      break;
  }
}

// ============================================
// PRACTICE EXECUTION (Same as before but with pause capability)
// ============================================
void runCurrentPractice() {
  switch (currentPractice) {
    case 0: runPractice1(); break;
    case 1: runPractice2(); break;
    case 2: runPractice3(); break;
    case 3: runPractice4(); break;
    case 4: runPractice5(); break;
  }
  
  // Update status LED based on practice
  updateStatusLED();
}

void runPractice1() {
  unsigned long currentTime = millis();
  
  // Handle red LED blinking
  if (currentTime - state.lastBlinkTime >= state.blinkInterval) {
    state.redLedState = !state.redLedState;
    digitalWrite(SHARED_D3, state.redLedState);
    state.lastBlinkTime = currentTime;
    
    Serial.print(F("🔴 Red LED: "));
    Serial.println(state.redLedState ? F("ON") : F("OFF"));
  }
  
  // Handle button input for green LED
  bool currentButtonState = digitalRead(SHARED_D5);
  if (currentButtonState && !state.lastInputButtonState) {
    state.greenLedState = !state.greenLedState;
    digitalWrite(SHARED_D4, state.greenLedState);
    
    Serial.print(F("🟢 Button pressed! Green LED: "));
    Serial.println(state.greenLedState ? F("ON") : F("OFF"));
  }
  state.lastInputButtonState = currentButtonState;
}

void runPractice2() {
  int potValue = analogRead(POTENTIOMETER);
  state.brightness = map(potValue, 0, 1023, 0, 255);
  analogWrite(SHARED_D6, state.brightness);
  
  // Print values if changed significantly
  if (abs(potValue - state.lastPotValue) > 10) {
    Serial.print(F("📊 Pot: "));
    Serial.print(potValue);
    Serial.print(F(" | PWM: "));
    Serial.print(state.brightness);
    Serial.print(F(" | Brightness: "));
    Serial.print(map(potValue, 0, 1023, 0, 100));
    Serial.println(F("%"));
    
    state.lastPotValue = potValue;
  }
}

void runPractice3() {
  unsigned long currentTime = millis();
  
  int potValue = analogRead(POTENTIOMETER);
  
  // Control LED brightness
  state.brightness = map(potValue, 0, 1023, 0, 255);
  analogWrite(SHARED_D6, state.brightness);
  
  // Control servo position
  state.targetServoPosition = map(potValue, 0, 1023, 0, 180);
  
  // Update servo position smoothly
  if (currentTime - state.lastServoUpdate >= 15) {
    if (state.servoPosition < state.targetServoPosition) {
      state.servoPosition++;
    } else if (state.servoPosition > state.targetServoPosition) {
      state.servoPosition--;
    }
    myServo.write(state.servoPosition);
    state.lastServoUpdate = currentTime;
  }
  
  // Print status occasionally
  static unsigned long lastPrint = 0;
  if (currentTime - lastPrint > 1000) {
    Serial.print(F("📊 Pot: "));
    Serial.print(potValue);
    Serial.print(F(" | LED: "));
    Serial.print(state.brightness);
    Serial.print(F(" | Servo: "));
    Serial.print(state.servoPosition);
    Serial.println(F("°"));
    lastPrint = currentTime;
  }
}

void runPractice4() {
  unsigned long currentTime = millis();
  
  // Update colors every 50ms
  if (currentTime - state.lastColorUpdate >= 50) {
    updateRGBColors();
    state.lastColorUpdate = currentTime;
    state.colorStep++;
    
    // Print color values occasionally
    static int lastPrintStep = 0;
    if (state.colorStep - lastPrintStep >= 100) {
      Serial.print(F("🌈 RGB: ("));
      Serial.print(state.redValue);
      Serial.print(F(", "));
      Serial.print(state.greenValue);
      Serial.print(F(", "));
      Serial.print(state.blueValue);
      Serial.println(F(")"));
      lastPrintStep = state.colorStep;
    }
  }
  
  // Apply colors to RGB LED
  analogWrite(SHARED_D9, state.redValue);
  analogWrite(SHARED_D10, state.greenValue);
  analogWrite(SHARED_D11, state.blueValue);
}

void runPractice5() {
  unsigned long currentTime = millis();
  
  // Update display multiplexing
  if (currentTime - state.lastDisplayUpdate >= 2) {
    updateSevenSegmentDisplay();
    state.lastDisplayUpdate = currentTime;
  }
}

// ============================================
// PRACTICE-SPECIFIC COMMAND HANDLERS
// ============================================
void handlePracticeCommand(String command) {
  // Handle pause command for all practices
  if (command == "pause") {
    practiceActive = false;
    Serial.println(F(""));
    Serial.println(F("⏸️  PRACTICE PAUSED"));
    Serial.println(F("Type 'resume' to continue or 'menu' to exit"));
    return;
  }
  
  if (command == "resume") {
    practiceActive = true;
    Serial.println(F(""));
    Serial.println(F("▶️  PRACTICE RESUMED"));
    Serial.println(F(""));
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
        Serial.println(F("⚡ Blink mode: FAST (100ms)"));
      } else if (command == "n") {
        state.blinkInterval = 500;
        Serial.println(F("🔄 Blink mode: NORMAL (500ms)"));
      } else if (command == "s") {
        state.blinkInterval = 1000;
        Serial.println(F("🐌 Blink mode: SLOW (1000ms)"));
      } else if (command == "status") {
        showPractice1Status();
      } else {
        Serial.println(F("❌ Unknown command. Try: f, n, s, status, pause"));
      }
      break;
      
    case 1: // Practice 2
      if (command == "status") {
        showPractice2Status();
      } else {
        Serial.println(F("❌ Unknown command. Try: status, pause"));
      }
      break;
      
    case 2: // Practice 3
      if (command == "center") {
        state.targetServoPosition = 90;
        Serial.println(F("🎯 Centering servo to 90°..."));
      } else if (command == "test") {
        practiceActive = false; // Pause during test
        testServoRange();
        practiceActive = true; // Resume after test
      } else if (command == "status") {
        showPractice3Status();
      } else {
        Serial.println(F("❌ Unknown command. Try: center, test, status, pause"));
      }
      break;
      
    case 3: // Practice 4
      if (command == "mode") {
        state.colorMode = (state.colorMode + 1) % 3;
        state.colorStep = 0;
        Serial.print(F("🎨 Color mode: "));
        switch (state.colorMode) {
          case 0: Serial.println(F("Rainbow")); break;
          case 1: Serial.println(F("Breathing")); break;
          case 2: Serial.println(F("Strobe")); break;
        }
      } else if (command == "faster") {
        Serial.println(F("⚡ Animation speed increased"));
      } else if (command == "slower") {
        Serial.println(F("🐌 Animation speed decreased"));
      } else if (command == "status") {
        showPractice4Status();
      } else {
        Serial.println(F("❌ Unknown command. Try: mode, faster, slower, status, pause"));
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
        practiceActive = false; // Pause during test
        testSevenSegmentDisplay();
        practiceActive = true; // Resume after test
      } else {
        // Try to parse as number
        int number = command.toInt();
        if (number >= 0 && number <= 9999 || command == "0") {
          state.displayNumber = number;
          Serial.print(F("🔢 Displaying: "));
          Serial.println(state.displayNumber);
        } else {
          Serial.println(F("❌ Enter number (0-9999) or: clear, zeros, test, pause"));
        }
      }
      break;
  }
}

// ============================================
// HELPER FUNCTIONS (Same as before)
// ============================================
void updateRGBColors() {
  int phase = state.colorStep % 360;
  
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
        int brightness = (sin(state.colorStep * 0.1) + 1) * 127.5;
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
          case 7: break; // Off
        }
      }
      break;
  }
}

void updateSevenSegmentDisplay() {
  // Turn off all digits
  digitalWrite(SHARED_D11, LOW);
  digitalWrite(SHARED_D12, LOW);
  digitalWrite(SHARED_A0, LOW);
  digitalWrite(SHARED_A1, LOW);
  
  // Extract digits
  int thousands = (state.displayNumber / 1000) % 10;
  int hundreds = (state.displayNumber / 100) % 10;
  int tens = (state.displayNumber / 10) % 10;
  int units = state.displayNumber % 10;
  
  int digitValue;
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
  // Different blink patterns for each practice
  int interval = 200 + (currentPractice * 200); // 200, 400, 600, 800, 1000ms
  digitalWrite(STATUS_LED, (millis() / interval) % 2);
}

// ============================================
// STATUS DISPLAY FUNCTIONS
// ============================================
void showPractice1Status() {
  Serial.println(F(""));
  Serial.println(F("📊 --- Practice 1 Status ---"));
  Serial.print(F("🔴 Red LED: "));
  Serial.println(state.redLedState ? F("ON") : F("OFF"));
  Serial.print(F("🟢 Green LED: "));
  Serial.println(state.greenLedState ? F("ON") : F("OFF"));
  Serial.print(F("⏱️  Blink Interval: "));
  Serial.print(state.blinkInterval);
  Serial.println(F("ms"));
  Serial.print(F("🔘 Button State: "));
  Serial.println(digitalRead(SHARED_D5) ? F("PRESSED") : F("RELEASED"));
  Serial.println();
}

void showPractice2Status() {
  int potValue = analogRead(POTENTIOMETER);
  Serial.println(F(""));
  Serial.println(F("📊 --- Practice 2 Status ---"));
  Serial.print(F("🎛️  Potentiometer: "));
  Serial.print(potValue);
  Serial.print(F(" ("));
  Serial.print((potValue / 1023.0) * 5.0, 2);
  Serial.println(F("V)"));
  Serial.print(F("💡 PWM Value: "));
  Serial.print(state.brightness);
  Serial.print(F(" ("));
  Serial.print((state.brightness / 255.0) * 100, 1);
  Serial.println(F("%)"));
  Serial.println();
}

void showPractice3Status() {
  Serial.println(F(""));
  Serial.println(F("📊 --- Practice 3 Status ---"));
  Serial.print(F("💡 LED Brightness: "));
  Serial.print(state.brightness);
  Serial.print(F(" ("));
  Serial.print((state.brightness / 255.0) * 100, 1);
  Serial.println(F("%)"));
  Serial.print(F("🔄 Servo Position: "));
  Serial.print(state.servoPosition);
  Serial.print(F("° (Target: "));
  Serial.print(state.targetServoPosition);
  Serial.println(F("°)"));
  Serial.println();
}

void showPractice4Status() {
  Serial.println(F(""));
  Serial.println(F("📊 --- Practice 4 Status ---"));
  Serial.print(F("🎨 Color Mode: "));
  switch (state.colorMode) {
    case 0: Serial.println(F("Rainbow")); break;
    case 1: Serial.println(F("Breathing")); break;
    case 2: Serial.println(F("Strobe")); break;
  }
  Serial.print(F("🌈 RGB Values: R="));
  Serial.print(state.redValue);
  Serial.print(F(" G="));
  Serial.print(state.greenValue);
  Serial.print(F(" B="));
  Serial.println(state.blueValue);
  Serial.print(F("🎬 Animation Step: "));
  Serial.println(state.colorStep);
  Serial.println();
}

void testServoRange() {
  Serial.println(F(""));
  Serial.println(F("🧪 Testing servo full range..."));
  Serial.println(F("📍 Moving to 0°"));
  for (int pos = state.servoPosition; pos >= 0; pos--) {
    myServo.write(pos);
    delay(15);
  }
  delay(1000);
  
  Serial.println(F("📍 Moving to 180°"));
  for (int pos = 0; pos <= 180; pos++) {
    myServo.write(pos);
    delay(15);
  }
  delay(1000);
  
  Serial.println(F("📍 Returning to center"));
  for (int pos = 180; pos >= 90; pos--) {
    myServo.write(pos);
    delay(15);
  }
  
  state.servoPosition = 90;
  state.targetServoPosition = 90;
  Serial.println(F("✅ Servo test complete"));
  Serial.println();
}

void testSevenSegmentDisplay() {
  Serial.println(F(""));
  Serial.println(F("🧪 Testing 7-segment display..."));
  for (int i = 0; i <= 9; i++) {
    state.displayNumber = i * 1111; // 0000, 1111, 2222, etc.
    Serial.print(F("🔢 Showing: "));
    Serial.println(state.displayNumber);
    delay(1000);
  }
  state.displayNumber = 1234;
  Serial.println(F("✅ Test complete - showing 1234"));
  Serial.println();
}

void showSystemInfo() {
  Serial.println();
  Serial.println(F("═══════════════ SYSTEM INFORMATION ═══════════════"));
  Serial.println(F("🔧 Hardware: Arduino Nano (ATmega328P)"));
  Serial.println(F("⚡ Clock Speed: 16 MHz"));
  Serial.println(F("💾 Flash Memory: 32 KB"));
  Serial.println(F("🧠 SRAM: 2 KB"));
  Serial.println(F("💿 EEPROM: 1 KB"));
  Serial.println();
  Serial.print(F(" Uptime: "));
  Serial.print(millis() / 1000);
  Serial.println(F(" seconds"));
  Serial.print(F("🆓 Free RAM: "));
  Serial.print(getFreeRAM());
  Serial.println(F(" bytes"));
  Serial.println(F("═══════════════════════════════════════════════════"));
  Serial.println();
  Serial.print(F(" Your selection: "));
}

int getFreeRAM() {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}
