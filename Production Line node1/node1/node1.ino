//include required libraries
#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <DHT.h>

#include "config.h"
#include "SensorManager.h"
#include "actuator.h"
#include "network.h"
#include "telemetry.h"
#include "rpc.h"

static char sharedBuf[200];

// create object of type ethernet pubsubClient
static EthernetClient ethClient;
static PubSubClient mqttClient(ethClient);


unsigned long lastTelemetry = 0;


void setup() 
{
  Serial.begin(9600);
  Serial.print("Node1 is booting....");

  // put your setup code here, to run once:
   // initilase sensor 
   sensor_begin();
   //initialise actutors
   actuators_begin();

   telemetry_init(sharedBuf,sizeof(sharedBuf));
   rpc_init(&mqttClient,sharedBuf,sizeof(sharedBuf));

   mqttClient.setCallback(rpc_mqttCallback);

   // connect board to internet and mqtt
   network_begin(&mqttClient);
   mqttClient.setCallback(rpc_mqttCallback);

   digitalWrite(PIN_LED_RED, LOW);

}

void loop() 
{
  // put your main code here, to run repeatedly:
  //keep checking if board is connectrd to THingsBoard
  network_maintain();

  //read data from the sensors
  SensorData data;
  sensors_read( &data);    //humidity,temp,lm35,vibration,sensor error,machine status

  //publish to the cloud 
  //check if network is avilable then push to thingsboard
  if(network_isConnected())
  { unsigned long now = millis();
    //publish data to thingsboard  every for 5s
    if( now - lastTelemetry >= TELEMETRY_INTERVAL )   ///if 5s is over
    {
      lastTelemetry = now;
      telemetry_publishTelemetry( &data, actuators_getRelayState);     //converting data into json and publish   
    }
  }

   actuators_updateStatusLEDs(network_isConnected(), data.sensorError);
   

}
