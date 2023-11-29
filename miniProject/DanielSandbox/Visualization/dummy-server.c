

// This is not a RPL server. It simulates printing sensor data to the terminal every 1 second

// contiki
#include "contiki.h"

// rpl routing
#include "net/routing/routing.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"
#include "sys/log.h"

// other
#include <stdlib.h>
#include "defs.h"

#define LOG_MODULE "sinkMote"
#define LOG_LEVEL LOG_LEVEL_INFO


PROCESS(udp_server_process, "UDP server");
AUTOSTART_PROCESSES(&udp_server_process);
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_server_process, ev, data)
{
    static struct etimer timer;
// Define local variables (reset with every process call)
    PROCESS_BEGIN();
        etimer_set(&timer, CLOCK_SECOND); // Initiate timer to trigger event
        
        while(1) {
            // Temperature
            SENSORS_ACTIVATE(sht11_sensor); //Activate Sensor
            int val_temp = sht11_sensor.value(SHT11_SENSOR_TEMP);

            // Humidity
            int val_hum = sht11_sensor.value(SHT11_SENSOR_HUMIDITY);
            
            //Photo
            SENSORS_ACTIVATE(light_sensor); //Activate
            int val_photo = light_sensor.value(LIGHT_SENSOR_PHOTOSYNTHETIC);
            
            //Solar
            int val_solar = light_sensor.value(LIGHT_SENSOR_TOTAL_SOLAR);
            printf("%d %d %d %d \n",val_temp,val_hum,val_photo,val_solar);
            
            PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer)); // Wait until time is expired
            etimer_reset(&timer); // Reset timer
            SENSORS_DEACTIVATE(sht11_sensor); //Deactivate Temp Sensor
            SENSORS_DEACTIVATE(light_sensor); //Deactivaet Light Sensor
        }
    
    PROCESS_END();
}
/*---------------------------------------------------------------------------*/
