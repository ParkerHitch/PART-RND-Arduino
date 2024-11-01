#define DIR_PIN 4      // Direction pin for MD20A
#define PWM_PIN 5      // PWM pin for MD20A
#define ENC_A_PIN 2    // Encoder channel A pin (interrupt)
#define ENC_B_PIN 3    // Encoder channel B pin

#define CIRCUMFERENCE 18.0 // Circumference of the spool in cm (adjust based on spool size)
#define PPR 64             // Pulses per revolution (adjust based on your encoder)
#define GEAR_RATIO 1.0     // Gear ratio of the motor (adjust if necessary)

// #define GAIN_P (10/64.0)
#define GAIN_P  0
// #define GAIN_I  0.0001
#define GAIN_I  0
#define GAIN_FF 0

// Desired height and motor speed
const int pwmSpeed = 3;      // Initial PWM value (can be controlled with PID later)
int direction = 0;           // 0 = Forward, 1 = Reverse
float integralAccumulator = 0;

volatile long encoderTicks = 0;   // Counts encoder ticks
long targetTicks = 0;           // Target encoder ticks to reach desired height

void setup() {
  pinMode(DIR_PIN, OUTPUT);   // Set DIR pin as output
  pinMode(PWM_PIN, OUTPUT);   // Set PWM pin as output
  
  pinMode(ENC_A_PIN, INPUT_PULLUP);  // Encoder channel A
  pinMode(ENC_B_PIN, INPUT_PULLUP);  // Encoder channel B

  // Attach interrupts for the encoder pins
  attachInterrupt(digitalPinToInterrupt(ENC_A_PIN), a_changed, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENC_B_PIN), b_changed, CHANGE);

  Serial.begin(9600);

  Serial.println("System Initialized.");

  zeroDisplacement();

  Serial.println("Enter disp: ");
  setTargetDisplacement(0);
}

void zeroDisplacement() {
  encoderTicks = 0;  // Resets encoder ticks to 0
  Serial.println("Encoder reset to zero.");
}

void setTargetDisplacement(float dispFt) {
  // Convert displacement from feet to cm, then calculate target ticks
  float dispCm = dispFt * 30.48;  // Convert feet to cm
  float rotationsNeeded = dispCm / CIRCUMFERENCE;
  targetTicks = abs(rotationsNeeded * PPR * GEAR_RATIO);

  Serial.print("Target displacement set to ");
  Serial.print(dispFt);
  Serial.print(" ft (");
  Serial.print(targetTicks);
  Serial.println(" ticks).");

  // startMotor();  // Start the motor to drive towards the target displacement
}

void loop() {
  Serial.print("Target: ");
  Serial.println(targetTicks);
  Serial.print("Actual: ");
  Serial.println(encoderTicks);
  int tickError = targetTicks - encoderTicks;

  integralAccumulator += tickError;

  float pCorrected = tickError * GAIN_P;

  pCorrected += integralAccumulator * GAIN_I;

  if (pCorrected > pwmSpeed) {
    pCorrected = pwmSpeed;
  } else if (pCorrected < -pwmSpeed){
    pCorrected = -pwmSpeed;
  }

  if (Serial.available()) {
    setTargetDisplacement(Serial.parseFloat());
    Serial.read();
  }

  Serial.println(GAIN_FF + pCorrected);
  setPowerSigned(GAIN_FF + pCorrected);
}

void setPowerSigned(int power) {
  digitalWrite(DIR_PIN, power < 0 ? 1 : 0);
  analogWrite(PWM_PIN, abs(power));
}

void a_changed() {
  int a = digitalRead(ENC_A_PIN);
  int b = digitalRead(ENC_B_PIN);

  if (a == b) {
    encoderTicks++;  // Forward direction
  } else {
    encoderTicks--;  // Reverse direction
  }
}

void b_changed() {
  int a = digitalRead(ENC_A_PIN);
  int b = digitalRead(ENC_B_PIN);
  
  if (b != a) {
    encoderTicks++;  // Forward direction
  } else {
    encoderTicks--;  // Reverse direction
  }
}

void startMotor() {
  digitalWrite(DIR_PIN, direction);  // Set motor direction
  analogWrite(PWM_PIN, pwmSpeed);    // Set motor speed using PWM

  Serial.println("Motor started.");
}

void stopMotor() {
  analogWrite(PWM_PIN, 0);  // Stop the motor by setting PWM to 0
  Serial.println("Motor stopped.");

  // Disable interrupts to stop counting encoder ticks
  detachInterrupt(digitalPinToInterrupt(ENC_A_PIN));
  detachInterrupt(digitalPinToInterrupt(ENC_B_PIN));
}
