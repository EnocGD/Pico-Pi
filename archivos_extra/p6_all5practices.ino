// ============================================
// ARDUINO NANO LEARNING SYSTEM - EDUCATIONAL VERSION
// Memory Optimized with User-Friendly Interface
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
bool showMenu = true;
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
  
  // Wait for serial connection
  delay(1000);
  
  // Show welcome message
  showWelcomeMessage();
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
  
  // Run current practice or show menu
  if (practiceRunning && currentPractice >= 0) {
    runCurrentPractice();
  } else if (showMenu) {
    // In menu mode - just blink status LED slowly
    digitalWrite(STATUS_LED, (millis() / 1000) % 2);
  }
}

// ============================================
// MENU SYSTEM
// ============================================
void showWelcomeMessage() {
  Serial.println(F(""));
  Serial.println(F("╔══════════════════════════════════════════════════════════╗"));
  Serial.println(F("║          ARDUINO NANO LEARNING SYSTEM v2.0              ║"));
  Serial.println(F("║              Educational Microcontroller Kit             ║"));
  Serial.println(F("╚══════════════════════════════════════════════════════════╝"));
  Serial.println(F(""));
  Serial.println(F("Welcome to the comprehensive Arduino learning experience!"));
  Serial.println(F("This system teaches fundamental microcontroller concepts."));
  Serial.println(F(""));
}

void showMainMenu() {
  showMenu = true;
  practiceRunning = false;
  currentPractice = -1;
  
  Serial.println(F(""));
  Serial.println(F("═══════════════ PRACTICE SELECTION MENU ═══════════════"));
  Serial.println(F(""));
  Serial.println(F("Available Learning Practices:"));
  Serial.println(F(""));
  Serial.println(F("  1 → Digital GPIO Control"));
  Serial.println(F("      Learn: pinMode(), digitalWrite(), digitalRead()"));
  Serial.println(F("      Hardware: LEDs, buttons, pull-up/down resistors"));
  Serial.println(F(""));
  Serial.println(F("  2 → Analog Input Processing"));
  Serial.println(F("      Learn: analogRead(), PWM output, voltage dividers"));
  Serial.println(F("      Hardware: Potentiometer, LED brightness control"));
  Serial.println(F(""));
  Serial.println(F("  3 → PWM Output & Servo Control"));
  Serial.println(F("      Learn: analogWrite(), Servo library, smooth control"));
  Serial.println(F("      Hardware: Servo motor, LED dimming"));
  Serial.println(F(""));
  Serial.println(F("  4 → RGB LED Color Mixing"));
  Serial.println(F("      Learn: Color theory, interrupts, non-blocking code"));
  Serial.println(F("      Hardware: RGB LED, color animations"));
  Serial.println(F(""));
  Serial.println(F("  5 → 7-Segment Display Control"));
  Serial.println(F("      Learn: Multiplexing, display drivers, number systems"));
  Serial.println(F("      Hardware: 4-digit 7-segment display"));
  Serial.println(F(""));
  Serial.println(F("═══════════════════════════════════════════════════════"));
  Serial.println(F(""));
  Serial.println(F("Enter practice number (1-5) or command:"));
  Serial.println(F("  • 'menu' - Show this menu"));
  Serial.println(F("  • 'info' - System information"));
  Serial.println(F("  • Hardware button - Return to menu anytime"));
  Serial.println(F(""));
  Serial.print(F("Selection: "));
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
      if (!practiceRunning) {
        Serial.print(inChar); // Echo input in menu mode
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
    else if (input == "menu") showMainMenu();
    else if (input == "info") showSystemInfo();
    else {
      Serial.println();
      Serial.println(F("Invalid selection. Please enter 1-5 or 'menu'"));
      Serial.print(F("Selection: "));
    }
  } else {
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
  showMenu = false;
  
  // Turn off all outputs
  turnOffAllOutputs();
  
  // Configure pins for practice
  configurePinsForPractice(practiceNum);
  
  // Show practice header
  showPracticeHeader(practiceNum);
  
  // Practice-specific initialization
  initializePracticeState(practiceNum);
}

void exitCurrentPractice() {
  Serial.println();
  Serial.println(F("═══════════════════════════════════════════════════════"));
  Serial.println(F("Exiting practice... Returning to main menu."));
  
  // Clean up resources
  turnOffAllOutputs();
  if (myServo.attached()) {
    myServo.detach();
  }
  
  practiceRunning = false;
  currentPractice = -1;
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
      break;
      
    case 1: // Practice 2
      pinMode(SHARED_D6, OUTPUT);  // LED brightness
      break;
      
    case 2: // Practice 3
      pinMode(SHARED_D6, OUTPUT);  // LED brightness
      myServo.attach(SHARED_D9);   // Servo
      myServo.write(90);
      break;
      
    case 3: // Practice 4
      pinMode(SHARED_D9, OUTPUT);  // RGB Red
      pinMode(SHARED_D10, OUTPUT); // RGB Green
      pinMode(SHARED_D11, OUTPUT); // RGB Blue
      break;
      
    case 4: // Practice 5
      for (int i = 3; i <= 12; i++) {
        pinMode(i, OUTPUT);
        digitalWrite(i, LOW);
      }
      pinMode(SHARED_A0, OUTPUT);
      pinMode(SHARED_A1, OUTPUT);
      break;
  }
}

