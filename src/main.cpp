#include "config.h"
#include <Wire.h>
#include <FastLED.h>
#include <si5351.h>

#include "morse.h"

#define NUM_LEDS 1
#define DATA_PIN 16

CRGB leds[NUM_LEDS];

Si5351 si5351;

String VERSION = "1.1A";

unsigned long freqCLK[3] = {
     3568600,    // Frequence 80M WSPR
    14095600,    // Frequence 20M WSPR
//    28124600,    // Frequence 10M WSPR
   144489000    // Frequence 2M WSPR
//   121375000,    // Frequence EXERCICE AERO
};

byte freq_indx = 0;

String morse_msg[] = {
  MSG_01, 
  MSG_02
};

void setup()
{
  Serial.begin(115200);
  delay(1000);
  Serial.println();
  Serial.print(F("-- uBEACON - VERSION: "));
  Serial.print(VERSION);
  Serial.println(F(" --"));

  FastLED.addLeds<WS2812B, DATA_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(64);

  // Start serial and initialize the Si5351
  bool i2c_detect = si5351.init(SI5351_CRYSTAL_LOAD_8PF, 0, 0);
  if(!i2c_detect)
  {
    Serial.println(F("si5351 not detected on I2C bus!"));
  }
  else
  {
    si5351.set_pll(SI5351_PLL_FIXED, SI5351_PLLA);
    si5351.set_freq(freqCLK[freq_indx] * SI5351_FREQ_MULT, SI5351_CLK0);
    si5351.drive_strength(SI5351_CLK0, SI5351_DRIVE_2MA);
    si5351.output_enable(SI5351_CLK0, 0);   
    Serial.println(F("INIT si5351... CLK0"));

    si5351.set_freq(freqCLK[freq_indx+1] * SI5351_FREQ_MULT, SI5351_CLK1);
    si5351.drive_strength(SI5351_CLK1, SI5351_DRIVE_2MA);
    si5351.output_enable(SI5351_CLK1, 0);
    Serial.println(F("INIT si5351... CLK1"));
  
    si5351.set_freq(freqCLK[freq_indx+2] * SI5351_FREQ_MULT, SI5351_CLK2);
    si5351.drive_strength(SI5351_CLK2, SI5351_DRIVE_2MA);
    si5351.output_enable(SI5351_CLK2, 0);
    Serial.println(F("INIT si5351... CLK2"));
    
    Serial.println(F("INIT si5351... DONE"));
  }
}

// -- MAIN  ------------------------------------------------------
void loop()
{
  Serial.println();
  play_message(morse_msg[0]);
  delay(5000);
  Serial.println();
  play_message(morse_msg[1]);
  delay(30000);
}

// --  FONCTIONS  ------------------------------------------------------
void send_dot()
{
  delay(dot_length_ms);  // wait for one dot period (space)
  leds[0] = CRGB::Blue;
  FastLED.show();
  si5351.output_enable(SI5351_CLK0, 1);
  si5351.output_enable(SI5351_CLK1, 1);
  si5351.output_enable(SI5351_CLK2, 1);

  Serial.print(".");
  delay(dot_length_ms);
  leds[0] = CRGB::Black;
  FastLED.show();  
  si5351.output_enable(SI5351_CLK0, 0);
  si5351.output_enable(SI5351_CLK1, 0);
  si5351.output_enable(SI5351_CLK2, 0);
}

void send_dash()
{
  delay(dot_length_ms);
  leds[0] = CRGB::Blue;
  FastLED.show();
  si5351.output_enable(SI5351_CLK0, 1);
  si5351.output_enable(SI5351_CLK1, 1);
  si5351.output_enable(SI5351_CLK2, 1);

  Serial.print("-");
  delay(dot_length_ms * CW_DASH_LEN);
  leds[0] = CRGB::Black;
  FastLED.show();
  si5351.output_enable(SI5351_CLK0, 0);
  si5351.output_enable(SI5351_CLK1, 0);
  si5351.output_enable(SI5351_CLK2, 0);

}