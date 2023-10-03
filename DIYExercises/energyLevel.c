#include "contiki.h"
#include <stdio.h> /* For printf() */
#include "sys/energest.h"
/*-----------------------------------------------------------*/
PROCESS(energy_level_process, "Energy level process");
AUTOSTART_PROCESSES(&energy_level_process);
/*-----------------------------------------------------------*/
// Define global variables
static inline unsigned long
to_seconds(uint64_t time)
{
  return (unsigned long)(time / ENERGEST_SECOND);
}

PROCESS_THREAD(energy_level_process, ev, data)
{
		static struct etimer timer;
        // Define local variables (reset with every process call)
        PROCESS_BEGIN();
		
        etimer_set(&timer, CLOCK_SECOND * 2); // Initiate timer to trigger event
        while(1) {
			
			PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer)); // Wait until time is expired
			etimer_reset(&timer); // Reset time
			
			
			/* Update all energest times. */
			energest_flush();
			
			printf("\nEnergest:\n");
			printf(" CPU          %4lus LPM      %4lus DEEP LPM %4lus  Total time %lus\n",
           to_seconds(energest_type_time(ENERGEST_TYPE_CPU)),
           to_seconds(energest_type_time(ENERGEST_TYPE_LPM)),
           to_seconds(energest_type_time(ENERGEST_TYPE_DEEP_LPM)),
           to_seconds(ENERGEST_GET_TOTAL_TIME()));
		
		}
                           
PROCESS_END();
}
