#include "network.h"
#include "config.h"


static PubSubClient* mqtt      = NULL;
static byte          mac[6]    = NODE_MAC;
static unsigned long lastReconnectTime = 0;

static void doConnect(void)
{
  if(mqtt->connect(CLIENT_ID,ACCESS_TOKEN,NULL))
  {
    Serial.print("connected");
    mqtt->subscribe("v1/devices/me/rpc/request/+");
  }
  else
  {
    Serial.print("mqtt failed");
  }
}


void network_begin(PubSubClient* mqttClient)
{
  mqtt = mqttClient;

  //connect board to internet
  Ethernet.begin(mac);
  delay(2000);

  //connect to ThingsBoard
  mqtt->setServer(TB_SERVER, TB_PORT);            //-> or .
  doConnect();
} 

void network_maintain(void)
{
  Ethernet.maintain();

  // iif board connected to THingsBoard try to connect 5s
  if(mqtt->connected())
  {
    mqtt->loop();
  }
  else
  {
    unsigned long now = millis();
    if( now - lastReconnectTime >= RECONNECT_INTERVAL )
    {
      lastReconnectTime = now;
      doConnect();
    }
  }

}

uint8_t network_isConnected(void)
{
  return (uint8_t) mqtt->connected();
}


uint8_t network_publish(const char* topic, const char* payload, uint8_t retain)
{
  return (uint8_t) mqtt->publish(topic, payload,retain);
}
