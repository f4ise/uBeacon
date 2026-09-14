#include <Arduino.h>
#include <EEPROM.h>
#include <si5351.h>
#include <Wire.h>

#include "config.h"
#include "morse.h"
#include "serialCLI.h"

String morse_msg[] = {
  MSG_01,
  MSG_02
};

unsigned long currMillis = 0;
unsigned long lastMillis = 0;

uint8_t currOut = 0;

void transmitTask(void);

void setup()
{
  Serial.begin(115200);
  delay(1000);
  Serial.println();
  Serial.print(F("-- uBEACON - VERSION: "));
  Serial.print(VERSION);
  Serial.println(F(" --"));
  byte value = 0;
  // EEPROM Backup
  EEPROM.begin(256);
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, i);
  }
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    value = EEPROM.read(i);
    Serial.print(i);
    Serial.print("\t");
    Serial.print(value, DEC);
    Serial.println();
  }

  morseInit();
  terminalInit();
}
// -- MAIN  ------------------------------------------------------
void loop()
{
  transmitTask();
  //receiveCLI();
}

// --  FONCTIONS  ------------------------------------------------------
void transmitTask(void) {
  currMillis = millis();
  if ((currMillis - lastMillis) > (1000 * DELAY_TX)) {
    lastMillis = currMillis;
    Serial.println();
    play_message(morse_msg[0], currOut);
    currOut++;
    if (currOut >= 3) {
      currOut = 0;
    }
  }
}