// ============================================
// PRACTICE 4: RGB LED COLOR MIXING
// ============================================

// Pin definitions
const int RGB_RED = 9;
const int RGB_GREEN = 10;
const int RGB_BLUE = 11;
const int MODE_BUTTON = 2;
const int STATUS_LED = 13;

// Color variables
int redValue = 0;
int greenValue = 0;
int blueValue = 0;

// Animation variables
unsigned long lastColorUpdate = 0;
const int COLOR_UPDATE_INTERVAL = 20; // 50 FPS
int animationStep = 0;
float animationSpeed = 1.0;

// Mode variables
volatile bool modeChanged = false;
int colorMode = 0;
const int MAX_COLOR_MODES = 6;

// Timing variables
unsigned long lastModeChange = 0;
unsigned long lastStatusReport = 0;

// Color mode names
const char* modeNames[] = {
  "Rainbow Cycle",
  "Breathing White",
  "Color Strobe",
  "Smooth Transitions",
  "Random Colors",
  "Manual Control"
};

void setup() {
  Serial.begin(9600);
  
  // Configure pins
  pinMode(RGB_RED, OUTPUT);
  pinMode(RGB_GREEN, OUTPUT);
  pinMode(RGB_BLUE, OUTPUT);
  pinMode(STATUS_LED, OUTPUT);
  pinMode(MODE_BUTTON, INPUT);
  
  // Setup interrupt
  attachInterrupt(digitalPinToInterrupt(MODE_BUTTON), changeColorMode, RISING);
  
  Serial.println("=== PRACTICE 4: RGB LED COLOR MIXING ===");
  Serial.println("Mode button cycles through color patterns");
  Serial.println("Serial commands:");
  Serial.println("  's' - Show status");
  Serial.println("  '+' - Increase speed");
  Serial.println("  '-' - Decrease speed");
  Serial.println("  'r/g/b' + number - Set manual color (0-255)");
  Serial.println();
  
  printCurrentMode();
  digitalWrite(STATUS_LED, HIGH);
}

void loop() {
  unsigned long currentTime = millis();
  
  // Handle mode changes
  if (modeChanged) {
    handleModeChange();
    modeChanged = false;
  }
  
  // Handle serial commands
  handleSerialCommands();
  
  // Update colors based on current mode
  if (currentTime - lastColorUpdate >= COLOR_UPDATE_INTERVAL) {
    updateColors();
    lastColorUpdate = currentTime;
    animationStep++;
  }
  
  // Apply colors to RGB LED
  setRGBColor(redValue, greenValue, blueValue);
  
  // Status reporting
  if (currentTime - lastStatusReport >= 5000) { // Every 5 seconds
    printColorStatus();
    lastStatusReport = currentTime;
  }
  
  // Status LED heartbeat
  digitalWrite(STATUS_LED, (currentTime / 1000) % 2);
}

void changeColorMode() {
  unsigned long currentTime = millis();
  
  // Debouncing
  if (currentTime - lastModeChange > 300) {
    modeChanged = true;
    lastModeChange = currentTime;
  }
}

void handleModeChange() {
  colorMode = (colorMode + 1) % MAX_COLOR_MODES;
  animationStep = 0; // Reset animation
  
  Serial.print("Mode changed to: ");
  Serial.print(colorMode);
  Serial.print(" - ");
  Serial.println(modeNames[colorMode]);
  
  printCurrentMode();
}

void updateColors() {
  switch (colorMode) {
    case 0: // Rainbow Cycle
      rainbowCycle();
      break;
    case 1: // Breathing White
      breathingWhite();
      break;
    case 2: // Color Strobe
      colorStrobe();
      break;
    case 3: // Smooth Transitions
      smoothTransitions();
      break;
    case 4: // Random Colors
      randomColors();
      break;
    case 5: // Manual Control
      // Colors set via serial commands
      break;
  }
}

void rainbowCycle() {
  float phase = (animationStep * animationSpeed) * 0.01;
  int hue = (int)(phase * 360) % 360;
  
  // Convert HSV to RGB
  hsvToRgb(hue, 255, 255, &redValue, &greenValue, &blueValue);
}

void breathingWhite() {
  float breath = sin((animationStep * animationSpeed) * 0.05);
  int brightness = (int)((breath + 1.0) * 127.5); // 0-255 range
  
  redValue = brightness;
  greenValue = brightness;
  blueValue = brightness;
}

void colorStrobe() {
  int strobePhase = ((int)(animationStep * animationSpeed) / 20) % 8;
  
  // Reset all colors
  redValue = greenValue = blueValue = 0;
  
  switch (strobePhase) {
    case 0: redValue = 255; break;                    // Red
    case 1: greenValue = 255; break;                  // Green
    case 2: blueValue = 255; break;                   // Blue
    case 3: redValue = greenValue = 255; break;       // Yellow
    case 4: redValue = blueValue = 255; break;        // Magenta
    case 5: greenValue = blueValue = 255; break;      // Cyan
    case 6: redValue = greenValue = blueValue = 255; break; // White
    case 7: break; // Off
  }
}

