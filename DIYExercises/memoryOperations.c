#include "contiki.h"
#include <stdio.h> /* For printf() */
#include "lib/memb.h"

#define NUM_STRUCTS 3

/*-----------------------------------------------------------*/
PROCESS(memory_operations_process, "Memory operations process");
AUTOSTART_PROCESSES(&memory_operations_process);
/*-----------------------------------------------------------*/
/* 
 * This function implements the following requirements from the DIY exercise:
 * Create a struct with two integer values - store three of them in memory
 * •Delete the second struct
 * •Check if second struct is still stored
 * •Remove all stored data


 * •Store a string in a file
 * •Print contents of the file
 * •Append ”whoops. Somethings wrong” to the file
 * •Print contents of the file to make sure it worked
 * •Delete file
*/
static void
array_test(){
	
	struct twoIntegers {
		int value1;
		int value2;
	};
	
	MEMB(twoIntegers_mem, struct twoIntegers, NUM_STRUCTS);
	memb_init(&twoIntegers_mem);
	
	for ( int i = 0; i <NUM_STRUCTS; i++){
		struct twoIntegers *tI; 
		tI = memb_alloc(&twoIntegers_mem);
		tI->value1 = i;
		tI->value2 = i+1;
	}
	if(memb_free(&twoIntegers_mem, &twoIntegers_mem[1])==0){printf("Block successfully deallocated\n");}
	printf("inmemb returned: %d",memb_inmemb(&twoIntegers_mem, &twoIntegers_mem[1]));
	
}


PROCESS_THREAD(memory_operations_process, ev, data)
{
		static struct etimer timer;
        // Define local variables (reset with every process call)
        PROCESS_BEGIN();
		
        etimer_set(&timer, CLOCK_SECOND * 20); // Initiate timer to trigger event
        while(1) {
			
			PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer)); // Wait until time is expired
			etimer_reset(&timer); // Reset time
			
			array_test();
			
		
		}
                           
PROCESS_END();
}
