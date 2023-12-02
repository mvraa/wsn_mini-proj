#include "utils.h"


// Reused from assignment 1. Changed it to take floats as inputs. We probably don't need to work with doubles for this project
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

// Just copied this from the other project.
// TODO: Test it and see if it makes sense
void PrintEnergestMeasurement(char c, int8_t txPower, int runTime) {
  energest_flush(); // kan muligvis godt være at dette skal ændres så vi udregner fra den forrige måling.
  uint64_t cpuMeasurement = energest_type_time(ENERGEST_TYPE_CPU);      // We assume that Current Consumption: MCU on, Radio off 1800 µA is for this energest. 
  uint64_t lpmMeasurement = energest_type_time(ENERGEST_TYPE_LPM);      
  uint64_t txMeasurement = energest_type_time(ENERGEST_TYPE_TRANSMIT);
  uint64_t rxMeasurement = energest_type_time(ENERGEST_TYPE_LISTEN);

  printf("CPU clocks %" PRIu64 "\n", cpuMeasurement);
  printf("LPM clocks %" PRIu64 "\n", lpmMeasurement);
  printf("TX  clocks %" PRIu64 "\n", txMeasurement);
  printf("Listen clocks %" PRIu64 "\n", rxMeasurement);
 
  double current_tx_mode; // in mA
  float voltage = 3;
  if(c == 't' && (txPower >= -25 && txPower <= 0)) {

    switch(txPower) {
      case -25:
        current_tx_mode = 8.5;
        break;
      case -15:
        current_tx_mode = 9.9;
        break;
      case -10:
        current_tx_mode = 11.2;
        break;
      case -5:
        current_tx_mode = 13.9;
        break;
      default:
        current_tx_mode = 17.4;
    }

  } else {
    current_tx_mode = 17.4;
  }
  
  // https://stackoverflow.com/questions/45644277/how-to-calculate-total-energy-consumption-using-cooja
  // Values from datasheet
  float current_rx_mode = 19.7;
  float current_cpu_active = 1.8;
  float current_cpu_idle = 0.0026;
  
  double current = (  txMeasurement * current_tx_mode + rxMeasurement * current_rx_mode + 
                        cpuMeasurement * current_cpu_active + lpmMeasurement * current_cpu_idle) 
                        / RTIMER_ARCH_SECOND;
  
  uint64_t power = current * voltage; 

  printf("Power used total : ");
  printf("%" PRIu64 "mW\n", power);

  printf("Power used minus listening: ");
  current = (  txMeasurement * current_tx_mode + 
                        cpuMeasurement * current_cpu_active + lpmMeasurement * current_cpu_idle) 
                        / RTIMER_ARCH_SECOND;
  power = (current) * voltage;  
  printf("%" PRIu64 "mW\n", power);                 
  printf("clock_time: %d", (uint16_t)clock_time());
}