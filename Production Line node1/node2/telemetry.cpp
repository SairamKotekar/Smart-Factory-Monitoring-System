#include "telemetry.h"
#include "network.h"
#include <stdlib.h>

static char* buffer = NULL;
static uint8_t len = 0;

//to convert float value to string
static char* f1( float v, char *dst)
{
  return dtostrf(v , 1 , 1 , dst);
}


void telemetry_init(char* buf, uint8_t bufLen)
{
  buffer = buf;
  len = bufLen;
}
void telemetry_publishTelemetry(const SensorData* data, uint8_t relayState)
{
  char t[8], h[8];
  f1(data->temperature,t);
  f1(data->humidity,h);
  

  // put data in JSON format
   snprintf_P(buffer,len,PSTR("{\"temperature\":%s, \"humidity\":%s, \"ldrValue\":%u,"
                                "\"isDark\":%u,\"motionDetected\":%u,\"relayState\":%u,"
                                "\"doorOpen\":%u,\"sensorError\":%u}"), t , h ,
                                (unsigned)data->ldrValue,
                                (unsigned)data->isDark,
                                (unsigned)data->motionDetected,
                                (unsigned)relayState,
                                (unsigned)data->doorOpen,
                                (unsigned)data->sensorError
                                );
    network_publish(TOPIC_TELEMETRY,buffer,0);
}

/*
  snprintf_P(buffer, len, PSTR("{\"temperature\":%s,\"humidity\":%s,"
                                "\"ldrValue\":%u,\"isDark\":%u,"
                                "\"motionDetected\":%u,\"relayState\":%u,"
                                "\"doorOpen\":%u,\"sensorError\":%u}"),t, h,
                                (unsigned)data->ldrValue,
                                (unsigned)data->isDark,
                                (unsigned)data->motionDetected,
                                (unsigned)relayState,
                                (unsigned)data->doorOpen,
                                (unsigned)data->sensorError
                                );

  network_publish(TOPIC_TELEMETRY,buffer,0);
}

#include "telemetry.h"
#include "network.h"
#include <stdlib.h>

static char* buffer = NULL;
static uint16_t len = 0; // Changed to uint16_t to support buffers > 255

// Function to convert float to string
static char* f1(float v, char *dst) {
    return dtostrf(v, 1, 2, dst); // Changed precision to 2 for better data
}

void telemetry_init(char* buf, uint16_t bufLen) {
    buffer = buf;
    len = bufLen;
}

void telemetry_publishTelemetry(const SensorData* data, uint8_t relayState) {
    if (buffer == NULL || len == 0) return;

    char t[10], h[10];
    f1(data->temperature, t);
    f1(data->humidity, h);
    

    // Fixed the formatting and the relayState variable access
    snprintf_P(buffer, len, PSTR("{\"temperature\":%s,\"humidity\":%s,\"ldrValue\":%u,\"isDark\":%u,\"motionDetected\":%u,\"relayState\":%u,\"doorOpen\":%u,\"sensorError\":%u}"), 
        t, 
        h, 
        (unsigned int)data->ldrValue,
        (unsigned int)data->isDark,
        (unsigned int)data->motionDetected,
        (unsigned int)relayState, // relayState is passed as an argument, not from data struct
        (unsigned int)data->doorOpen,
        (unsigned int)data->sensorError
    );

    network_publish(TOPIC_TELEMETRY, buffer, 0);
}
*/

