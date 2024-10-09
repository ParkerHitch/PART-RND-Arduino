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

    // Start communication with the RYLR993 module
    loraSerial1.begin(9600); // Module 1 (Transmitter)

    // Set the address for the module and ensure successful communication
    loraSerial1.println("AT+RESET"); // Reset Module 1
    loraSerial1.println("AT+ADDRESS=1"); // Set Module 1 address
    
    delay(1000); // Allow time for reset

    if (loraSerial1.available())
    {
        String response = loraSerial1.readString();
        Serial.println("Module 1 response: " + response);
    }
    else
    {
        Serial.println("Error: No response from Module 1");
    }

    // Set a dynamic random seed based on analog pin
    randomSeed(analogRead(0));
    Serial.println("Setup complete. Random seed initialized.");
}

void loop()
{
    // Generate a random address between 2 and 1000
    int randomAddress = random(2, 1000);
    String newChildAddress = String(randomAddress);

    if (counter == 0)
    {
        counter = 5;
        String messageToSend = newChildAddress;
        String messageLength = String(messageToSend.length());

        // Send random address to the child module
        loraSerial1.println("AT+SEND=" + oldChildAddress + "," + messageLength + "," + messageToSend);
        Serial.println("Random Address Change Request: Sending address " + newChildAddress + " to child at address " + oldChildAddress);

        // Error handling in case of communication issues
        delay(1000);
        if (loraSerial1.available())
        {
            String response = loraSerial1.readString();
            Serial.println("Module 1 Response: " + response);

            // Update oldChildAddress with newChildAddress after successful transmission
            oldChildAddress = newChildAddress;
            Serial.println("Child address updated to: " + oldChildAddress);
        }
        else
        {
            Serial.println("Error: No response from child module");
        }
    }
    else
    {
        Serial.println("Sending message to child at address " + oldChildAddress);
        loraSerial1.println("AT+SEND=" + oldChildAddress + ",5,Hello");
    }

    counter--;
    delay(2000);
}
