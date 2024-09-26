#include <SoftwareSerial.h>

// Define two software serial ports for the two RYLR993 modules
SoftwareSerial loraSerial2(4, 5); // RX2, TX2 (Module 2)
const int interruptPin = 12;      // Used for TX and RX with the pixhawk
bool randomizeCancellation = false;
const int ledPin = 13;
const String morseCode[10] = {
    ".----", // 0
    "..---", // 1
    "...--", // 2
    "....-", // 3
    ".....", // 4
    "-....", // 5
    "--...", // 6
    "---..", // 7
    "----.", // 8
    "-----"  // 9
};

void setup()
{
    Serial.begin(9600);      // For debugging
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, LOW);
    loraSerial2.begin(9600); // Module 2 (Receiver)
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
void sendMorseCode(const String &message){
    for (char c : message) {
    if (isDigit(c)) {
      String code = morseCode[c - '0'];
      for (char dotDash : code) {
        if (dotDash == '.') {
          digitalWrite(ledPin, HIGH); // Turn LED on
          delay(200); // Duration for a dot
          digitalWrite(ledPin, LOW); // Turn LED off
          delay(200); // Space between parts
        } else if (dotDash == '-') {
          digitalWrite(ledPin, HIGH); // Turn LED on
          delay(600); // Duration for a dash
          digitalWrite(ledPin, LOW); // Turn LED off
          delay(200); // Space between parts
        }
      }
      delay(600); // Space between letters
    }
  }
}
void handleAddressChange(String address)
{
    loraSerial2.println("AT+OPMODE=1");
    // delay(1000);
    loraSerial2.println("AT+RESET");
    // delay(1000);
    loraSerial2.println("AT+ADDRESS=" + address);
    // delay(500);
    loraSerial2.println("AT+ADDRESS?");
    if (loraSerial2.available())
    {
        String response = loraSerial2.readString();
        Serial.println(response);
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
                sendMorseCode(thirdValue);
                sendACK("1");
                Serial.println("Address change complete to: " + String(thirdValue));
                delay(1000);
            }
        }
    }
    delay(2000);
}
