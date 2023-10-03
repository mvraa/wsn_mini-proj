#include "contiki.h"
#include <stdio.h> /* For printf() */
/*-----------------------------------------------------------*/
PROCESS(hello_world_process, "Hello world process");
AUTOSTART_PROCESSES(&hello_world_process);
/*-----------------------------------------------------------*/
// Define global variables
int counter = 0;
PROCESS_THREAD(hello_world_process, ev, data)
{
	static struct etimer timer;
        // Define local variables (reset with every process call)
        PROCESS_BEGIN();
        etimer_set(&timer, CLOCK_SECOND * 2); // Initiate timer to trigger event
        while(1) {
        	printf("Counter: %d \n", counter++);
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer)); // Wait until time is expired
		etimer_reset(&timer); // Reset time
		}
                           
PROCESS_END();
}
