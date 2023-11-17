#ifndef UTILS
#define UTILS
#include <inttypes.h> 


#define WITH_SERVER_REPLY  1
#define UDP_CLIENT_PORT	8765
#define UDP_SERVER_PORT	5678
#define SEND_INTERVAL		  (10 * CLOCK_SECOND)
#define MEASURE_INTERVAL (1*CLOCK_SECOND)

// Structs to send data via RPL ports
typedef struct Data
{
    float temp;
    float hum;
    float photo;
    float solar;
} Data;

typedef struct AggregatedData
{
    float Average;
    uint8_t Min;
    uint8_t Max;
    uint8_t Median;
    int NumMeasurements;
} AggregatedData;

#endif