void showPracticeHeader(int practice) {
  Serial.println(F("═══════════════════════════════════════════════════════"));
  
  switch (practice) {
    case 0:
      Serial.println(F("           PRACTICE 1: DIGITAL GPIO CONTROL"));
      Serial.println(F("═══════════════════════════════════════════════════════"));
      Serial.println(F("Learning Objectives:"));
      Serial.println(F("• Understand digital I/O pin configuration"));
      Serial.println(F("• Master pinMode(), digitalWrite(), digitalRead()"));
      Serial.println(F("• Learn about pull-up/pull-down resistors"));
      Serial.println(F("• Practice with LED control and button input"));
      Serial.println();
      Serial.println(F("Hardware Setup:"));
      Serial.println(F("• Red LED: Pin D3 → 220Ω → LED → GND"));
      Serial.println(F("• Green LED: Pin D4 → 220Ω → LED → GND"));
      Serial.println(F("• Button: Pin D5 ← Button ← 10kΩ ← GND"));
      Serial.println();
      Serial.println(F("Available Commands:"));
      Serial.println(F("• 'f' - Fast blink mode (100ms)"));
      Serial.println(F("• 'n' - Normal blink mode (500ms)"));
      Serial.println(F("• 's' - Slow blink mode (1000ms)"));
      Serial.println(F("• 'status' - Show current status"));
      break;
      
    case 1:
      Serial.println(F("          PRACTICE 2: ANALOG INPUT PROCESSING"));
      Serial.println(F("═══════════════════════════════════════════════════════"));
      Serial.println(F("Learning Objectives:"));
      Serial.println(F("• Understand Analog-to-Digital Conversion (ADC)"));
      Serial.println(F("• Learn analogRead() and analogWrite() functions"));
      Serial.println(F("• Work with potentiometers as voltage dividers"));
      Serial.println(F("• Practice PWM for analog output simulation"));
      Serial.println();
      Serial.println(F("Hardware Setup:"));
      Serial.println(F("• Potentiometer: A2 ← Middle pin, Outer pins to 5V/GND"));
      Serial.println(F("• LED: Pin D6 → 220Ω → LED → GND"));
      Serial.println();
      Serial.println(F("Available Commands:"));
      Serial.println(F("• 'status' - Show current readings"));
      break;
      
    case 2:
      Serial.println(F("         PRACTICE 3: PWM OUTPUT & SERVO CONTROL"));
      Serial.println(F("═══════════════════════════════════════════════════════"));
      Serial.println(F("Learning Objectives:"));
      Serial.println(F("• Understand Pulse Width Modulation (PWM)"));
      Serial.println(F("• Learn Servo library and position control"));
      Serial.println(F("• Practice simultaneous output control"));
      Serial.println(F("• Master smooth value transitions"));
      Serial.println();
      Serial.println(F("Hardware Setup:"));
      Serial.println(F("• Servo: Signal→D9, Power→5V, Ground→GND"));
      Serial.println(F("• LED: Pin D6 → 220Ω → LED → GND"));
      Serial.println(F("• Potentiometer: A2 ← Middle pin, Outer pins to 5V/GND"));
      Serial.println();
      Serial.println(F("Available Commands:"));
      Serial.println(F("• 'center' - Center servo at 90°"));
      Serial.println(F("• 'test' - Test servo full range"));
      Serial.println(F("• 'status' - Show current positions"));
      break;
      
    case 3:
      Serial.println(F("          PRACTICE 4: RGB LED COLOR MIXING"));
      Serial.println(F("═══════════════════════════════════════════════════════"));
      Serial.println(F("Learning Objectives:"));
      Serial.println(F("• Learn RGB color theory and mixing"));
      Serial.println(F("• Practice non-blocking programming"));
      Serial.println(F("• Understand multiple PWM outputs"));
      Serial.println(F("• Master color animations and transitions"));
      Serial.println();
      Serial.println(F("Hardware Setup:"));
      Serial.println(F("• RGB LED Red: Pin D9 → 220Ω → LED → GND"));
      Serial.println(F("• RGB LED Green: Pin D10 → 220Ω → LED → GND"));
      Serial.println(F("• RGB LED Blue: Pin D11 → 220Ω → LED → GND"));
      Serial.println();
      Serial.println(F("Available Commands:"));
      Serial.println(F("• 'mode' - Change color animation mode"));
      Serial.println(F("• 'faster' - Increase animation speed"));
      Serial.println(F("• 'slower' - Decrease animation speed"));
      Serial.println(F("• 'status' - Show current color values"));
      break;
      
    case 4:
      Serial.println(F("         PRACTICE 5: 7-SEGMENT DISPLAY CONTROL"));
      Serial.println(F("═══════════════════════════════════════════════════════"));
      Serial.println(F("Learning Objectives:"));
      Serial.println(F("• Understand display multiplexing techniques"));
      Serial.println(F("• Learn about display drivers and timing"));
      Serial.println(F("• Practice with number systems and encoding"));
      Serial.println(F("• Master real-time display updates"));
      Serial.println();
      Serial.println(F("Hardware Setup:"));
      Serial.println(F("• 7SEG-MPX4-CC: 4-digit 7-segment display"));
      Serial.println(F("• Segments A-G,DP: D3-D10 → 220Ω → Display"));
      Serial.println(F("• Digit Control: D11,D12,A0,A1 → Transistors"));
      Serial.println();
      Serial.println(F("Available Commands:"));
      Serial.println(F("• Enter number (0-9999) - Display the number"));
      Serial.println(F("• 'clear' - Clear display"));
      Serial.println(F("• 'zeros' - Toggle leading zeros"));
      Serial.println(F("• 'test' - Test all segments"));
      break;
  }
  
  Serial.println(F("═══════════════════════════════════════════════════════"));
  Serial.println(F("Press hardware button anytime to return to main menu"));
  Serial.println(F("═══════════════════════════════════════════════════════"));
  Serial.println();
}

