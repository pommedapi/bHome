/*
The MIT License (MIT)

Copyright (c) 2015 Matthieu BARGHEON

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef BHOME_LEDMANAGER_H_INCLUDED
#define BHOME_LEDMANAGER_H_INCLUDED

#include "Arduino.h"

// Led statuses
#define LED_STANDBY 0
#define LED_TRANSMIT 1
#define LED_RECEIVE 2
#define LED_BUSY 3
#define LED_ERROR 4
#define LED_BLINK 5
#define LED_OFF 6

// Led colors
#define COLOR_WHITE 0
#define COLOR_RED 1
#define COLOR_GREEN 2
#define COLOR_BLUE 3
#define COLOR_YELLOW 4
#define COLOR_CYAN 5
#define COLOR_VIOLET 6
#define COLOR_ORANGE 7
#define COLOR_PINK 8

#define LED_BREATHE_OFF_DURATION 10000
#define LED_BREATHE_ON_DURATION 2000
#define LED_BLINK_OFF_DURATION 1000
#define LED_BLINK_ON_DURATION 50
#define LED_DIM 0.05

namespace bHome
{

class ledManager
{
public:
  ledManager();
  ~ledManager();
  void setRedPin(int pin);
  void setGreenPin(int pin);
  void setBluePin(int pin);
  void setRGB();
  void setSingleColor();
  void setCommonCathode();
  void setCommonAnode();
  void setDim(bool dim);
  void switchDim();
  void setMode(int mode);
  void run();

protected:
  void setColorForMode();
  void setColor(int color);
  void breathe();
  void blink();
  void switchOff();

private:
  bool m_isRGB = true;
  bool m_commonCathode = true;
  bool m_dim = false;
  unsigned long m_delayTimer = 0;
  unsigned long m_breatheTimer = 0;
  unsigned long m_breatheOffDuration = LED_BREATHE_OFF_DURATION;
  unsigned long m_breatheDuration = LED_BREATHE_ON_DURATION;
  unsigned long m_blinkOffDuration = LED_BLINK_OFF_DURATION;
  unsigned long m_blinkOnDuration = LED_BLINK_ON_DURATION;
  //unsigned long m_blinkTimer = 0;
  bool m_blink = false;
  float m_redValue = 1.0;
  float m_greenValue = 1.0;
  float m_blueValue = 1.0;
  unsigned short m_pin;
  unsigned short m_redPin;
  unsigned short m_greenPin;
  unsigned short m_bluePin;
  unsigned short m_mode;

};

}

#endif // BHOME_LEDMANAGER_H_INCLUDED
