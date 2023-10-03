#include "contiki.h"
#include "sht11-sensor.h"
#include "dev/light-sensor.h"
#include <stdio.h> /* For printf() */
/*-----------------------------------------------------------*/
PROCESS(hello_world_process, "Hello world process");
AUTOSTART_PROCESSES(&hello_world_process);
/*-----------------------------------------------------------*/
// Define global variables
PROCESS_THREAD(hello_world_process, ev, data)
{
    static struct etimer timer;
    
    // Define local variables (reset with every process call)
    PROCESS_BEGIN();
        etimer_set(&timer, CLOCK_SECOND); // Initiate timer to trigger event
        
        while(1) {
            // Temperature
            SENSORS_ACTIVATE(sht11_sensor); //Activate Sensor
            int val = sht11_sensor.value(SHT11_SENSOR_TEMP);
            int temp = -39.6 + 0.01*val;
            printf("Temperature value: %d \n", temp);

            //Light
            SENSORS_ACTIVATE(light_sensor); //Activate
            val = light_sensor.value(LIGHT_SENSOR_PHOTOSYNTHETIC);
            int light = val * 2.288818;
            printf("Light value: %d \n", light);
            
            PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer)); // Wait until time is expired
            etimer_reset(&timer); // Reset timer
            SENSORS_DEACTIVATE(sht11_sensor); //Deactivate Temp Sensor
            SENSORS_DEACTIVATE(light_sensor); //Deactivaet Light Sensor
        }
    
    PROCESS_END();
}