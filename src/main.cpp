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
  delay(2000);
  Serial.println();
  Serial.print(F("-- uBEACON - VERSION: "));
  Serial.print(VERSION);
  Serial.println(F(" --"));

  // EEPROM Backup
  EEPROM.begin(512);

  morseInit();
  terminalInit();
}
// -- MAIN  ------------------------------------------------------
void loop()
{
  transmitTask();
  receiveCLI();
}

// --  FONCTIONS  ------------------------------------------------------
void transmitTask(void) {
  currMillis = millis();
  if (((currMillis - lastMillis) > (1000 * DELAY_TX)) /*|| (lastMillis == 0)*/) {
    lastMillis = currMillis;
    Serial.println();
    Serial.print("TX CLK");
    Serial.print(currOut);
    Serial.print(": ");
    play_message(morse_msg[0], currOut);
    currOut++;
    if (currOut >= 3) {
      currOut = 0;
    }
  }
}
