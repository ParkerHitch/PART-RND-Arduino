#include <Servo.h>
#include <SoftwareSerial.h>

Servo continuousServo;
SoftwareSerial loraSerial2(4, 5); // RX2, TX2 (Module 2)

struct ContinousServoValues
{
    const int dropOneBottle = 500;
    const int stationary = 90;
    const int dropAllBottles = dropOneBottle * 3; // need an offset as well

} servoValues;
bool randomizeCancellation = false;
const int ledPin = 13;
const int servoPin = 9;
const String DropPayloadBegin_String = "DROP_PAYLOAD";
void setup()
{
    continuousServo.attach(servoPin);
    Serial.begin(9600);
    pinMode(ledPin, OUTPUT);
    // digitalWrite(ledPin, LOW); // Cut the led off
    loraSerial2.begin(9600);
    handleAddressChange("2");
    Serial.println("Setup complete.");
}

bool isValidInteger(String str)
{
    for (int i = 0; i < str.length(); i++)
    {
        if (!isDigit(str[i]))
        {
            return false;
        }
    }
    return true;
}

void handlePayloadDrop(String PayloadMessage)
{
    Serial.println("Handle Payload Drop " + PayloadMessage);
    continuousServo.write(servoValues.dropOneBottle);
    delay(500);
    continuousServo.write(servoValues.stationary);
    sendACK("1");
    return;
}

void handleAddressChange(String address)
{
    loraSerial2.println("AT+OPMODE=1");
    loraSerial2.println("AT+RESET");
    loraSerial2.println("AT+ADDRESS=" + address);
    loraSerial2.println("AT+ADDRESS?");
    if (loraSerial2.available())
    {
        String response = loraSerial2.readString();
        Serial.println(response);
        sendACK("1");
    }
}

void sendACK(String address)
{
    loraSerial2.println("AT+SEND=" + address + ",3,ACK");
}

void loop()
{
    if (loraSerial2.available())
    {
        String receivedMessage = loraSerial2.readStringUntil('\n');
        receivedMessage.trim();
        Serial.println(receivedMessage);
        if (receivedMessage.startsWith("+RCV"))
        {
            int firstComma = receivedMessage.indexOf(',');
            int secondComma = receivedMessage.indexOf(',', firstComma + 1);
            int thirdComma = receivedMessage.indexOf(',', secondComma + 1);
            String thirdValue = receivedMessage.substring(secondComma + 1, thirdComma);
            Serial.println(thirdValue);
            if (isValidInteger(thirdValue))
            {
                int receivedAddress = thirdValue.toInt();
                Serial.println("Address change has been requested: " + String(thirdValue));
                handleAddressChange(thirdValue);
                sendACK("1");
                Serial.println("Address change complete to: " + String(thirdValue));
                delay(1000);
            }
            else if (thirdValue.startsWith(DropPayloadBegin_String))
            {
                handlePayloadDrop(receivedMessage);
            }
        }
    }
    delay(2000);
}