void initializePracticeState(int practice) {
  switch (practice) {
    case 0:
      state.blinkInterval = 500;
      state.blinkMode = 0;
      Serial.println(F("Red LED will start blinking automatically..."));
      Serial.println(F("Press the button to toggle the green LED."));
      break;
      
    case 1:
      Serial.println(F("Turn the potentiometer to control LED brightness..."));
      break;
      
    case 2:
      state.servoPosition = 90;
      state.targetServoPosition = 90;
      Serial.println(F("Turn potentiometer to control LED and servo..."));
      break;
      
    case 3:
      state.colorMode = 0;
      state.colorStep = 0;
      Serial.println(F("RGB LED will start rainbow animation..."));
      break;
      
    case 4:
      state.displayNumber = 1234;
      state.currentDigit = 0;
      Serial.println(F("Displaying initial number: 1234"));
      Serial.println(F("Enter a number (0-9999) to display:"));
      break;
  }
  Serial.println();
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
    
    Serial.print(F("Red LED: "));
    Serial.println(state.redLedState ? F("ON") : F("OFF"));
  }
  
  // Handle button input for green LED
  bool currentButtonState = digitalRead(SHARED_D5);
  if (currentButtonState && !state.lastInputButtonState) {
    state.greenLedState = !state.greenLedState;
    digitalWrite(SHARED_D4, state.greenLedState);
    
    Serial.print(F("Button pressed! Green LED: "));
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
    Serial.print(F("Potentiometer: "));
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
  if (currentTime - lastPrint > 500) {
    Serial.print(F("Pot: "));
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
  switch (currentPractice) {
    case 0: // Practice 1
      if (command == "f") {
        state.blinkInterval = 100;
        Serial.println(F("Blink mode: FAST (100ms)"));
      } else if (command == "n") {
        state.blinkInterval = 500;
        Serial.println(F("Blink mode: NORMAL (500ms)"));
      } else if (command == "s") {
        state.blinkInterval = 1000;
        Serial.println(F("Blink mode: SLOW (1000ms)"));
      } else if (command == "status") {
        showPractice1Status();
      } else {
        Serial.println(F("Unknown command. Try: f, n, s, status"));
      }
      break;
      
    case 1: // Practice 2
      if (command == "status") {
        showPractice2Status();
      } else {
        Serial.println(F("Unknown command. Try: status"));
      }
      break;
      
    case 2: // Practice 3
      if (command == "center") {
        state.targetServoPosition = 90;
        Serial.println(F("Centering servo to 90°..."));
      } else if (command == "test") {
        testServoRange();
      } else if (command == "status") {
        showPractice3Status();
      } else {
        Serial.println(F("Unknown command. Try: center, test, status"));
      }
      break;
      
    case 3: // Practice 4
      if (command == "mode") {
        state.colorMode = (state.colorMode + 1) % 3;
        state.colorStep = 0;
        Serial.print(F("Color mode changed to: "));
        switch (state.colorMode) {
          case 0: Serial.println(F("Rainbow")); break;
          case 1: Serial.println(F("Breathing")); break;
          case 2: Serial.println(F("Strobe")); break;
        }
      } else if (command == "faster") {
        Serial.println(F("Animation speed increased"));
      } else if (command == "slower") {
        Serial.println(F("Animation speed decreased"));
      } else if (command == "status") {
        showPractice4Status();
      } else {
        Serial.println(F("Unknown command. Try: mode, faster, slower, status"));
      }
      break;
      
    case 4: // Practice 5
      if (command == "clear") {
        state.displayNumber = 0;
        Serial.println(F("Display cleared"));
      } else if (command == "zeros") {
        state.showLeadingZeros = !state.showLeadingZeros;
        Serial.print(F("Leading zeros: "));
        Serial.println(state.showLeadingZeros ? F("ON") : F("OFF"));
      } else if (command == "test") {
        testSevenSegmentDisplay();
      } else {
        // Try to parse as number
        int number = command.toInt();
        if (number >= 0 && number <= 9999 || command == "0") {
          state.displayNumber = number;
          Serial.print(F("Displaying: "));
          Serial.println(state.displayNumber);
        } else {
          Serial.println(F("Enter number (0-9999) or command: clear, zeros, test"));
        }
      }
      break;
  }
}

// ============================================
// HELPER FUNCTIONS
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
  Serial.println(F("\n--- Practice 1 Status ---"));
  Serial.print(F("Blink Mode: "));
  Serial.print(state.blinkMode);
  Serial.print(F(" (Interval: "));
  Serial.print(state.blinkInterval);
  Serial.println(F("ms)"));
  Serial.print(F("Red LED: "));
  Serial.println(state.redLedState ? F("ON") : F("OFF"));
  Serial.print(F("Green LED: "));
  Serial.println(state.greenLedState ? F("ON") : F("OFF"));
  Serial.print(F("Button State: "));
  Serial.println(digitalRead(SHARED_D5) ? F("PRESSED") : F("RELEASED"));
  Serial.println();
}

