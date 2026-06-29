#include "SensorManager.h"
#include "config.h"
#include <DHT.h>

static DHT dht(PIN_DHT,DHT_TYPE);

void sensor_init  (SensorManager* sm)
{
  sm->lastTemp        = 0.0f;
  sm->lastHumidity    = 0.0f;
  sm->motionActive    = false;
  sm->motionClearTime = 0;
}




void sensor_begin(void)
{
  pinMode(PIN_PIR, INPUT);
  pinMode(PIN_DOOR, INPUT_PULLUP);
  dht.begin();
}

// read sensor of PIR
static bool readPIR(SensorManager* sm)
{    //read value from motion sensor ,check if it is true for 10s
  const bool pirHigh = (digitalRead(PIN_PIR)) == HIGH;
  const unsigned long now = millis();
  if(pirHigh)
  {
    sm->motionActive = true;
    sm->motionClearTime = now + MOTION_CLEAR_MS;
  }
  else if(sm->motionActive && (now > sm->motionClearTime))
  {
    sm->motionActive = false;
  }

  return sm->motionActive;               // 1 -> motion, 0 -> no motion
}

//read door status
static bool readDoor(void)
{
  return (digitalRead(PIN_DOOR) == LOW);         //0 if door is open ,1 if door is closed
}

//read ldr sensor
static uint16_t readLDR(void)
{
  uint16_t sum = 0;
  uint8_t i;
  for(i = 0; i<4; i++)          //avg of 4 readings
  {
    sum += (uint16_t)analogRead(PIN_LDR);   //0 to 1023
    delay(2);     //2ms
  }
  return sum/4;           //warehouse is dark or not , value is <300 ,dark, light is avilable
}

//read data from srnsor , return status of DHT22->returns 0 on error
bool sensors_read(SensorData * out, SensorManager* sm)
{
  //read dht22 sensor,
  float t = dht.readTemperature();
  float h = dht.readHumidity();

  if( isnan(t) || isnan(h))    //if error in dht sensor
  {
    out->sensorError = 1;
    out->temperature = sm->lastTemp ;
    out->humidity    = sm->lastHumidity ;
  }
  else
  {
    out->sensorError = 0;
    out->temperature = round(t * 10.0f) * 0.1f;
    out->humidity     = round(h * 10.0f) * 0.1f;
    sm->lastTemp  =  out->temperature;
    sm->lastHumidity = out->humidity;
  }

    //read motion dectected
    out->motionDetected = readPIR(sm);
    //read door status
    out->doorOpen = readDoor();
    //read LDR sensor
    out->ldrValue = readLDR();
    out->isDark   =  (out->ldrValue < LDR_DARK_THRESH);   //value is <300 ,dark, light is avilable
    
  return ! out->sensorError;
}