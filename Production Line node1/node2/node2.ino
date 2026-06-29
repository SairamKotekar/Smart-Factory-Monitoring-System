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

static SensorManager  sensors;


void setup() 
{
  Serial.begin(9600);
  Serial.print("Node2 is booting....");
 //mqttClient.setCallback(rpc_mqttCallback);

  // put your setup code here, to run once:
   // initilase sensor 
   sensor_begin();
   sensor_init(&sensors);
   //initialise actutors
   actuators_begin();

   telemetry_init(sharedBuf,sizeof(sharedBuf));
   rpc_init(&mqttClient,sharedBuf,sizeof(sharedBuf));
   mqttClient.setCallback(rpc_mqttCallback);
   // connect board to internet and mqtt
   network_begin(&mqttClient);

  
}

void loop() 
{
  // put your main code here, to run repeatedly:
  //keep checking if board is connectrd to THingsBoard
  network_maintain();

  //read data from the sensors
  SensorData data;
  sensors_read( &data, &sensors);    //humidity,temp,ldr,door status, motion status

  if(data.humidity >= HUMIDITY_CRIT && !actuators_getRelayState())
  {
    actuators_setRelay(1);
  }

  //publish to the cloud 
  //check if network is avilable then push to thingsboard
  if(network_isConnected())
  { unsigned long now = millis();
    //publish data to thingsboard  every for 5s
    if( now - lastTelemetry >= TELEMETRY_INTERVAL )   ///if 5s is over
    {
      lastTelemetry = now;
      telemetry_publishTelemetry( &data, actuators_getRelayState());     //converting data into json and publish   
    }
  }

  actuators_updateStatusLEDs(network_isConnected(), data.sensorError);
   

}
