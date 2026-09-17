#include <Arduino.h>
#include <EEPROM.h>

#include <FastLED.h>
#include <si5351.h>

#include "config.h"
#include "morse.h"

CRGB leds[NUM_LEDS];

Si5351 si5351;

unsigned long freqCLK[3] = {7040200, 50294600, 144490100};

morse_char_t MorseCode[] = {
    {'A', '.', '-',  0,   0,   0,   0},
    {'B', '-', '.', '.', '.',  0,   0},
    {'C', '-', '.', '-', '.',  0,   0},
    {'D', '-', '.', '.',  0,   0,   0},
    {'E', '.',  0,   0,   0,   0,   0},
    {'F', '.', '.', '-', '.',  0,   0},
    {'G', '-', '-', '.',  0,   0,   0},
    {'H', '.', '.', '.', '.',  0,   0},
    {'I', '.', '.',  0,   0,   0,   0},
    {'J', '.', '-', '-', '-',  0,   0},
    {'K', '-', '.', '-',  0,   0,   0},
    {'L', '.', '-', '.', '.',  0,   0},
    {'M', '-', '-',  0,   0,   0,   0},
    {'N', '-', '.',  0,   0,   0,   0},
    {'O', '-', '-', '-',  0,   0,   0},
    {'P', '.', '-', '-', '.',  0,   0},
    {'Q', '-', '-', '.', '-',  0,   0},
    {'R', '.', '-', '.',  0,   0,   0},
    {'S', '.', '.', '.',  0,   0,   0},
    {'T', '-',  0,   0,   0,   0,   0},
    {'U', '.', '.', '-',  0,   0,   0},
    {'V', '.', '.', '.', '-',  0,   0},
    {'W', '.', '-', '-',  0,   0,   0},
    {'X', '-', '.', '.', '-',  0,   0},
    {'Y', '-', '.', '-', '-',  0,   0},
    {'Z', '-', '-', '.', '.',  0,   0},
    {'0', '-', '-', '-', '-', '-',  0},
    {'1', '.', '-', '-', '-', '-',  0},
    {'2', '.', '.', '-', '-', '-',  0},
    {'3', '.', '.', '.', '-', '-',  0},
    {'4', '.', '.', '.', '.', '-',  0},
    {'5', '.', '.', '.', '.', '.',  0},
    {'6', '-', '.', '.', '.', '.',  0},
    {'7', '-', '-', '.', '.', '.',  0},
    {'8', '-', '-', '-', '.', '.',  0},
    {'9', '-', '-', '-', '-', '.',  0},
    {'/', '-', '.', '.', '-', '.',  0},
    {'?', '.', '.', '-', '-', '.', '.'},
    {'.', '.', '-', '.', '-', '.', '-'},
    {',', '-', '-', '.', '.', '-', '-'},
    {'(', '-', '.', '-', '.', '-', 0},
    {')', '.', '-', '.', '-', '.', 0},
    {'a', '.', '-',  0,   0,   0,   0},
    {'b', '-', '.', '.', '.',  0,   0},
    {'c', '-', '.', '-', '.',  0,   0},
    {'d', '-', '.', '.',  0,   0,   0},
    {'e', '.',  0,   0,   0,   0,   0},
    {'f', '.', '.', '-', '.',  0,   0},
    {'g', '-', '-', '.',  0,   0,   0},
    {'h', '.', '.', '.', '.',  0,   0},
    {'i', '.', '.',  0,   0,   0,   0},
    {'j', '.', '-', '-', '-',  0,   0},
    {'k', '-', '.', '-',  0,   0,   0},
    {'l', '.', '-', '.', '.',  0,   0},
    {'m', '-', '-',  0,   0,   0,   0},
    {'n', '-', '.',  0,   0,   0,   0},
    {'o', '-', '-', '-',  0,   0,   0},
    {'p', '.', '-', '-', '.',  0,   0},
    {'q', '-', '-', '.', '-',  0,   0},
    {'r', '.', '-', '.',  0,   0,   0},
    {'s', '.', '.', '.',  0,   0,   0},
    {'t', '-',  0,   0,   0,   0,   0},
    {'u', '.', '.', '-',  0,   0,   0},
    {'v', '.', '.', '.', '-',  0,   0},
    {'w', '.', '-', '-',  0,   0,   0},
    {'x', '-', '.', '.', '-',  0,   0},
    {'y', '-', '.', '-', '-',  0,   0},
    {'z', '-', '-', '.', '.',  0,   0}
};

