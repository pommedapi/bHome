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

#ifndef BHOME_RF_H_INCLUDED
#define BHOME_RF_H_INCLUDED

#include "Arduino.h"
#include "VirtualWire.h"

// Communication
#define ERROR_RANGE_LOW 1
#define ERROR_RANGE_HIGH 10
// Communication types
#define SEND_CMD 10
#define SEND_SIMPLE_CMD 11
#define SEND_COMPLEX_CMD 12
#define SEND_DIM_CMD 13
#define SEND_COLOR_CMD 14
#define SEND_VALUE_CMD 15
#define SEND_LIGHT_THRESHOLD 16
#define SEND_RESTORE_STATE 17
#define REQUEST 20
#define REQUEST_SENSOR_VALUE 21
#define REQUEST_SENSOR_TYPE 22
#define REQUEST_STATUS_SIMPLE 23
#define REQUEST_STATUS_COMPLEX 24
#define REQUEST_DIM_VALUE 25
#define REQUEST_COLOR_VALUE 26
#define REQUEST_TEMP_VALUE 27
#define REQUEST_HUMIDITY_VALUE 28
#define BROADCAST 30
#define BROADCAST_MODE 31
#define BROADCAST_STATUS_SIMPLE 32
#define BROADCAST_STATUS_COMPLEX 33
#define BROADCAST_DIM_VALUE 34
#define BROADCAST_COLOR_VALUE 35
#define BROADCAST_VALUE 36
#define BROADCAST_MODE 37
#define BROADCAST_INFO 38
// Broadcast types
#define MODE_ALARM_ENABLE 1
#define MODE_ALARM_DISABLE 2
#define MODE_PRESENCE_SIMULATION_ENABLE 3
#define MODE_PRESENCE_SIMULATION_DISABLE 4
#define MODE_NIGHT_ENABLE 5
#define MODE_NIGHT_DISABLE 6
#define INFO_DAYLIGHT_NIGHT 1
#define INFO_DAYLIGHT_LOW 2
#define INFO_DAYLIGHT_NORMAL 3
#define INFO_DAYLIGHT_BRIGHT 4
#define INFO_DAYLIGHT_SUNLIGHT 5
// Response types
#define ERROR_GENERIC 1
#define ERROR_UNKNOWN_MODULE 2
#define ERROR_CMDTYPE_NOT_HANDLED 3
#define ERROR_CMDTYPE_NOT_AVAIL_MODULE 4
#define ERROR_NO_RESPONSE_FROM_MOBULE 5
#define SIMPLE_CMD_ACK 51
#define COMPLEX_CMD_ACK 52
#define DIM_CMD_ACK 53
#define COLOR_CMD_ACK 54
#define VALUE_CMD_ACK 55
#define SENSOR_VALUE_RESPONSE 61
#define SENSOR_TYPE_RESPONSE 62
#define STATUS_SIMPLE_RESPONSE 63
#define STATUS_COMPLEX_RESPONSE 64
#define DIM_VALUE_RESPONSE 65
#define COLOR_VALUE_RESPONSE 66
#define TEMP_VALUE_RESPONSE 67
#define HUMIDITY_VALUE_RESPONSE 68
// Responses
#define SENSOR_TYPE_PIR 1
#define SENSOR_TYPE_LUX 2
#define SENSOR_TYPE_DHT11 3
#define SENSOR_TYPE_DHT22 4
#define SENSOR_TYPE_TEMP 5
#define SENSOR_TYPE_HUMIDITY 6
#define SENSOR_TYPE_MOIST 7
#define SENSOR_TYPE_HALL 8
#define SENSOR_TYPE_REED 9
#define SENSOR_TYPE_ULTRASOUND 10
#define SENSOR_TYPE_PHOTORESISTOR 11
#define SENSOR_TYPE_PRESSURE 12
#define SENSOR_TYPE_VIBRATION 13

#define DEFAULT_RF_BAUD_RATE 2000
#define DEFAULT_RF_RX_TIMEOUT 1000
#define DEFAULT_RF_BROADCAST_ADDRESS 255

namespace bHome
{

struct signalToEmit_t
{
  unsigned int receiver;
  unsigned int sender;
  unsigned int type;
  unsigned int data;
  bool toSend;
  uint8_t message[VW_MAX_MESSAGE_LEN];
  int length;
};

struct signal_emit_homeeasy_t
{
  unsigned int receiver;
  unsigned int state;
  bool toSend;
};

struct receivedSignal_t
{
  unsigned int emitter;
  unsigned int type;
  unsigned int size;
  unsigned int data;
  //int countI2C;
  boolean error;
};

class RF
{
public:
  RF(int address);
  ~RF();
  // Emition
  void setReceiver(unsigned int receiver);
  void setSender(unsigned int sender);
  void setType(unsigned int type);
  void setData(unsigned int data);
  void setSignalReady();
  bool isSignalReady();
  void resetSignal();
  signalToEmit_t getEmitSignal();
  // Home Easy emition
  void setHEReceiver(unsigned int receiver);
  void setHEState(unsigned int data);
  void setHESignalReady();
  bool isHESignalReady();
  void resetHESignal();
  signal_emit_homeeasy_t getHEEmitSignal();
  // Reception
  bool signalReceived();
  receivedSignal_t getReceivedSignal();

protected:

private:
  int m_baudRate = DEFAULT_RF_BAUD_RATE;
  int m_rxTimeout = DEFAULT_RF_RX_TIMEOUT;
  int m_broadcastAddress = DEFAULT_RF_BROADCAST_ADDRESS;
  int m_txPin;
  int m_rxPin;
  int m_address;
  bool m_server;
  signalToEmit_t m_signalToEmit;
  signal_emit_homeeasy_t m_homeEasySignalToEmit;
  receivedSignal_t m_receivedSignal;
};

}

#endif // BHOME_RF_H_INCLUDED
