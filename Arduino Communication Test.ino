#include <SoftwareSerial.h>

// Define two software serial ports for the two RYLR993 modules
SoftwareSerial loraSerial1(6, 7); // RX1, TX1 (Module 1)
SoftwareSerial loraSerial2(4, 5); // RX2, TX2 (Module 2)

void setup() {
  Serial.begin(9600); // For debugging

  // Start communication with both RYLR993 modules
  loraSerial1.begin(9600); // Module 1 (Transmitter)
  loraSerial2.begin(9600); // Module 2 (Receiver)

  // Set address for both modules
  loraSerial1.println("AT+RESET"); // Set Module 1 address
  loraSerial2.println("AT+RESET"); // Set Module 1 address

  loraSerial1.println("AT+ADDRESS=1"); // Set Module 1 address
  delay(1000);
  if (loraSerial1.available()){
    Serial.println("Module 1 response:"+  loraSerial1.readString());
  }
  delay(1000);
  loraSerial2.println("AT+ADDRESS=2"); // Set Module 2 address
  delay(1000);
  if (loraSerial2.available()){
    Serial.println("Module 2 response:"+ loraSerial2.readString());
  }
  

  Serial.println("Setup complete.");
}

void loop() {
  // Send a test message from Module 1 (Transmitter) to Module 2 (Receiver)
  loraSerial1.println("AT+SEND=2,5,Hello");

  // Listen for the message on Module 2 (Receiver)
  if (loraSerial2.available()) {
    String receivedMessage = loraSerial2.readStringUntil('\n');
    Serial.println("Message received on Address=2");
    Serial.println(receivedMessage);
  }

  delay(2000); // Wait 5 seconds before repeating
}
