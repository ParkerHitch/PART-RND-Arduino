
void setup()
{
};

void sendMessage()
{
}
void printTime(){

}
void loop()
{
    unsigned long currentMilli = millis();
    unsigned long seconds = currentMilli / 1000;
    unsigned long minutes = seconds / 60;
    unsigned long hours = minutes / 60;

    Serial.println(String(hours) + "H:" + String(minutes) + "M:" + String(seconds)+"S");
    sendMessage();
    // Await response from the child
    Serial.println(String(hours) + "H:" + String(minutes) + "M:" + String(seconds)+"S");

    delay(60000); // 60 seconds * 1000 milliseconds
}