void showPractice2Status() {
  int potValue = analogRead(POTENTIOMETER);
  Serial.println(F("\n--- Practice 2 Status ---"));
  Serial.print(F("Potentiometer: "));
  Serial.print(potValue);
  Serial.print(F(" ("));
  Serial.print((potValue / 1023.0) * 5.0, 2);
  Serial.println(F("V)"));
  Serial.print(F("PWM Value: "));
  Serial.print(state.brightness);
  Serial.print(F(" ("));
  Serial.print((state.brightness / 255.0) * 100, 1);
  Serial.println(F("%)"));
  Serial.println();
}

void showPractice3Status() {
  Serial.println(F("\n--- Practice 3 Status ---"));
  Serial.print(F("LED Brightness: "));
  Serial.print(state.brightness);
  Serial.print(F(" ("));
  Serial.print((state.brightness / 255.0) * 100, 1);
  Serial.println(F("%)"));
  Serial.print(F("Servo Position: "));
  Serial.print(state.servoPosition);
  Serial.print(F("° (Target: "));
  Serial.print(state.targetServoPosition);
  Serial.println(F("°)"));
  Serial.println();
}

void showPractice4Status() {
  Serial.println(F("\n--- Practice 4 Status ---"));
  Serial.print(F("Color Mode: "));
  switch (state.colorMode) {
    case 0: Serial.println(F("Rainbow")); break;
    case 1: Serial.println(F("Breathing")); break;
    case 2: Serial.println(F("Strobe")); break;
  }
  Serial.print(F("RGB Values: R="));
  Serial.print(state.redValue);
  Serial.print(F(" G="));
  Serial.print(state.greenValue);
  Serial.print(F(" B="));
  Serial.println(state.blueValue);
  Serial.print(F("Animation Step: "));
  Serial.println(state.colorStep);
  Serial.println();
}

