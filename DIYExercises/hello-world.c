#include "contiki.h"
#include <stdio.h> /* For printf() */
/*-----------------------------------------------------------*/
PROCESS(hello_world_process, "Hello world process");
AUTOSTART_PROCESSES(&hello_world_process);
/*-----------------------------------------------------------*/
// Define global variables
PROCESS_THREAD(hello_world_process, ev, data)
{
  // Define local variables (reset with every process call)
  PROCESS_BEGIN();
 
    // Define behaviour
    printf("Hello World");
  
  PROCESS_END();
}
