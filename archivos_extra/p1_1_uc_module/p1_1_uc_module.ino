// ============================================
// PRACTICE 1: DIGITAL GPIO CONTROL
// ============================================

// Pin definitions
const int RED_LED = 3;
const int GREEN_LED = 4;
const int BUTTON_INPUT = 5;
const int MODE_BUTTON = 2;
const int STATUS_LED = 13;

// Variables
bool redLedState = false;
bool greenLedState = false;
bool lastButtonState = false;
bool lastModeButtonState = false;
volatile bool modeChanged = false;
int blinkMode = 0; // 0=normal blink, 1=fast blink, 2=button controlled

unsigned long lastBlinkTime = 0;
unsigned long blinkInterval = 500; // milliseconds

void setup() {
  Serial.begin(9600);
  
  // Configure pins
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(STATUS_LED, OUTPUT);
  pinMode(BUTTON_INPUT, INPUT);
  pinMode(MODE_BUTTON, INPUT);
  
  // Setup interrupt for mode button - FIXED FUNCTION NAME
  attachInterrupt(digitalPinToInterrupt(MODE_BUTTON), changeBlinkMode, RISING);
  
  Serial.println("=== PRACTICE 1: DIGITAL GPIO CONTROL ===");
  Serial.println("Red LED: Automatic blinking");
  Serial.println("Green LED: Button controlled");
  Serial.println("Mode Button: Changes blink speed");
  Serial.println("Button Input: Controls green LED");
  Serial.println();
  
  // Initial state
  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(STATUS_LED, HIGH); // Show system is ready
}

void loop() {
  unsigned long currentTime = millis();
  
  // Handle mode changes
  if (modeChanged) {
    handleModeChange();
    modeChanged = false;
  }
  
  // Handle red LED blinking based on current mode
  handleRedLedBlinking(currentTime);
  
  // Handle button input for green LED
  handleButtonInput();
  
  // Status reporting
  static unsigned long lastReport = 0;
  if (currentTime - lastReport >= 2000) { // Report every 2 seconds
    printStatus();
    lastReport = currentTime;
  }
  
  delay(10); // Small delay for stability
}

// FIXED: Corrected function name (was "changeBlink Mode" with space)
void changeBlinkMode() {
  static unsigned long lastInterrupt = 0;
  unsigned long currentTime = millis();
  
  // Simple debouncing
  if (currentTime - lastInterrupt > 200) {
    modeChanged = true;
    lastInterrupt = currentTime;
  }
}

void handleModeChange() {
  blinkMode = (blinkMode + 1) % 3;
  
  switch (blinkMode) {
    case 0:
      blinkInterval = 500;  // Normal blink
      Serial.println("Mode: Normal blink (500ms)");
      break;
    case 1:
      blinkInterval = 100;  // Fast blink
      Serial.println("Mode: Fast blink (100ms)");
      break;
    case 2:
      blinkInterval = 1000; // Slow blink
      Serial.println("Mode: Slow blink (1000ms)");
      break;
  }
}

void handleRedLedBlinking(unsigned long currentTime) {
  if (currentTime - lastBlinkTime >= blinkInterval) {
    redLedState = !redLedState;
    digitalWrite(RED_LED, redLedState);
    lastBlinkTime = currentTime;
    
    Serial.print("Red LED: ");
    Serial.println(redLedState ? "ON" : "OFF");
  }
}

void handleButtonInput() {
  bool currentButtonState = digitalRead(BUTTON_INPUT);
  
  // Detect button press (rising edge)
  if (currentButtonState && !lastButtonState) {
    greenLedState = !greenLedState;
    digitalWrite(GREEN_LED, greenLedState);
    
    Serial.print("Button pressed! Green LED: ");
    Serial.println(greenLedState ? "ON" : "OFF");
  }
  
  lastButtonState = currentButtonState;
}

void printStatus() {
  Serial.println("--- STATUS REPORT ---");
  Serial.print("Blink Mode: ");
  Serial.print(blinkMode);
  Serial.print(" (Interval: ");
  Serial.print(blinkInterval);
  Serial.println("ms)");
  
  Serial.print("Red LED: ");
  Serial.println(redLedState ? "ON" : "OFF");
  
  Serial.print("Green LED: ");
  Serial.println(greenLedState ? "ON" : "OFF");
  
  Serial.print("Button State: ");
  Serial.println(digitalRead(BUTTON_INPUT) ? "PRESSED" : "RELEASED");
  
  Serial.print("Uptime: ");
  Serial.print(millis() / 1000);
  Serial.println(" seconds");
  Serial.println();
}
