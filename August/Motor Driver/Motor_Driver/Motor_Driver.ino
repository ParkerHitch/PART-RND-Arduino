// Pin definitions
const int pwmPin = 9;    // PWM pin connected to MD20A PWM input
const int dirPin = 8;    // Direction pin connected to MD20A DIR input
const int potPin = A0;   // Analog input pin for potentiometer
const int buttonPin = 7; // Pin for direction control button

// Variables for direction control
bool motorDirection = LOW;  // Initial motor direction is forward (DIR = LOW)
bool lastButtonState = HIGH; // Track the previous state of the button

void setup() {
  // Set PWM and direction pins as outputs
  pinMode(pwmPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  
  // Set the direction control button as an input with an internal pull-up
  pinMode(buttonPin, INPUT_PULLUP); 

  // Set initial motor direction
  digitalWrite(dirPin, motorDirection);
}

void loop() {
  // Read the potentiometer value (0 to 1023)
  int potValue = analogRead(potPin);

  // Map the potentiometer value to PWM range (0 to 255)
  int motorSpeed = map(potValue, 0, 1023, 0, 255);

  // Check if the button is pressed to change the direction
  bool buttonState = digitalRead(buttonPin);
  if (buttonState == LOW && lastButtonState == HIGH) {
    // Button was pressed, toggle the direction
    motorDirection = !motorDirection;
    digitalWrite(dirPin, motorDirection);  // Update the motor direction
  }
  // Save the current button state for next loop iteration
  lastButtonState = buttonState;

  // Send the PWM signal to control motor speed
  analogWrite(pwmPin, motorSpeed);
  
  // Optional delay to debounce button and smooth PWM signal
  delay(50); 
}
