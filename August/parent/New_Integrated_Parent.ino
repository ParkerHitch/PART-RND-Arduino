#include <SoftwareSerial.h>
#include <Servo.h>

// --------------------------
// Flight Controller (Pixhawk) Communication
// --------------------------
#define FC_RX 2  // Flight Controller Serial RX Pin
#define FC_TX 3  // Flight Controller Serial TX Pin
SoftwareSerial flightControllerSerial(FC_RX, FC_TX); 

// --------------------------
// LoRa Module Communication (Parent ↔ Child)
// --------------------------
#define LORA_RX 4  // LoRa Serial RX Pin
#define LORA_TX 5  // LoRa Serial TX Pin
SoftwareSerial loraSerial(LORA_RX, LORA_TX);

// --------------------------
// Motor Driver Setup (Winch System)
// --------------------------
#define PWM_PIN 9       // PWM pin for ESC (Winch)
#define ENC_A_PIN 7     // Encoder Channel A (interrupt)
#define ENC_B_PIN 8     // Encoder Channel B

// --------------------------
// Winch Control PWM Values
// --------------------------
// Adjust these values based on your motor/ESC calibration.
// Typically, 1500 µs is neutral. Values greater than 1500 might make the winch raise,
// while values lower than 1500 will lower the winch.
const int PWM_IDLE  = 1500;
const int PWM_RAISE = 1400;   // Adjust for your “raise” direction speed
const int PWM_LOWER = 1600;   // Adjust for your “lower” direction speed

// --------------------------
// Encoder & Closed-Loop (Optional)
// --------------------------
#define CIRCUMFERENCE 18.0  // Circumference of the spool in cm (adjust if needed)
#define PPR 64              // Pulses Per Revolution (encoder specification)
#define GEAR_RATIO 1.0      // Motor gear ratio

// The following gains and variables are for closed-loop control if you wish
// to later implement a position control algorithm.
#define GAIN_P  0    // Proportional gain (set to 0 for now)
#define GAIN_I  0    // Integral gain (set to 0 for now)
#define GAIN_FF 0    // Feedforward term (if needed)

const int pwmSpeed = 50;  // (Not used directly for fixed commands)

// Encoder variables
volatile long encoderTicks = 0;
long targetTicks = 0;
float integralAccumulator = 0;

// --------------------------
// LoRa Addressing
// --------------------------
String oldChildAddress = "2";

// --------------------------
// Create servo instance for ESC control
// --------------------------
Servo myservo;

void setup() {
    // Debug Serial Monitor for setup and troubleshooting
    Serial.begin(9600);
    
    // Initialize flight controller serial
    flightControllerSerial.begin(9600);
    
    // Initialize LoRa serial and module (uncomment the begin if needed)
    loraSerial.begin(9600);  
    pinMode(LORA_RX, INPUT);
    pinMode(LORA_TX, OUTPUT);
    loraSerial.println("AT+RESET");
    loraSerial.println("AT+ADDRESS=1");
    delay(100);  // Small delay for LoRa to process commands
    if (loraSerial.available()) {
        Serial.println("LoRa Module response: " + loraSerial.readString());
    }

    // Motor Driver Setup
    pinMode(PWM_PIN, OUTPUT);
    myservo.attach(PWM_PIN);

    // Encoder Setup
    pinMode(ENC_A_PIN, INPUT_PULLUP);
    pinMode(ENC_B_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(ENC_A_PIN), a_changed, CHANGE);
    attachInterrupt(digitalPinToInterrupt(ENC_B_PIN), b_changed, CHANGE);

    setMotor(PWM_IDLE);

    Serial.println("System Initialized.");
}

