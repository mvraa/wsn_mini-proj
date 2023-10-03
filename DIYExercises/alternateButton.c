#include "contiki.h"
#include "dev/light-sensor.h"
#include <stdio.h> /* For printf() */
/*-----------------------------------------------------------*/
PROCESS(alt_button_process, "Alt Button Process");
PROCESS(counter_process, "Counter Process");
AUTOSTART_PROCESSES(&alt_button_process, &counter_process);
/*-----------------------------------------------------------*/
// Define global variables
bool buttonWasPressed =false;
int THRESHOLD = 135; //TODO find out threshold
int counter = 0;
bool countUp = true;
PROCESS_THREAD(alt_button_process, ev, data)
{    
    // Define local variables (reset with every process call)
    PROCESS_BEGIN();        
        while(1) {
            //Light
            SENSORS_ACTIVATE(light_sensor); //Activate
            int val = light_sensor.value(LIGHT_SENSOR_PHOTOSYNTHETIC);
            int light = 10 * val / 7;
			//printf("Light value: %d \n", light);

            if (light >= THRESHOLD){
                // Button not pressed
                if(buttonWasPressed == true){
                    buttonWasPressed = false;
                }
            }
            else{
                // Button pressed
                buttonWasPressed = true;
            }
            SENSORS_DEACTIVATE(light_sensor); //Deactivaet Light Sensor
			PROCESS_PAUSE();
        }
    
    PROCESS_END();
}

PROCESS_THREAD(counter_process, ev, data)
{
    static struct etimer timer;
    
    // Define local variables (reset with every process call)
    PROCESS_BEGIN();
        etimer_set(&timer, CLOCK_SECOND); // Initiate timer to trigger event
        
        while(1) {
            
			 
            printf("Counter: %d \n", counter);
			if (countUp == true){ counter++;}
			else {counter--;}
            
            PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer)); // Wait until time is expired
            etimer_reset(&timer); // Reset timer
            
        }
    
    PROCESS_END();
}