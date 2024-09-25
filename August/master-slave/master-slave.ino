#include <SoftwareSerial.h>

// Define two software serial ports for the two RYLR993 modules
SoftwareSerial loraSerial1(6, 7); // RX1, TX1 (Module 1)
SoftwareSerial loraSerial2(4, 5); // RX2, TX2 (Module 2)
const int interruptPin = 12;      // Used for TX and RX with the pixhawk
bool randomizeCancellation = false;
String oldChildAddress = "2";
int counter = 5;
void setup()
{

    Serial.begin(9600); // For debugging

    // Start communication with both RYLR993 modules
    loraSerial1.begin(9600); // Module 1 (Transmitter)
    loraSerial2.begin(9600); // Module 2 (Receiver)

    // Set address for both modules
    loraSerial1.println("AT+RESET"); // Set Module 1 address
    loraSerial2.println("AT+RESET"); // Set Module 1 address

    loraSerial1.println("AT+ADDRESS=1"); // Set Module 1 address
    delay(1000);
    if (loraSerial1.available())
    {
        Serial.println("Module 1 response:" + loraSerial1.readString());
    }
    delay(1000);
    loraSerial2.println("AT+ADDRESS="+oldChildAddress); // Set Module 2 address as plane reg to init
    delay(1000);
    if (loraSerial2.available())
    {
        Serial.println("Module 2 response:" + loraSerial2.readString());
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
        oldChildAddress = newChildAddress;
    }
    else {
        loraSerial1.println("AT+SEND=" + oldChildAddress+ ",5,Hello");
    }


    // Listen for the message on Module 2 (Receiver)
    if (loraSerial2.available())
    {
        String receivedMessage = loraSerial2.readStringUntil('\n');
        Serial.println("Message received on Address="+oldChildAddress);
        Serial.println(receivedMessage);

    }
    counter--;
    delay(2000);
}
