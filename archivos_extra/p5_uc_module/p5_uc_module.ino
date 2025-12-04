// ============================================
// PRACTICE 5 SIMPLIFIED: 7SEG-MPX4-CC DISPLAY TEST
// ============================================

// Pin definitions for segments
const int segmentPins[] = {3, 4, 5, 6, 7, 8, 9, 10}; // A,B,C,D,E,F,G,DP
const int digitPins[] = {11, 12, A0, A1}; // Digit 1,2,3,4 control pins
const int STATUS_LED = 13;

// 7-segment digit patterns (0-9) - Common Cathode
const byte digitPatterns[] = {
  0b11111100, // 0: A,B,C,D,E,F on
  0b01100000, // 1: B,C on
  0b11011010, // 2: A,B,D,E,G on
  0b11110010, // 3: A,B,C,D,G on
  0b01100110, // 4: B,C,F,G on
  0b10110110, // 5: A,C,D,F,G on
  0b10111110, // 6: A,C,D,E,F,G on
  0b11100000, // 7: A,B,C on
  0b11111110, // 8: A,B,C,D,E,F,G on
  0b11110110  // 9: A,B,C,D,F,G on
};

// Display variables
int displayNumber = 0;        // Number to display (0-9999)
int currentDigit = 0;         // Current digit being multiplexed (0-3)
unsigned long lastDisplayUpdate = 0;
const int DISPLAY_UPDATE_INTERVAL = 2; // 2ms for smooth multiplexing (500Hz)

// User input variables
String inputBuffer = "";
bool newNumberReceived = false;

// Display control
bool displayEnabled = true;
bool showLeadingZeros = false;
bool blinkDecimalPoints = false;
int brightness = 5; // 1-10 scale

void setup() {
  Serial.begin(9600);
  
  // Initialize segment pins as outputs
  for (int i = 0; i < 8; i++) {
    pinMode(segmentPins[i], OUTPUT);
    digitalWrite(segmentPins[i], LOW);
  }
  
  // Initialize digit control pins as outputs
  for (int i = 0; i < 4; i++) {
    pinMode(digitPins[i], OUTPUT);
    digitalWrite(digitPins[i], LOW); // Turn off all digits initially
  }
  
  pinMode(STATUS_LED, OUTPUT);
  digitalWrite(STATUS_LED, HIGH);
  
  Serial.println("=== PRACTICE 5: 7SEG-MPX4-CC DISPLAY TEST ===");
  Serial.println("4-Digit 7-Segment Display Controller");
  Serial.println();
  Serial.println("Commands:");
  Serial.println("  Enter number (0-9999) - Display the number");
  Serial.println("  'c' - Clear display");
  Serial.println("  'z' - Toggle leading zeros");
  Serial.println("  'd' - Toggle decimal points");
  Serial.println("  'b' - Toggle display on/off");
  Serial.println("  't' - Test all digits");
  Serial.println("  's' - Show status");
  Serial.println("  'h' - Show help");
  Serial.println();
  Serial.println("Enter a number to display (0-9999):");
  
  // Initial display test
  displayNumber = 1234;
  Serial.println("Initial display: 1234");
}

void loop() {
  unsigned long currentTime = millis();
  
  // Handle serial input
  handleSerialInput();
  
  // Update display (multiplexing)
  if (currentTime - lastDisplayUpdate >= DISPLAY_UPDATE_INTERVAL) {
    updateDisplay();
    lastDisplayUpdate = currentTime;
  }
  
  // Status LED heartbeat
  digitalWrite(STATUS_LED, (currentTime / 1000) % 2);
}

void handleSerialInput() {
  while (Serial.available()) {
    char inChar = Serial.read();
    
    if (inChar == '\n' || inChar == '\r') {
      // Process complete input
      if (inputBuffer.length() > 0) {
        processCommand(inputBuffer);
        inputBuffer = "";
      }
    } else if (inChar >= ' ') {
      // Add to input buffer
      inputBuffer += inChar;
    }
  }
}

void processCommand(String command) {
  command.trim();
  command.toLowerCase();
  
  if (command.length() == 0) return;
  
  // Check for single character commands
  if (command.length() == 1) {
    char cmd = command.charAt(0);
    
    switch (cmd) {
      case 'c':
        clearDisplay();
        break;
      case 'z':
        toggleLeadingZeros();
        break;
      case 'd':
        toggleDecimalPoints();
        break;
      case 'b':
        toggleDisplay();
        break;
      case 't':
        testAllDigits();
        break;
      case 's':
        showStatus();
        break;
      case 'h':
        showHelp();
        break;
      default:
        Serial.println("Unknown command. Type 'h' for help.");
        break;
    }
  } else {
    // Try to parse as number
    int number = command.toInt();
    
    if (number >= 0 && number <= 9999) {
      displayNumber = number;
      Serial.print("Displaying: ");
      Serial.println(displayNumber);
    } else {
      Serial.println("Invalid number. Enter 0-9999 or command.");
    }
  }
}

