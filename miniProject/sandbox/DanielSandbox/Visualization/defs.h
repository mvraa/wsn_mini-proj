#ifndef UTILS
#define UTILS
#include <inttypes.h>

// sensors
#include "sht11-sensor.h" // temperature and humidity
#include "dev/light-sensor.h" // light

// RPL connection
#define WITH_SERVER_REPLY  1
#define UDP_CLIENT_PORT 8765
#define UDP_SERVER_PORT 5678

// Logic
#define MEASURES 10 // Amount of accumulated client measurements before sending measurements to intermediate
#define SEND_INTERVAL         (MEASURES * CLOCK_SECOND)
#define MEASURE_INTERVAL (1*CLOCK_SECOND)
#define DATA_PACKAGES_INTERMEDIATE 5 //Amount of data packages the intermediate mote will receive before aggregation and forwarding to sink
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

void dataRawPrint(Data *d);
Data getSensorData();
Data getAverage(Data d[DATA_PACKAGES_INTERMEDIATE][MEASURES]);
void printf_float(float x, int p);

void printRawData(Data *d){
    printf("%d %d %d %d ", (int)d->temp, (int)d->hum, (int)d->photo, (int)d->solar);
}

Data getSensorData(){
  Data r;
  r.temp = sht11_sensor.value(SHT11_SENSOR_TEMP); // temperature
  r.hum = sht11_sensor.value(SHT11_SENSOR_HUMIDITY); // humidity       
  r.photo = light_sensor.value(LIGHT_SENSOR_PHOTOSYNTHETIC); // photo light      
  r.solar = light_sensor.value(LIGHT_SENSOR_TOTAL_SOLAR); // solar light

  return r;  
}

Data getAverage(Data d[DATA_PACKAGES_INTERMEDIATE][MEASURES]){
  Data r;

  // sum up measurements in received measurements
  for (int i = 0; i < DATA_PACKAGES_INTERMEDIATE; i++){
    for (int j = 0 ; j < MEASURES; j++){
        r.temp += d[i][j].temp;
        r.hum += d[i][j].hum;
        r.photo += d[i][j].photo;
        r.solar += d[i][j].solar;
    }
  }

  // average summed up measurements
  int n = DATA_PACKAGES_INTERMEDIATE * MEASURES;
  r.temp = r.temp / n;
  r.hum = r.hum / n;
  r.photo = r.photo / n;
  r.solar = r.solar / n;

  return r;
}

void printf_float(float x, int p) {
    // Check if x is negative
    if (x < 0) {
        printf("-");
        x = -x;
    }
    int i = (int)x; // integer part
    printf("%d", i); // print integer part

    // is p is positive
    if (p > 0) {
        printf("."); // decimal point
        float f = x - i; // the fractional part of x

        for (int j = 0; j < p; j++) {
            // multiply f by 10 and get the integer part
            f = f * 10;
            i = (int)f;

            printf("%d", i); // print integer part
            f = f - i; // fractional part
        }
    }
}

#endif