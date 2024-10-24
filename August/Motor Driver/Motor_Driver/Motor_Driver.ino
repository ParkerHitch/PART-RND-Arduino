#define DIR_PIN 8      // Direction pin for MD20A
#define PWM_PIN 9      // PWM pin for MD20A
#define ENC_A_PIN 2    // Encoder channel A pin (interrupt)
#define ENC_B_PIN 3    // Encoder channel B pin

#define CIRCUMFERENCE 10.0 // Circumference of the spool in cm (adjust based on spool size)
#define PPR 16             // Pulses per revolution (adjust based on your encoder)
#define GEAR_RATIO 1.0     // Gear ratio of the motor (adjust if necessary)

// Desired height and motor speed
const float height = 100.0;   // Target height in cm
const int pwmSpeed = 5;      // Initial PWM value (can be controlled with PID later)
const int direction = 0;       // 0 = Forward, 1 = Reverse

volatile long encoderTicks = 0;   // Counts encoder ticks
long targetTicks = 270;             // Target encoder ticks to reach desired height

void setup() {
  pinMode(DIR_PIN, OUTPUT);   // Set DIR pin as output
  pinMode(PWM_PIN, OUTPUT);   // Set PWM pin as output
  
  pinMode(ENC_A_PIN, INPUT_PULLUP);  // Encoder channel A
  pinMode(ENC_B_PIN, INPUT_PULLUP);  // Encoder channel B

  // Attach interrupts for the encoder pins
  attachInterrupt(digitalPinToInterrupt(ENC_A_PIN), a_changed, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENC_B_PIN), b_changed, CHANGE);

  Serial.begin(9600);

  // Calculate the number of ticks required to reach the desired height
  float rotationsNeeded = height / CIRCUMFERENCE;
  targetTicks = rotationsNeeded * PPR * GEAR_RATIO;  // Calculate total target ticks

  Serial.print("Target ticks: ");
  Serial.println(targetTicks);

  //Start the motor
  startMotor();
}

void loop() {
  // Simple bang-bang control to stop the motor when the target is reached
  if (abs(encoderTicks) >= targetTicks) {
    stopMotor();
  }
  // analogWrite(PWM_PIN, 0);    // Set motor speed using PWM~
  // // Serial.println("Loc ");
  // Serial.println(encoderTicks);
  // Serial.println("\n");
}

void a_changed(){
  int a = digitalRead(ENC_A_PIN);
  int b = digitalRead(ENC_B_PIN);

  if (a == b) {
    encoderTicks++;  // Forward direction
  } else {
    encoderTicks--;  // Reverse direction
  }

}

void b_changed(){
  int a = digitalRead(ENC_A_PIN);
  int b = digitalRead(ENC_B_PIN);
  
  if (b != a) {
    encoderTicks++;  // Forward direction
  } else {
    encoderTicks--;  // Reverse direction
  }
}

// Encoder tick handler (counts the ticks for both channels A & B)
void encoderTick() {
  int a = digitalRead(ENC_A_PIN);
  int b = digitalRead(ENC_B_PIN);

  // Determine the direction of rotation by comparing channel A and B
  if (a == b) {
    encoderTicks++;  // Forward direction
  } else {
    encoderTicks--;  // Reverse direction
  }
}

// Function to start the motor
void startMotor() {
  digitalWrite(DIR_PIN, direction);  // Set motor direction
  analogWrite(PWM_PIN, pwmSpeed);    // Set motor speed using PWM

  Serial.println("Motor started.");
}

// Function to stop the motor
void stopMotor() {
  analogWrite(PWM_PIN, 0);  // Stop the motor by setting PWM to 0
  Serial.println("Motor stopped.");

  // Disable interrupts to stop counting encoder ticks
  detachInterrupt(digitalPinToInterrupt(ENC_A_PIN));
  detachInterrupt(digitalPinToInterrupt(ENC_B_PIN));

  while (true);  // Stop the loop indefinitely
}
