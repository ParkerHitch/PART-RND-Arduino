#include <SoftwareSerial.h>
SoftwareSerial loraSerial1(4, 5); // RX1, TX1 (Module 1)
const int interruptPin = 12;      // Used for TX and RX with the pixhawk
bool randomizeCancellation = false;
String oldChildAddress = "2";
int counter = 1000;
int DROPCounter = 1;
#include "functions.h"

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
/**
 * @brief
 *
 * @return true
 * @return false
 */
// bool waitForAck()
// {
//     long timeout = millis() + 5000;

//     while (millis() < timeout)
//     {
//         if (loraSerial1.available())
//         {
//             String receivedMessage = loraSerial1.readStringUntil('\n');
//             receivedMessage.trim();
//             // Serial.println(receivedMessage);
//             if (receivedMessage.startsWith("+RCV"))
//             {
//                 int firstComma = receivedMessage.indexOf(',');
//                 int secondComma = receivedMessage.indexOf(',', firstComma + 1);
//                 int thirdComma = receivedMessage.indexOf(',', secondComma + 1);
//                 String lengthOfMesssage = receivedMessage.substring(firstComma + 1, secondComma);
//                 String message = receivedMessage.substring(secondComma + 1, thirdComma);
//                 if ((message == ACKMessage) && (lengthOfMesssage == ACKMessageLength))
//                 {
//                     return true; // ACK received
//                 }
//             }
//         }
//     }
//     return false; // No ACK received within timeout
// }

void sendDropMessage(String payload)
{
    String messageSize = String(payload.length());
    loraSerial1.println("AT+SEND=" + oldChildAddress + "," + messageSize + "," + payload);
    Serial.println(payload);
    if (waitForAck())
    {
        DROPCounter = 0;
        return;
    }
    else
    {
        if (DROPCounter >= 5)
        {
            Serial.println("No payload ACK Drop message received, return to fallback state");
            return;
        }
        Serial.println("No ACK " + payload + " " + DROPCounter);
        DROPCounter++;
        sendDropMessage(payload);
        return;
    }
}

void loop()
{

    int randomAddress = random(2, 1000);
    String newChildAddress = String(randomAddress);
    if (Serial.available() > 0)
    {
        String userInput = Serial.readStringUntil('\n');
        if (userInput.startsWith("DROP_PAYLOAD"))
        {
            sendDropMessage(userInput);
        }
    }
    if (counter == 0)
    {
        counter = 5;
        String messageToSend = newChildAddress;
        String messageLength = String(messageToSend.length());
        loraSerial1.println("AT+SEND=" + oldChildAddress + "," + messageLength + "," + messageToSend);
        Serial.println("Sent random address:" + oldChildAddress + " to " + newChildAddress);
        if (waitForAck())
        {
            oldChildAddress = newChildAddress;
            Serial.println("ACK received, updated oldChildAddress to " + oldChildAddress);
        }
        else
        {
            Serial.println("No ACK received, keeping oldChildAddress as " + oldChildAddress);
        }
    }
    else
    {
        Serial.println("Sending message");
        loraSerial1.println("AT+SEND=" + oldChildAddress + ",5,Hello");
    }

    counter--;

    delay(2000);
}

/**
 * @brief Hardware interrupt that can be used instead of software interrupts
 *
 */
// void serialEvent1()
// {
//     while (loraSerial1.available())
//     {
//         String receivedMessage = loraSerial1.readStringUntil('\n');
//         receivedMessage.trim();
//         Serial.println(receivedMessage);
//         if (receivedMessage.startsWith("+RCV"))
//         {
//             int firstComma = receivedMessage.indexOf(',');
//             int secondComma = receivedMessage.indexOf(',', firstComma + 1);
//             int thirdComma = receivedMessage.indexOf(',', secondComma + 1);
//             String message = receivedMessage.substring(secondComma + 1, thirdComma);
//             if (message == ACKMessage)
//             {
//                 Serial.println("New addy: " + message);
//                 // return true; // ACK received
//             }
//         }
//     }Do
// }