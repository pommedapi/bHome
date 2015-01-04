#include <VirtualWire.h>
#include <bHome_common.h>
#include <bHome_ledManager.h>
#include <bHome_rf.h>

#define BHOME_DEBUG 0

// LED pins
#define PIN_LED_RED 6
#define PIN_LED_GREEN 5
#define PIN_LED_BLUE 3

// RF pins
#define PIN_TX 4
#define PIN_RX 2

#define RF_ADDRESS 2

#define PIN_RELAY 12
#define PIN_PIR 11

boolean motion = false;
int daylight = INFO_DAYLIGHT_NORMAL;
int daylightThreshold = INFO_DAYLIGHT_LOW;
boolean nightMode = false;
boolean simulationMode = false;
boolean alarmMode = false;
boolean relayForced = false;

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
  vw_rx_start();
  
  led.setRGB();
  led.setCommonCathode();
  led.setRedPin(PIN_LED_RED);
  led.setGreenPin(PIN_LED_GREEN);
  led.setBluePin(PIN_LED_BLUE);
  
  led.setMode(LED_STANDBY);
  pinMode(PIN_RELAY, OUTPUT);
  pinMode(PIN_PIR, INPUT);
}

void processSignal()
{
  unsigned char data[rf.getReceivedSignal().size];
  unsigned int module;

  if (rf.getReceivedSignal().type == SEND_SIMPLE_CMD)
  {
    boolean state;
    int j = bHome::itobb(rf.getReceivedSignal().data, &data[0]);
    if (j == 2)
    {
      module = data[1];
      state = (data[0] == 1) ? true : false;
      if (module == PIN_RELAY)
      {
        digitalWrite(PIN_RELAY, ((state) ? HIGH : LOW));
        relayForced = true;
        rf.setType(SIMPLE_CMD_ACK);
        rf.setData(state);
      }
      else
      {
        led.setMode(LED_ERROR);
        if (module == PIN_PIR)
          rf.setType(ERROR_CMDTYPE_NOT_AVAIL_MODULE);
        else
          rf.setType(ERROR_UNKNOWN_MODULE);
      }
      rf.setSignalReady();
    }
  }
  else if (rf.getReceivedSignal().type == SEND_LIGHT_THRESHOLD)
  {
    int j = bHome::itobb(rf.getReceivedSignal().data, &data[0]);
    if (j == 2)
    {
      module = data[1];
      if (module == PIN_RELAY)
      {
        #if BHOME_DEBUG == 1
        Serial.print("Light threshold: ");
        Serial.println(data[0]);
        #endif
        daylightThreshold = data[0];
        rf.setType(SIMPLE_CMD_ACK);
        rf.setData(daylightThreshold);
      }
      else
      {
        led.setMode(LED_ERROR);
        if (module == PIN_PIR)
          rf.setType(ERROR_CMDTYPE_NOT_AVAIL_MODULE);
        else
          rf.setType(ERROR_UNKNOWN_MODULE);
      }
      rf.setSignalReady();
    }
  }
  else if (rf.getReceivedSignal().type == SEND_RESTORE_STATE)
  {
    int j = bHome::itobb(rf.getReceivedSignal().data, &data[0]);
    if (j == 1)
    {
      module = data[0];
      if (module == PIN_RELAY)
      {
        #if BHOME_DEBUG == 1
        Serial.println("Restore relay state");
        #endif
        relayForced = false;
        rf.setType(SIMPLE_CMD_ACK);
        rf.setData(module);
      }
      else
      {
        led.setMode(LED_ERROR);
        if (module == PIN_PIR)
          rf.setType(ERROR_CMDTYPE_NOT_AVAIL_MODULE);
        else
          rf.setType(ERROR_UNKNOWN_MODULE);
      }
      rf.setSignalReady();
    }
  }
  else if (rf.getReceivedSignal().type == REQUEST_SENSOR_VALUE)
  {
    if (rf.getReceivedSignal().data == PIN_PIR)
    {
      #if BHOME_DEBUG == 1
      Serial.println("Request PIR value");
      #endif
      rf.setType(SENSOR_VALUE_RESPONSE);
      rf.setData(motion);
    }
    else
    {
      led.setMode(LED_ERROR);
      if (module == PIN_RELAY)
        rf.setType(ERROR_CMDTYPE_NOT_AVAIL_MODULE);
      else
        rf.setType(ERROR_UNKNOWN_MODULE);
    }
    rf.setSignalReady();
  }
  else if (rf.getReceivedSignal().type == REQUEST_SENSOR_TYPE)
  {
    if (rf.getReceivedSignal().data == PIN_PIR)
    {
      #if BHOME_DEBUG == 1
      Serial.println("Request PIR type");
      #endif
      rf.setType(SENSOR_TYPE_RESPONSE);
      rf.setData(SENSOR_TYPE_PIR);
    }
    else
    {
      led.setMode(LED_ERROR);
      if (module == PIN_RELAY)
        rf.setType(ERROR_CMDTYPE_NOT_AVAIL_MODULE);
      else
        rf.setType(ERROR_UNKNOWN_MODULE);
    }
    rf.setSignalReady();
  }
  else if (rf.getReceivedSignal().type == BROADCAST_STATUS_SIMPLE)
  {
    #if BHOME_DEBUG == 1
    Serial.print("Broadcast status cmd: ");
    Serial.println(rf.getReceivedSignal().data);
    #endif
    if ((rf.getReceivedSignal().data == 1) || (rf.getReceivedSignal().data == 0))
      digitalWrite(PIN_RELAY, ((rf.getReceivedSignal().data) ? HIGH : LOW));
    else
      led.setMode(LED_ERROR);
  }
  else if (rf.getReceivedSignal().type == BROADCAST_MODE)
  {
    if (rf.getReceivedSignal().data == MODE_ALARM_ENABLE)
      alarmMode = true;
    else if (rf.getReceivedSignal().data == MODE_ALARM_DISABLE)
      alarmMode = false;
    else if (rf.getReceivedSignal().data == MODE_PRESENCE_SIMULATION_ENABLE)
      simulationMode = true;
    else if (rf.getReceivedSignal().data == MODE_PRESENCE_SIMULATION_DISABLE)
      simulationMode = false;
    else if (rf.getReceivedSignal().data == MODE_NIGHT_ENABLE)
      nightMode = true;
    else if (rf.getReceivedSignal().data == MODE_NIGHT_DISABLE)
      nightMode = false;
  }
  else if (rf.getReceivedSignal().type == BROADCAST_INFO)
  {
    #if BHOME_DEBUG == 1
    Serial.print("Broadcast info: ");
    Serial.println(rf.getReceivedSignal().data);
    #endif
    if ((rf.getReceivedSignal().data >= INFO_DAYLIGHT_NIGHT) && (rf.getReceivedSignal().data <= INFO_DAYLIGHT_SUNLIGHT))
      daylight = rf.getReceivedSignal().data;
  }
  else if ((rf.getReceivedSignal().type / 10) == (BROADCAST_MODE / 10))
  {
    // Just ignore
  }
}

