
#include <Arduino.h>
#include "functions.h"
const String ACKMessage = "ACK";
const String ACKMessageLength = "3";
/**
 * @brief
 *
 * @return true
 * @return false
 */
bool waitForAck()
{
    long timeout = millis() + 5000;

    while (millis() < timeout)
    {
        if (loraSerial1.available())
        {
            String receivedMessage = loraSerial1.readStringUntil('\n');
            receivedMessage.trim();
            // Serial.println(receivedMessage);
            if (receivedMessage.startsWith("+RCV"))
            {
                int firstComma = receivedMessage.indexOf(',');
                int secondComma = receivedMessage.indexOf(',', firstComma + 1);
                int thirdComma = receivedMessage.indexOf(',', secondComma + 1);
                String lengthOfMesssage = receivedMessage.substring(firstComma + 1, secondComma);
                String message = receivedMessage.substring(secondComma + 1, thirdComma);
                if ((message == ACKMessage) && (lengthOfMesssage == ACKMessageLength))
                {
                    return true; // ACK received
                }
            }
        }
    }
    return false; // No ACK received within timeout
}