void updateDisplay() {
  if (!displayEnabled) {
    // Turn off all digits
    for (int i = 0; i < 4; i++) {
      digitalWrite(digitPins[i], LOW);
    }
    return;
  }
  
  // Turn off all digits first
  for (int i = 0; i < 4; i++) {
    digitalWrite(digitPins[i], LOW);
  }
  
  // Extract individual digits
  int thousands = (displayNumber / 1000) % 10;
  int hundreds = (displayNumber / 100) % 10;
  int tens = (displayNumber / 10) % 10;
  int units = displayNumber % 10;
  
  // Determine what digit to display
  int digitValue = 0;
  bool showDigit = true;
  bool showDP = false;
  
  switch (currentDigit) {
    case 0: // Thousands
      digitValue = thousands;
      showDigit = showLeadingZeros || thousands > 0 || displayNumber >= 1000;
      break;
    case 1: // Hundreds
      digitValue = hundreds;
      showDigit = showLeadingZeros || hundreds > 0 || displayNumber >= 100;
      showDP = blinkDecimalPoints && ((millis() / 500) % 2);
      break;
    case 2: // Tens
      digitValue = tens;
      showDigit = showLeadingZeros || tens > 0 || displayNumber >= 10;
      break;
    case 3: // Units
      digitValue = units;
      showDigit = true; // Always show units digit
      showDP = blinkDecimalPoints && ((millis() / 300) % 2);
      break;
  }
  
  if (showDigit) {
    // Set segment pattern
    byte pattern = digitPatterns[digitValue];
    if (showDP) pattern |= 0x01; // Add decimal point
    
    // Output to segments
    for (int i = 0; i < 8; i++) {
      digitalWrite(segmentPins[i], (pattern >> (7-i)) & 1);
    }
    
    // Turn on current digit
    digitalWrite(digitPins[currentDigit], HIGH);
    
    // Brightness control through delay
    delayMicroseconds(brightness * 100);
  }
  
  // Move to next digit
  currentDigit = (currentDigit + 1) % 4;
}

void clearDisplay() {
  displayNumber = 0;
  Serial.println("Display cleared");
}

void toggleLeadingZeros() {
  showLeadingZeros = !showLeadingZeros;
  Serial.print("Leading zeros: ");
  Serial.println(showLeadingZeros ? "ON" : "OFF");
}

void toggleDecimalPoints() {
  blinkDecimalPoints = !blinkDecimalPoints;
  Serial.print("Blinking decimal points: ");
  Serial.println(blinkDecimalPoints ? "ON" : "OFF");
}

void toggleDisplay() {
  displayEnabled = !displayEnabled;
  Serial.print("Display: ");
  Serial.println(displayEnabled ? "ON" : "OFF");
}

void testAllDigits() {
  Serial.println("Testing all digits and segments...");
  
  // Test each digit position
  for (int digit = 0; digit < 4; digit++) {
    Serial.print("Testing digit ");
    Serial.print(digit + 1);
    Serial.println("...");
    
    // Turn off all digits
    for (int i = 0; i < 4; i++) {
      digitalWrite(digitPins[i], LOW);
    }
    
    // Test all numbers on current digit
    for (int num = 0; num <= 9; num++) {
      // Set segments for current number
      byte pattern = digitPatterns[num];
      for (int seg = 0; seg < 8; seg++) {
        digitalWrite(segmentPins[seg], (pattern >> (7-seg)) & 1);
      }
      
      // Turn on current digit
      digitalWrite(digitPins[digit], HIGH);
      
      Serial.print("  Showing: ");
      Serial.println(num);
      delay(500);
    }
    
    // Turn off current digit
    digitalWrite(digitPins[digit], LOW);
  }
  
  // Test all segments at once
  Serial.println("Testing all segments (8888)...");
  for (int i = 0; i < 4; i++) {
    digitalWrite(digitPins[i], LOW);
  }
  
  // Set all segments on
  for (int seg = 0; seg < 7; seg++) { // Skip DP
    digitalWrite(segmentPins[seg], HIGH);
  }
  
  // Turn on all digits briefly
  for (int i = 0; i < 4; i++) {
    digitalWrite(digitPins[i], HIGH);
  }
  delay(2000);
  
  // Turn off all
  for (int i = 0; i < 4; i++) {
    digitalWrite(digitPins[i], LOW);
  }
  for (int seg = 0; seg < 8; seg++) {
    digitalWrite(segmentPins[seg], LOW);
  }
  
  Serial.println("Test complete. Resuming normal operation.");
}

void showStatus() {
  Serial.println("\n=== DISPLAY STATUS ===");
  Serial.print("Current Number: ");
  Serial.println(displayNumber);
  Serial.print("Display Enabled: ");
  Serial.println(displayEnabled ? "Yes" : "No");
  Serial.print("Leading Zeros: ");
  Serial.println(showLeadingZeros ? "Yes" : "No");
  Serial.print("Blinking Decimal Points: ");
  Serial.println(blinkDecimalPoints ? "Yes" : "No");
  Serial.print("Brightness Level: ");
  Serial.print(brightness);
  Serial.println("/10");
  Serial.print("Multiplexing Rate: ");
  Serial.print(1000 / DISPLAY_UPDATE_INTERVAL);
  Serial.println(" Hz");
  Serial.print("Current Digit: ");
  Serial.println(currentDigit);
  Serial.print("Uptime: ");
  Serial.print(millis() / 1000);
  Serial.println(" seconds");
  Serial.println();
}

void showHelp() {
  Serial.println("\n=== HELP ===");
  Serial.println("Available Commands:");
  Serial.println("  0-9999    - Display number");
  Serial.println("  c         - Clear display (show 0)");
  Serial.println("  z         - Toggle leading zeros");
  Serial.println("  d         - Toggle decimal point blinking");
  Serial.println("  b         - Toggle display on/off");
  Serial.println("  t         - Test all digits and segments");
  Serial.println("  s         - Show current status");
  Serial.println("  h         - Show this help");
  Serial.println();
  Serial.println("Examples:");
  Serial.println("  1234      - Shows '1234'");
  Serial.println("  42        - Shows '  42' or '0042' with leading zeros");
  Serial.println("  0         - Shows '   0' or '0000' with leading zeros");
  Serial.println();
}
