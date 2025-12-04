/*
  LED Blink Practice
  Turns an LED on for one second, then off for one second, repeatedly.
  
  Circuit:
  - LED connected from digital pin 13 to ground through 220Ω resistor
*/

// Define the LED pin
int ledPin = 13;  // LED connected to digital pin 13

void setup() {
  // Initialize the digital pin as an output
  pinMode(ledPin, OUTPUT);
  
  // Optional: Initialize serial communication for debugging
  Serial.begin(9600);
  Serial.println("LED Blink Program Started");
}

void loop() {
  // Turn the LED on (HIGH is the voltage level)
  digitalWrite(ledPin, HIGH);
  Serial.println("LED ON");
  
  // Wait for 1000 milliseconds (1 second)
  delay(1000);
  
  // Turn the LED off by making the voltage LOW
  digitalWrite(ledPin, LOW);
  Serial.println("LED OFF");
  
  // Wait for 1000 milliseconds (1 second)
  delay(1000);
}
