#include "Arduino.h"

void setup() {
    Serial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
    Serial.println("On");
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);

    Serial.println("Off");
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
}