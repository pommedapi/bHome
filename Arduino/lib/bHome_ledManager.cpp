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

#include "Arduino.h"
#include "bHome_ledManager.h"
#include "VirtualWire.h"

namespace bHome
{

ledManager::ledManager()
{
}

ledManager::~ledManager()
{
}


void ledManager::setRedPin(int pin)
{
  m_redPin = pin;
  pinMode(pin, OUTPUT);
}

void ledManager::setGreenPin(int pin)
{
  m_greenPin = pin;
  pinMode(pin, OUTPUT);
}

void ledManager::setBluePin(int pin)
{
  m_bluePin = pin;
  pinMode(pin, OUTPUT);
}

void ledManager::setRGB()
{
  m_isRGB = true;
}

void ledManager::setSingleColor()
{
  m_isRGB = false;
}

void ledManager::setCommonCathode()
{
  m_commonCathode = true;
}

void ledManager::setCommonAnode()
{
  m_commonCathode = false;
}

void ledManager::setDim(bool dim)
{
  m_dim = dim;
}

void ledManager::switchDim()
{
  m_dim = !m_dim;
}

void ledManager::setMode(int mode)
{
  m_mode = mode;
}

void ledManager::run()
{
  if (m_mode == LED_STANDBY)
    breathe();
  else if (m_mode == LED_OFF)
    switchOff();
  else
    blink();
}

void ledManager::setColorForMode()
{
  if (m_mode == LED_TRANSMIT)
    setColor(COLOR_BLUE);
  else if (m_mode == LED_RECEIVE)
    setColor(COLOR_VIOLET);
  else if (m_mode == LED_BUSY)
    setColor(COLOR_CYAN);
  else if (m_mode == LED_ERROR)
    setColor(COLOR_RED);
  else if (m_mode == LED_BLINK)
    setColor(COLOR_WHITE);
}

void ledManager::setColor(int color)
{
  // http://www.rapidtables.com/web/color/RGB_Color.htm
  m_redValue = 1.0;
  m_greenValue = 1.0;
  m_blueValue = 1.0;
  if (color == COLOR_RED)
  {
    m_greenValue = 0.0;
    m_blueValue = 0.0;
  }
  else if (color == COLOR_GREEN)
  {
    m_redValue = 0.0;
    m_blueValue = 0.0;
  }
  else if (color == COLOR_BLUE)
  {
    m_redValue = 0.0;
    m_greenValue = 0.0;
  }
  else if (color == COLOR_YELLOW)
    m_blueValue = 0.0;
  else if (color == COLOR_CYAN)
    m_redValue = 0.0;
  else if (color == COLOR_VIOLET)
  {
    m_redValue = 0.54;
    m_greenValue = 0.17;
    m_blueValue = 0.89;
  }
  else if (color == COLOR_ORANGE)
  {
    m_greenValue = 0.27;
    m_blueValue = 0.0;
  }
  else if (color == COLOR_PINK)
    m_greenValue = 0.0;
  if (m_commonCathode == false)
  {
    m_redValue = 1.0 - m_redValue;
    m_greenValue = 1.0 - m_greenValue;
    m_blueValue = 1.0 - m_blueValue;
  }
}

void ledManager::breathe()
{
  float breatheValue;

  analogWrite(m_redPin, 255.0);
  analogWrite(m_bluePin, 255.0);
  if (m_breatheTimer <= m_breatheDuration)
  {
    m_breatheTimer = millis() - m_delayTimer;
    if (m_breatheTimer <= (m_breatheDuration / 2))
      breatheValue = ((float) m_breatheTimer / ((float) m_breatheDuration / 2.0)) * 255.0;
    else
      breatheValue = 255.0 - (((float) m_breatheTimer - ((float) m_breatheDuration / 2.0)) / ((float) m_breatheDuration / 2.0)) * 255.0;
    if (m_dim)
      breatheValue = breatheValue * LED_DIM;
    analogWrite(m_greenPin, 255.0 - breatheValue);
  }
  else
  {
    analogWrite(m_greenPin, 255.0);
    if (m_breatheTimer <= (m_breatheDuration + m_breatheOffDuration))
      m_breatheTimer = millis() - m_delayTimer;
    else
    {
      m_delayTimer = millis();
      m_breatheTimer = 0;
    }
  }
}

void ledManager::blink()
{
  float dim = 1.0;
  unsigned long blinkTimer;

  blinkTimer = millis() - m_delayTimer;
  if ((m_blink == false) && (blinkTimer > m_blinkOffDuration))
  {
    m_blink = true;
    setColorForMode();
    if (m_dim == true)
      dim = LED_DIM;
    analogWrite(m_redPin, 255.0 - (255.0 * dim * m_redValue));
    analogWrite(m_greenPin, 255.0 - (255.0 * dim * m_greenValue));
    analogWrite(m_bluePin, 255.0 - (255.0 * dim * m_blueValue));
    m_delayTimer = millis();
  }
  else if ((m_blink == true) && (blinkTimer > m_blinkOnDuration))
  {
    m_blink = false;
    analogWrite(m_redPin, 255.0);
    analogWrite(m_greenPin, 255.0);
    analogWrite(m_bluePin, 255.0);
    m_delayTimer = millis();
  }
}

void ledManager::switchOff()
{
  if (m_commonCathode)
  {
    digitalWrite(m_redPin, HIGH);
    digitalWrite(m_greenPin, HIGH);
    digitalWrite(m_bluePin, HIGH);
  }
  else
  {
    digitalWrite(m_redPin, LOW);
    digitalWrite(m_greenPin, LOW);
    digitalWrite(m_bluePin, LOW);
  }
}

}