void loop()
{
  led.run();
  if (vw_have_message() && rf.signalReceived())
  {
    led.setMode(LED_BUSY);
    led.run();
    #if BHOME_DEBUG == 1
    Serial.println("Transmission received");
    #endif
    processSignal();
    #if BHOME_DEBUG == 1
    Serial.println("Signal processed");
    #endif
    led.run();
    if (rf.isSignalReady())
    {
      vw_rx_stop();
      rf.setReceiver(rf.getReceivedSignal().emitter);
      led.setMode(LED_TRANSMIT);
      led.run();
      #if BHOME_DEBUG == 1
      Serial.println("Sending response");
      #endif
      sendSignal();
      rf.resetSignal();
      led.run();
      #if BHOME_DEBUG == 1
      Serial.println("Sent");
      #endif
      vw_rx_start();
    }
  }
  else
  {
    led.setMode(LED_STANDBY);
    if (motion != ((digitalRead(PIN_PIR) == HIGH) ? true : false))
    {
      motion = ((digitalRead(PIN_PIR) == HIGH) ? true : false);
      #if BHOME_DEBUG == 1
      Serial.println("Motion changed");
      if (motion == true)
        Serial.println("Motion detected");
      else
        Serial.println("No motion now");
      #endif
    }
    if ((relayForced == false) && (alarmMode == false) && (simulationMode == false))
    {
      if ((motion == true) && (daylight <= daylightThreshold))
        digitalWrite(PIN_RELAY, HIGH);
      else
        digitalWrite(PIN_RELAY, LOW);
    }
  }
  led.run();
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
  #if BHOME_DEBUG == 1
  delay(50);
  #else
  delay(100);
  #endif
  vw_send(rf.getEmitSignal().message, rf.getEmitSignal().length);
  vw_wait_tx();
}
