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

#ifndef BHOME_NODE_H_INCLUDED
#define BHOME_NODE_H_INCLUDED

#include "Arduino.h"

#define BHOME_NODE_PIR
#define BHOME_NODE_LUX 2
#define BHOME_NODE_DHT11 3
#define BHOME_NODE_DHT22 4
#define BHOME_NODE_TEMP 5
#define BHOME_NODE_HUMIDITY 6
#define BHOME_NODE_MOIST 7
#define BHOME_NODE_HALL 8
#define BHOME_NODE_REED 9
#define BHOME_NODE_ULTRASOUND 10
#define BHOME_NODE_PHOTORESISTOR 11
#define BHOME_NODE_PRESSURE 12
#define BHOME_NODE_VIBRATION 13

namespace bHome
{

class bHomeNode
{
public:
    bHomeNode(int type);
    ~bHomeNode();
    // Configuration
    void setServer() { m_server = true; }
    void setNode() { m_server = false; }
    void setBaudRate(int baudRate) { m_baudRate = baudRate; }
    void setRxTimeout(int timeout) { m_rxTimeout = timeout; }
    // Emition
    void setReceiver(unsigned int receiver) { m_signalToEmit.receiver = receiver; }
    void setType(unsigned int type) { m_signalToEmit.type = type; }
    void setData(unsigned int data) { m_signalToEmit.data = data; }
    void setSignalReady() { m_signalToEmit.toSend = true; }
    void emit();
    // Reception
    void

protected:


private:
    int m_baudRate = DEFAULT_RF_BAUD_RATE;
    int m_rxTimeout = DEFAULT_RF_RX_TIMEOUT;
    boolean m_server = false;
    int m_txPin;
    int m_rxPin;
    int m_address;
    signalToEmit_t m_signalToEmit;
    receivedSignal_t m_receivedSignal;
};

}

#endif // BHOME_NODE_H_INCLUDED
