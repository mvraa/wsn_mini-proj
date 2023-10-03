#include "contiki.h"
#include "dev/button-sensor.h"
#include <stdio.h> /* For printf() */
/*-----------------------------------------------------------*/
PROCESS(counter_process, "counter process");
PROCESS(button_process, "button process");
AUTOSTART_PROCESSES(&counter_process,&button_process);
/*-----------------------------------------------------------*/
// Define global variables
int counter = 0;
bool countUp = true;
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

PROCESS_THREAD(button_process,ev, data)
{
	PROCESS_BEGIN();
	
	while (1){
		
		SENSORS_ACTIVATE(button_sensor);
		PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor); //Wait until button is pressed
		if (countUp == true){ countUp= false;}
		else {countUp = true;}
		
	}
	
	
	PROCESS_END();
}