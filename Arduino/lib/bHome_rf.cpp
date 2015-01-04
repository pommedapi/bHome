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
#include "bHome_rf.h"
#include "bHome_common.h"
#include "VirtualWire.h"

namespace bHome
{


RF::RF(int address)
 :m_address(address)
{
  m_signalToEmit.receiver = 0;
  m_signalToEmit.type = 0;
  m_signalToEmit.data = 0;
  m_signalToEmit.toSend = false;
  m_signalToEmit.length = 0;
}

RF::~RF()
{
}

void RF::setReceiver(unsigned int receiver)
{
  m_signalToEmit.receiver = receiver;
}

void RF::setSender(unsigned int sender)
{
  m_signalToEmit.sender = sender;
}

void RF::setType(unsigned int type)
{
  m_signalToEmit.type = type;
}

void RF::setData(unsigned int data)
{
  m_signalToEmit.data = data;
}

void RF::setSignalReady()
{
  m_signalToEmit.toSend = true;
}

bool RF::isSignalReady()
{
  return m_signalToEmit.toSend;
}

void RF::resetSignal()
{
  m_signalToEmit.receiver = 0;
  m_signalToEmit.sender = 0;
  m_signalToEmit.type = 0;
  m_signalToEmit.data = 0;
  m_signalToEmit.toSend = false;
  m_signalToEmit.length = 0;
}

signalToEmit_t RF::getEmitSignal()
{
  m_signalToEmit.message[0] = m_signalToEmit.receiver;
  if (m_signalToEmit.sender == 0)
    m_signalToEmit.message[1] = m_address;
  else
    m_signalToEmit.message[1] = m_signalToEmit.sender;
  m_signalToEmit.message[2] = m_signalToEmit.type;
  int tmp = itobb(m_signalToEmit.data, &m_signalToEmit.message[3]);
  m_signalToEmit.length = 3 + tmp;
  return m_signalToEmit;
}

void RF::setHEReceiver(unsigned int receiver)
{
  m_homeEasySignalToEmit.receiver = receiver;
}

void RF::setHEState(unsigned int state)
{
  m_homeEasySignalToEmit.state = state;
}

void RF::setHESignalReady()
{
  m_homeEasySignalToEmit.toSend = true;
}

bool RF::isHESignalReady()
{
  return m_homeEasySignalToEmit.toSend;
}

void RF::resetHESignal()
{
  m_homeEasySignalToEmit.receiver = 0;
  m_homeEasySignalToEmit.state = 0;
  m_homeEasySignalToEmit.toSend = false;
}

signal_emit_homeeasy_t RF::getHEEmitSignal()
{
  return m_homeEasySignalToEmit;
}

bool RF::signalReceived()
{
  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;

  m_receivedSignal.emitter = 0;
  m_receivedSignal.type = 0;
  m_receivedSignal.size = 0;
  m_receivedSignal.data = 0;
  //m_receivedSignal.countI2C = 0;
  m_receivedSignal.error = false;

  vw_get_message(buf, &buflen);
  Serial.print("Receiver: ");
  Serial.println(buf[0]);
  Serial.print("Emitter: ");
  Serial.println(buf[1]);
  Serial.print("Type: ");
  Serial.println(buf[2]);
  if ((buf[0] == m_address) || (buf[0] == m_broadcastAddress))
  {
    m_receivedSignal.emitter = buf[1];
    m_receivedSignal.type = buf[2];
    m_receivedSignal.size = buflen - 3;
    for (int i = 0; i < m_receivedSignal.size; i++)
      m_receivedSignal.data += buf[i + 3] * power2(8 * i);
    Serial.print("Data: ");
    Serial.println(m_receivedSignal.data);
    Serial.print("Size: ");
    Serial.println(m_receivedSignal.size);
    return true;
  }
  else
  {
    m_receivedSignal.error = true;
    return false;
  }
}

receivedSignal_t RF::getReceivedSignal()
{
  return m_receivedSignal;
}

}