void smoothTransitions() {
  // Smooth transitions between primary colors
  int cycle = ((int)(animationStep * animationSpeed) / 100) % 6;
  int progress = ((int)(animationStep * animationSpeed) % 100) * 255 / 100;
  
  switch (cycle) {
    case 0: // Red to Yellow
      redValue = 255;
      greenValue = progress;
      blueValue = 0;
      break;
    case 1: // Yellow to Green
      redValue = 255 - progress;
      greenValue = 255;
      blueValue = 0;
      break;
    case 2: // Green to Cyan
      redValue = 0;
      greenValue = 255;
      blueValue = progress;
      break;
    case 3: // Cyan to Blue
      redValue = 0;
      greenValue = 255 - progress;
      blueValue = 255;
      break;
    case 4: // Blue to Magenta
      redValue = progress;
      greenValue = 0;
      blueValue = 255;
      break;
    case 5: // Magenta to Red
      redValue = 255;
      greenValue = 0;
      blueValue = 255 - progress;
      break;
  }
}

void randomColors() {
  // Change to random color every 2 seconds
  if (animationStep % (int)(100 / animationSpeed) == 0) {
    redValue = random(0, 256);
    greenValue = random(0, 256);
    blueValue = random(0, 256);
    
    Serial.print("Random color: R=");
    Serial.print(redValue);
    Serial.print(" G=");
    Serial.print(greenValue);
    Serial.print(" B=");
    Serial.println(blueValue);
  }
}

void setRGBColor(int red, int green, int blue) {
  // Constrain values to valid range
  red = constrain(red, 0, 255);
  green = constrain(green, 0, 255);
  blue = constrain(blue, 0, 255);
  
  // Apply to RGB LED (assuming common cathode)
  analogWrite(RGB_RED, red);
  analogWrite(RGB_GREEN, green);
  analogWrite(RGB_BLUE, blue);
}

void hsvToRgb(int h, int s, int v, int* r, int* g, int* b) {
  // Convert HSV to RGB
  float hf = h / 60.0;
  float sf = s / 255.0;
  float vf = v / 255.0;
  
  int i = (int)hf;
  float f = hf - i;
  float p = vf * (1.0 - sf);
  float q = vf * (1.0 - sf * f);
  float t = vf * (1.0 - sf * (1.0 - f));
  
  switch (i) {
    case 0: *r = vf * 255; *g = t * 255; *b = p * 255; break;
    case 1: *r = q * 255; *g = vf * 255; *b = p * 255; break;
    case 2: *r = p * 255; *g = vf * 255; *b = t * 255; break;
    case 3: *r = p * 255; *g = q * 255; *b = vf * 255; break;
    case 4: *r = t * 255; *g = p * 255; *b = vf * 255; break;
    default: *r = vf * 255; *g = p * 255; *b = q * 255; break;
  }
}

void handleSerialCommands() {
  if (Serial.available()) {
    String command = Serial.readString();
    command.trim();
    
    if (command == "s" || command == "S") {
      printDetailedStatus();
    } else if (command == "+") {
      animationSpeed = min(animationSpeed * 1.5, 10.0);
      Serial.print("Speed increased to: ");
      Serial.println(animationSpeed);
    } else if (command == "-") {
      animationSpeed = max(animationSpeed / 1.5, 0.1);
      Serial.print("Speed decreased to: ");
      Serial.println(animationSpeed);
    } else if (command.startsWith("r")) {
      if (colorMode == 5) { // Manual mode
        int value = command.substring(1).toInt();
        redValue = constrain(value, 0, 255);
        Serial.print("Red set to: ");
        Serial.println(redValue);
      }
    } else if (command.startsWith("g")) {
      if (colorMode == 5) { // Manual mode
        int value = command.substring(1).toInt();
        greenValue = constrain(value, 0, 255);
        Serial.print("Green set to: ");
        Serial.println(greenValue);
      }
    } else if (command.startsWith("b")) {
      if (colorMode == 5) { // Manual mode
        int value = command.substring(1).toInt();
        blueValue = constrain(value, 0, 255);
        Serial.print("Blue set to: ");
        Serial.println(blueValue);
      }
    }
  }
}

void printCurrentMode() {
  Serial.print("Current mode: ");
  Serial.print(colorMode);
  Serial.print(" - ");
  Serial.println(modeNames[colorMode]);
  
  if (colorMode == 5) {
    Serial.println("Manual mode: Use 'r###', 'g###', 'b###' to set colors");
  }
}

void printColorStatus() {
  Serial.print("RGB: (");
  Serial.print(redValue);
  Serial.print(", ");
  Serial.print(greenValue);
  Serial.print(", ");
  Serial.print(blueValue);
  Serial.print(") | Mode: ");
  Serial.print(modeNames[colorMode]);
  Serial.print(" | Speed: ");
  Serial.println(animationSpeed);
}

void printDetailedStatus() {
  Serial.println("\n=== RGB LED STATUS ===");
  Serial.print("Current Mode: ");
  Serial.print(colorMode);
  Serial.print(" - ");
  Serial.println(modeNames[colorMode]);
  
  Serial.print("RGB Values: R=");
  Serial.print(redValue);
  Serial.print(" G=");
  Serial.print(greenValue);
  Serial.print(" B=");
  Serial.println(blueValue);
  
  Serial.print("Animation Speed: ");
  Serial.println(animationSpeed);
  
  Serial.print("Animation Step: ");
  Serial.println(animationStep);
  
  Serial.print("Uptime: ");
  Serial.print(millis() / 1000);
  Serial.println(" seconds");
  Serial.println();
}
