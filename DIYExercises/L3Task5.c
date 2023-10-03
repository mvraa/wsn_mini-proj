#include "contiki.h"
#include "dev/button-sensor.h"
#include "leds.h"
#include <stdio.h> /* For printf() */
/*-----------------------------------------------------------*/
PROCESS(button_process, "counter process");
AUTOSTART_PROCESSES(&button_process);
/*-----------------------------------------------------------*/
// Define global variables
int arr[]={LEDS_RED, LEDS_GREEN, LEDS_YELLOW};
int count = 0;

PROCESS_THREAD(button_process, ev, data)
{   
    // Define local variables (reset with every process call)
    PROCESS_BEGIN();
        
        
        while(1) {

            SENSORS_ACTIVATE(button_sensor);
			PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor); //Wait until button is pressed	
			int x = count %3;
			leds_off(arr[x-1]);
			leds_on(arr[x]);
			count++;		
        }
    
    PROCESS_END();
}
