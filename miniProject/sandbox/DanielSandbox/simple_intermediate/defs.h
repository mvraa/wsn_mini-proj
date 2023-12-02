#ifndef UTILS
#define UTILS
#include <inttypes.h> 

// RPL connection
#define WITH_SERVER_REPLY  1
#define UDP_CLIENT_PORT	8765
#define UDP_SERVER_PORT	5678

// Logic
#define MEASURES 10 //Amount of measures on a mote before sending to sink and overwriting previous
#define SEND_INTERVAL		  (MEASURES * CLOCK_SECOND)
#define MEASURE_INTERVAL (1*CLOCK_SECOND)
#define DATA_PACKAGES_INTERMEDIATE 5 //Amount of data packages the intermediate mote will receive before aggregation and forwarding to sing
#define AGG_DATA_PACKAGE_LENGTH 10 

// Struct the client nodes use to store data. 
// After writing MEASURES amount, send a package of DATA_PACKAGE_LENGTH  length to the sink
typedef struct Data
{
    float temp;
    float hum;
    float photo;
    float solar;
} Data;

typedef struct AggregatedData
{
    float avgTemp;
    float avgHum;
    float avgPhoto;
    float avgSolar;
} AggregatedData;

#endif