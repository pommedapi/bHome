#include <Wire.h>
#include <VirtualWire.h>
#include <bHome_ledManager.h>
#include <bHome_common.h>
#include <bHome_rf.h>
#include <bHome_i2c.h>

#define BHOME_DEBUG 0

// LED pins
#define PIN_LED_RED 6
#define PIN_LED_GREEN 5
#define PIN_LED_BLUE 3

// RF pins
#define PIN_TX 4
#define PIN_RX 2

#define RF_ADDRESS 1
#define SLAVE_ADDRESS 0x04

// i2c
int i2cMode = -1;
uint8_t i2cReturnValue[VW_MAX_MESSAGE_LEN + 1]; // 1 for the length byte
unsigned long i2cWrite = 0;

bHome::ledManager led;
bHome::RF rf(RF_ADDRESS);

unsigned long timerec1, timerec2, timeem1, timeem2;

void setup()
{
  #if BHOME_DEBUG == 1
  Serial.begin(9600);
  Serial.println("start");
  #endif
  
  vw_set_tx_pin(PIN_TX);
  vw_set_rx_pin(PIN_RX);
  vw_setup(DEFAULT_RF_BAUD_RATE);
  
  led.setRGB();
  led.setCommonCathode();
  led.setRedPin(PIN_LED_RED);
  led.setGreenPin(PIN_LED_GREEN);
  led.setBluePin(PIN_LED_BLUE);
  led.setMode(LED_STANDBY);

  // initialize i2c as slave
  Wire.begin(SLAVE_ADDRESS);
  // define callbacks for i2c communication
  Wire.onReceive(receiveData);
  Wire.onRequest(sendData);
}

void sendSignal()
{
  #if BHOME_DEBUG == 1
  Serial.print("Data: ");
  for (int i = 0; i < rf.getEmitSignal().length; i++)
  {
    Serial.print(rf.getEmitSignal().message[i]);
    Serial.print(" ");
  }
  Serial.println("");
  #endif
  vw_send(rf.getEmitSignal().message, rf.getEmitSignal().length);
  vw_wait_tx();
}

void sendBit(boolean b)
{
  if (b)
  {
    digitalWrite(PIN_TX, HIGH);
    delayMicroseconds(310);   //275 orinally, but tweaked.
    digitalWrite(PIN_TX, LOW);
    delayMicroseconds(1340);  //1225 orinally, but tweaked.
  }
  else
  {
    digitalWrite(PIN_TX, HIGH);
    delayMicroseconds(310);   //275 orinally, but tweaked.
    digitalWrite(PIN_TX, LOW);
    delayMicroseconds(310);   //275 orinally, but tweaked.
  }
}

void sendPair(boolean b)
{
    sendBit(b);
    sendBit(!b);
}

void sendHomeEasySignal()
{
  // LATCHES
  digitalWrite(PIN_TX, HIGH);
  delayMicroseconds(275);
  digitalWrite(PIN_TX, LOW);
  delayMicroseconds(9900);
  digitalWrite(PIN_TX, HIGH);
  delayMicroseconds(275);
  digitalWrite(PIN_TX, LOW);
  delayMicroseconds(2675);
  digitalWrite(PIN_TX, HIGH);
  // Sending emitter code 00011101010110011000111010 (7693882)
  sendPair(false);
  sendPair(false);
  sendPair(false);
  sendPair(true);
  sendPair(true);
  sendPair(true);
  sendPair(false);
  sendPair(true);
  sendPair(false);
  sendPair(true);
  sendPair(false);
  sendPair(true);
  sendPair(true);
  sendPair(false);
  sendPair(false);
  sendPair(true);
  sendPair(true);
  sendPair(false);
  sendPair(false);
  sendPair(false);
  sendPair(true);
  sendPair(true);
  sendPair(true);
  sendPair(false);
  sendPair(true);
  sendPair(false);
  // Group
  sendPair(false);
  // State
  if (rf.getHEEmitSignal().state == 1)
    sendPair(true);
  else
    sendPair(false);
  // Receptor
  sendPair(false);
  sendPair(false);
  if (rf.getHEEmitSignal().receiver == 3)
  {
    sendPair(true);
    sendPair(false);
  }
  else if (rf.getHEEmitSignal().receiver == 2)
  {
    sendPair(false);
    sendPair(true);
  }
  else if (rf.getHEEmitSignal().receiver == 1)
  {
    sendPair(false);
    sendPair(false);
  }
 
  digitalWrite(PIN_TX, HIGH);
  delayMicroseconds(275);
  digitalWrite(PIN_TX, LOW);
  delayMicroseconds(2675);
}
 