void loop() {
    // 1. Handle commands from Flight Controller (Pixhawk)
    Serial.print("Encoder: ");
    Serial.println(encoderTicks);
    if (Serial.available() > 0) {
        char cmd = Serial.read();
        cmd = toupper(cmd);  // Normalize to uppercase
        switch(cmd) {
            case 'L':
                Serial.println("Lowering Winch...");
                setMotor(PWM_LOWER);
                sendLoRaMessage("Winch Lowering");
                break;
            case 'R':
                Serial.println("Raising Winch...");
                setMotor(PWM_RAISE);
                sendLoRaMessage("Winch Raising");
                break;
            case 'I':
                Serial.println("Winch Idle (Stop)...");
                stopMotor();
                sendLoRaMessage("Winch Idle");
                break;
            case '1':
                Serial.println("Sending 1");
                setMotor(2000);
                break;
            case '0':
                Serial.println("Sending 0");
                setMotor(1000);
                break;
            default:
                Serial.print("Unknown command received: ");
                Serial.println(cmd);
                break;
        }
    }
    
    // 2. (Optional) You can add code here to process LoRa incoming messages if needed.
    /*
    if (loraSerial.available()) {
        String incomingMessage = loraSerial.readStringUntil('\n');
        Serial.print("LoRa Received: ");
        Serial.println(incomingMessage);
    }
    */

    // (Optional) For debugging: print encoder ticks periodically
    // Serial.print("Encoder Ticks: ");
    // Serial.println(encoderTicks);
    
    delay(100);  // Delay to avoid overwhelming serial buffer
}


// ============ Motor Control Functions ============

// This function directly sends a PWM signal to the ESC.
// It uses the Servo library to write microsecond values.
// The ESC expects values typically between 1000 and 2000 µs.
void setMotor(int power) {
    int pwmValue = constrain(power, 1000, 2000);
    myservo.writeMicroseconds(pwmValue);
}

// This stops the motor by sending the neutral PWM signal.
// You can also disable the encoder interrupts here if you wish.
void stopMotor() {
    myservo.writeMicroseconds(PWM_IDLE);  // Neutral signal
    Serial.println("Motor stopped.");
    
    // Optionally, disable encoder counting until next movement:
    // detachInterrupt(digitalPinToInterrupt(ENC_A_PIN));
    // detachInterrupt(digitalPinToInterrupt(ENC_B_PIN));
    
    // (Optional) Reattach interrupts after a short period if needed.
    // attachInterrupt(digitalPinToInterrupt(ENC_A_PIN), a_changed, CHANGE);
    // attachInterrupt(digitalPinToInterrupt(ENC_B_PIN), b_changed, CHANGE);
}


// ============ (Optional) Closed-Loop Control Function ============
// This function is based on the encoder ticks feedback and is intended
// for future improvements if you want to drive the motor to a target displacement.
// Currently, the gains are set to zero, so this simply returns a neutral signal.
float get_power() {
    int tickError = targetTicks - encoderTicks;
    integralAccumulator += tickError;
    
    float pCorrected = tickError * GAIN_P;
    pCorrected += integralAccumulator * GAIN_I;
    
    if (pCorrected > pwmSpeed) {
        pCorrected = pwmSpeed;
    } else if (pCorrected < -pwmSpeed) {
        pCorrected = -pwmSpeed;
    }
    
    float power = (pCorrected + pwmSpeed) * (1000.0 / (pwmSpeed * 2)) + 1000;
    return power;
}


// ============ Encoder Functions ============
void a_changed() {
    int a = digitalRead(ENC_A_PIN);
    int b = digitalRead(ENC_B_PIN);
    encoderTicks += (a == b) ? 1 : -1;
}

void b_changed() {
    int a = digitalRead(ENC_A_PIN);
    int b = digitalRead(ENC_B_PIN);
    encoderTicks += (b != a) ? 1 : -1;
}

// ============ LoRa Communication (Parent → Child) ============
void sendLoRaMessage(String message) {
    String messageSize = String(message.length());
    loraSerial.println("AT+SEND=" + oldChildAddress + "," + messageSize + "," + message);
    Serial.println("LoRa Sent: " + message);
}
