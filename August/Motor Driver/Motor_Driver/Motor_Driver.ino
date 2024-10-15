#define DIR_PIN 8    // Direction pin for MD20A
#define PWM_PIN 9    // PWM pin for MD20A

const float height = 1000.0;  // Desired height in cm
const float speed = 50.0;    // Motor speed in cm/s (depends on your setup)
const int pwmSpeed = 128;     // PWM value (0-255) to control motor speed
const int direction = 0;      // 0 = Forward, 1 = Reverse

unsigned long runTime;  // Motor run time in milliseconds
unsigned long startTime;  // Start time of the motor run

void setup() {
  pinMode(DIR_PIN, OUTPUT);  // Set DIR pin as output
  pinMode(PWM_PIN, OUTPUT);  // Set PWM pin as output

  // Calculate the run time required to reach the given height
  runTime = (height / speed) * 1000;  // Convert seconds to milliseconds

  Serial.begin(9600);  // Start serial communication
  Serial.print("Motor will run for ");
  Serial.print(runTime / 1000.0);
  Serial.println(" seconds to reach the target height.");
  
  // Start the motor
  startMotor();
}

void loop() {
  // Stop the motor if the calculated time has passed
  if (millis() - startTime >= runTime) {
    stopMotor();
  }
}

// Function to start the motor
void startMotor() {
  digitalWrite(DIR_PIN, direction);  // Set motor direction
  analogWrite(PWM_PIN, pwmSpeed);    // Set motor speed using PWM
  startTime = millis();  // Record the start time
  
  Serial.println("Motor started.");
}

// Function to stop the motor
void stopMotor() {
  analogWrite(PWM_PIN, 0);  // Stop the motor by setting PWM to 0
  Serial.println("Motor stopped.");
  
  while (true);  // Stop the loop indefinitely
}