void loop()
{
  led.run();
  if (rf.isSignalReady())
  {
    led.setMode(LED_TRANSMIT);
    #if BHOME_DEBUG == 1
    Serial.println("Sending signal");
    #endif
    sendSignal();
    if ((rf.getEmitSignal().type / 10) != (BROADCAST_MODE / 10))
    {
      #if BHOME_DEBUG == 1
      Serial.println("Starting reception");
      #endif
      vw_rx_start();
      led.setMode(LED_RECEIVE);
      if (vw_wait_rx_max(DEFAULT_RF_RX_TIMEOUT))
      {
        if (rf.signalReceived())
        {
          led.setMode(LED_BUSY);
          i2cReturnValue[1] = (uint8_t) rf.getReceivedSignal().emitter;
          i2cReturnValue[0] = 1;
        }
        else
        {
          led.setMode(LED_ERROR);
          i2cReturnValue[1] = 0;
          i2cReturnValue[0] = 1;
        }
      }
      else
      {
        led.setMode(LED_ERROR);
        i2cReturnValue[1] = 0;
        i2cReturnValue[0] = 1;
      }
      vw_rx_stop();
    }
    rf.resetSignal();
    led.setMode(LED_STANDBY);
  }
  if (rf.isHESignalReady())
  {
    led.setMode(LED_TRANSMIT);
    #if BHOME_DEBUG == 1
    Serial.println("Sending home easy signal");
    Serial.print("Receiver: ");
    Serial.println(rf.getHEEmitSignal().receiver);
    Serial.print("State: ");
    Serial.println(rf.getHEEmitSignal().state);
    #endif
    for (int i = 0; i < 5; i++)
    {
      sendHomeEasySignal();
      delay(20);
    }
    led.setMode(LED_STANDBY);
    rf.resetHESignal();
  }
}

// callback for received data
void receiveData(int byteCount)
{
  unsigned int tmp, i2cNumber;
  int type = 0;
  i2cNumber = 0;
  for (int i = 0; i < byteCount; i++)
  {
    tmp = Wire.read();
    if (i == 0)
    {
      #if BHOME_DEBUG == 1
      if (tmp == I2C_EMIT_SIGNAL)
        Serial.println("Mode: emit signal");
      else if (tmp == I2C_EMIT_HOME_EASY_SIGNAL)
        Serial.println("Mode: emit home easy signal");
      else if (tmp == I2C_GET_RESPONSE)
        Serial.println("Mode: get response");
      else if (tmp == I2C_SET_LED_MODE)
        Serial.println("Mode: set LED mode");
      else
      #endif
      if (tmp == I2C_SWITCH_LED_DIM)
      {
        #if BHOME_DEBUG == 1
        Serial.println("Mode: switch LED dim");
        #endif
        led.switchDim();
      }
      i2cMode = tmp;
    }
    else
    {
      if (i2cMode == I2C_EMIT_SIGNAL)
      {
        if (i == 1)
          rf.setReceiver(tmp);
        else if (i == 2)
          rf.setSender(tmp);
        else if (i == 3)
          rf.setType(tmp);
        else
          rf.setData(rf.getEmitSignal().data + tmp * bHome::power2(8 * (i - 4)));
      }
      else if (i2cMode == I2C_EMIT_HOME_EASY_SIGNAL)
      {
        if (i == 1)
          rf.setHEReceiver(tmp);
        else
          rf.setHEState(tmp);
      }
      else if (i2cMode == I2C_SET_LED_MODE)
        led.setMode((int) tmp);
      Serial.println(tmp);
    }
  }
  if (i2cMode == I2C_EMIT_SIGNAL)
    rf.setSignalReady();
  else if (i2cMode == I2C_EMIT_HOME_EASY_SIGNAL)
    rf.setHESignalReady();
  else if (i2cMode == I2C_GET_RESPONSE)
  {
    int dataLen;
    i2cReturnValue[1] = rf.getReceivedSignal().emitter;
    i2cReturnValue[2] = rf.getReceivedSignal().type;
    dataLen = bHome::itobb(rf.getReceivedSignal().data, &i2cReturnValue[3]);
    i2cReturnValue[0] = 3 + dataLen;
  }
  i2cMode = -1;
}
 
// callback for sending data
void sendData()
{
  Wire.write(i2cReturnValue, i2cReturnValue[0]);
}