void morseInit() {
  FastLED.addLeds<WS2812B, DATA_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(64);

  // Restore Config
  restoreConfig();

  // Start serial and initialize the Si5351
  bool i2c_detect = si5351.init(SI5351_CRYSTAL_LOAD_8PF, 0, 0);
  if(i2c_detect)
  {
    si5351.set_pll(SI5351_PLL_FIXED, SI5351_PLLA);
    si5351.set_freq(freqCLK[0] * SI5351_FREQ_MULT, SI5351_CLK0);
    si5351.drive_strength(SI5351_CLK0, SI5351_DRIVE_8MA);
    si5351.output_enable(SI5351_CLK0, 0);
    Serial.println(F("INIT si5351... CLK0"));

    si5351.set_freq(freqCLK[1] * SI5351_FREQ_MULT, SI5351_CLK1);
    si5351.drive_strength(SI5351_CLK1, SI5351_DRIVE_8MA);
    si5351.output_enable(SI5351_CLK1, 0);
    Serial.println(F("INIT si5351... CLK1"));

    si5351.set_freq(freqCLK[2] * SI5351_FREQ_MULT, SI5351_CLK2);
    si5351.drive_strength(SI5351_CLK2, SI5351_DRIVE_8MA);
    si5351.output_enable(SI5351_CLK2, 0);
    Serial.println(F("INIT si5351... CLK2"));

    Serial.println(F("INIT si5351... DONE"));
  }
  else
  {
    Serial.println(F("si5351 not detected on I2C bus!"));
  }
}

void restoreConfig(void) {
  //
}

void play_message(String m, uint8_t outX)
{
// sends the message in string 'm' as CW, with inter letter and word spacing
// s is the speed to play at; if s == 0, use the current speed  
  unsigned int i, j; 
  int n; 
  char buff[100];

  // use ch = m.charAt(index);
  m.toCharArray(buff, m.length()+1);

  for (i=0; i<m.length(); i++)
  {
    if(buff[i] == ' ') 
    {
       send_word_space(); 
    }
    else
    {
      if( (n = morse_lookup(buff[i])) == -1 )
      {
        Serial.print(F("char not found"));
      }
      else
      {
        // char found, so send it as dots and dashes
        // Serial.println(n);
        for(j=1; j<7; j++)
          send_morse_char(MorseCode[n].ch[j], outX);
        send_letter_space();  // send an inter-letter space
      }
    }
  }
}

int morse_lookup(char c)
// returns the index of parameter 'c' in MorseCode array, or -1 if not found
{
  unsigned int i;
  for(i=0; i<sizeof(MorseCode); i++)
  {
    if(c == MorseCode[i].ch[0])
      return i;
  }
  return -1; 
}

void send_morse_char(char dotdash, uint8_t outX)
{
  if(dotdash == '.') {
    Serial.print(F("."));
    send_dotdash(outX, dot_length_ms);
  }
  if(dotdash == '-') {
    Serial.print(F("-"));
    send_dotdash(outX, dot_length_ms * CW_DASH_LEN);
  }    
}

void send_letter_space()
{
  delay(dot_length_ms * 2);  // wait for 2 dot periods
}

void send_word_space()
{
  delay(dot_length_ms * 6);  // wait for 6 dot periods
  Serial.print(F(" "));
}

void send_dotdash(uint8_t outX, int duration)
{
  delay(dot_length_ms);  // wait for one dot period (space)
  switch(outX) {
    case SI5351_CLK0:
      leds[0] = CRGB::Blue;
      FastLED.show();
      si5351.output_enable(SI5351_CLK0, 1);
      delay(duration);
      leds[0] = CRGB::Black;
      FastLED.show();
      si5351.output_enable(SI5351_CLK0, 0);
      break;
    case SI5351_CLK1:
      leds[0] = CRGB::Yellow;
      FastLED.show();
      si5351.output_enable(SI5351_CLK1, 1);
      delay(duration);
      leds[0] = CRGB::Black;
      FastLED.show();
      si5351.output_enable(SI5351_CLK1, 0);
      break;
    case SI5351_CLK2:
      leds[0] = CRGB::Red;
      FastLED.show();
      si5351.output_enable(SI5351_CLK2, 1);
      delay(duration);
      leds[0] = CRGB::Black;
      FastLED.show();
      si5351.output_enable(SI5351_CLK2, 0);
      break;
    case 3:
      leds[0] = CRGB::Green;
      FastLED.show();
      si5351.output_enable(SI5351_CLK0, 1);
      si5351.output_enable(SI5351_CLK1, 1);
      si5351.output_enable(SI5351_CLK2, 1);
      delay(duration);
      leds[0] = CRGB::Black;
      FastLED.show();
      si5351.output_enable(SI5351_CLK0, 0);
      si5351.output_enable(SI5351_CLK1, 0);
      si5351.output_enable(SI5351_CLK2, 0);
      break;
  }
}