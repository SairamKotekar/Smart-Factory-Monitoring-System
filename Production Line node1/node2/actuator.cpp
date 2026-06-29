#include "actuator.h"

static uint8_t relay_status;

static uint8_t blinkstate = 0;
static unsigned long lastBlinkTime = 0;

void actuators_begin(void)
{
  pinMode(PIN_LED_GREEN  , OUTPUT);
  pinMode(PIN_LED_YELLOW , OUTPUT);
  pinMode(PIN_LED_RED , OUTPUT);
  pinMode(PIN_RELAY , OUTPUT);
 

//in the beginning till network connection led will be on
  digitalWrite(PIN_LED_GREEN, LOW);   //everything is normal
  digitalWrite(PIN_LED_YELLOW, LOW);  //on warning ,blinking critical
  digitalWrite(PIN_LED_RED, LOW); 
  digitalWrite(PIN_RELAY, LOW);       //ventilation, if more hummidity
}


void    actuators_setRelay(uint8_t on)
{
  relay_status = on;
  digitalWrite(PIN_RELAY, on ? HIGH : LOW);
}
uint8_t actuators_getRelayState(void)
{
  return relay_status;
}

void    actuators_updateStatusLEDs(uint8_t mqttConnected, uint8_t sensorError)
{
 //if board not connected to things board turn on red led
  if(!mqttConnected)
  {
    digitalWrite(PIN_LED_RED, HIGH);
    digitalWrite(PIN_LED_GREEN, LOW);
    digitalWrite(PIN_LED_YELLOW, LOW);
    return;
  }

    digitalWrite(PIN_LED_RED, LOW);
  //if every thing is normal turn on green led
  if(sensorError)
  {
     digitalWrite(PIN_LED_GREEN, HIGH);
     unsigned long now = millis();
     if(now - lastBlinkTime > 500)     //half sec delay
     {
      lastBlinkTime = now;
      blinkstate = !blinkstate;
      digitalWrite(PIN_LED_YELLOW, blinkstate ? HIGH : LOW);
     }
  }
    else
    {
      digitalWrite(PIN_LED_GREEN, HIGH);
      digitalWrite(PIN_LED_YELLOW, LOW);
    }
  
  //if sensor error blink green led
}