void testServoRange() {
  Serial.println(F("Testing servo full range..."));
  Serial.println(F("Moving to 0°"));
  for (int pos = state.servoPosition; pos >= 0; pos--) {
    myServo.write(pos);
    delay(15);
  }
  delay(1000);
  
  Serial.println(F("Moving to 180°"));
  for (int pos = 0; pos <= 180; pos++) {
    myServo.write(pos);
    delay(15);
  }
  delay(1000);
  
  Serial.println(F("Returning to center"));
  for (int pos = 180; pos >= 90; pos--) {
    myServo.write(pos);
    delay(15);
  }
  
  state.servoPosition = 90;
  state.targetServoPosition = 90;
  Serial.println(F("Servo test complete"));
}

void testSevenSegmentDisplay() {
  Serial.println(F("Testing 7-segment display..."));
  for (int i = 0; i <= 9; i++) {
    state.displayNumber = i * 1111; // 0000, 1111, 2222, etc.
    Serial.print(F("Showing: "));
    Serial.println(state.displayNumber);
    delay(1000);
  }
  state.displayNumber = 1234;
  Serial.println(F("Test complete - showing 1234"));
}

void showSystemInfo() {
  Serial.println();
  Serial.println(F("═══════════════ SYSTEM INFORMATION ═══════════════"));
  Serial.println(F("Hardware: Arduino Nano (ATmega328P)"));
  Serial.println(F("Clock Speed: 16 MHz"));
  Serial.println(F("Flash Memory: 32 KB"));
  Serial.println(F("SRAM: 2 KB"));
  Serial.println(F("EEPROM: 1 KB"));
  Serial.println();
  Serial.print(F("Uptime: "));
  Serial.print(millis() / 1000);
  Serial.println(F(" seconds"));
  Serial.print(F("Free RAM: "));
  Serial.print(getFreeRAM());
  Serial.println(F(" bytes"));
  Serial.println(F("═══════════════════════════════════════════════════"));
  Serial.println();
  Serial.print(F("Selection: "));
}

int getFreeRAM() {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}
