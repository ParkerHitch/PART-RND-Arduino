#include <SoftwareSerial.h>

// Define two software serial ports for the two RYLR993 modules
SoftwareSerial loraSerial1(4, 5); // RX1, TX1 (Module 1)
const int interruptPin = 12;      // Used for TX and RX with the pixhawk
bool randomizeCancellation = false;
String oldChildAddress = "2";
int counter = 5;
void setup()
{

    Serial.begin(9600); // For debugging

    // Start communication with both RYLR993 modules
    loraSerial1.begin(9600); // Module 1 (Transmitter)

    // Set address for both modules
    loraSerial1.println("AT+RESET"); // Set Module 1 address

    loraSerial1.println("AT+ADDRESS=1"); // Set Module 1 address
    if (loraSerial1.available())
    {
        Serial.println("Module 1 response:" + loraSerial1.readString());
    }
    Serial.println("Setup complete.");
    randomSeed(0);
}


void loop()
{

    int randomAddress = random(2, 1000);
    String newChildAddress = String(randomAddress);

    // Send the random address from Master (Module 1) to Slave (Module 2)
    if (counter == 0)
    {
        counter = 5;
        String messageToSend = newChildAddress;
        String messageLength = String(messageToSend.length());
        loraSerial1.println("AT+SEND=" + oldChildAddress + "," + messageLength + "," + messageToSend);
        Serial.println("Sent random address:" + oldChildAddress + " to " + newChildAddress);
        //oldChildAddress = newChildAddress;
    }
    else
    {
        Serial.println("Sending message");
        loraSerial1.println("AT+SEND=" + oldChildAddress + ",5,Hello");
    }

    counter--;

    delay(2000);
}