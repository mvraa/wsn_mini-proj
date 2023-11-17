#include "contiki.h"
#include "net/routing/routing.h"
#include "random.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"
#include <stdint.h>
#include "defs.h"

#include "sht11-sensor.h" // temperature and humidity
#include "dev/light-sensor.h" // light

#include "sys/log.h"
#define LOG_MODULE "clientNode"
#define LOG_LEVEL LOG_LEVEL_INFO



static struct simple_udp_connection udp_conn;
static uint32_t rx_count = 0;
//Package to send on every send interval
static Data dataPackage [SEND_INTERVAL/MEASURE_INTERVAL];

void measureData(uint32_t measure_count);


/*---------------------------------------------------------------------------*/
PROCESS(udp_client_process, "UDP client");
AUTOSTART_PROCESSES(&udp_client_process);
/*---------------------------------------------------------------------------*/
static void
udp_rx_callback(struct simple_udp_connection *c,
         const uip_ipaddr_t *sender_addr,
         uint16_t sender_port,
         const uip_ipaddr_t *receiver_addr,
         uint16_t receiver_port,
         const uint8_t *data,
         uint16_t datalen)
{

  LOG_INFO("Received response '%.*s' from ", datalen, (char *) data);
  LOG_INFO_6ADDR(sender_addr);
#if LLSEC802154_CONF_ENABLED
  LOG_INFO_(" LLSEC LV:%d", uipbuf_get_attr(UIPBUF_ATTR_LLSEC_LEVEL));
#endif
  LOG_INFO_("\n");
  rx_count++;
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_client_process, ev, data)
{
  static struct etimer periodic_timer;
  uip_ipaddr_t dest_ipaddr;
  static uint32_t tx_count;
  static uint32_t missed_tx_count;
  static uint32_t measure_count = 0;

  PROCESS_BEGIN();
  SENSORS_ACTIVATE(sht11_sensor); // activate temp/hum sensor
  SENSORS_ACTIVATE(light_sensor); // activate light sensor
	
  /* Initialize UDP connection */
  simple_udp_register(&udp_conn, UDP_CLIENT_PORT, NULL,
                      UDP_SERVER_PORT, udp_rx_callback);

  etimer_set(&periodic_timer, random_rand() % MEASURE_INTERVAL);
  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
	
	
	/*Record the data*/
	measureData(measure_count);
	LOG_INFO("Data measures written \n");
	measure_count ++;
	if (measure_count == 10){
		/*Reset counter*/
		measure_count=0;
	
	
		/* Send the data */
		if(NETSTACK_ROUTING.node_is_reachable() &&
			NETSTACK_ROUTING.get_root_ipaddr(&dest_ipaddr)) {

		  /* Print statistics every 10th TX */
		  if(tx_count % 10 == 0) {
			LOG_INFO("Tx/Rx/MissedTx: %" PRIu32 "/%" PRIu32 "/%" PRIu32 "\n",
					 tx_count, rx_count, missed_tx_count);
		  }

		  /* Send to DAG root */
		  LOG_INFO("Sending request %"PRIu32" to ", tx_count);
		  LOG_INFO_6ADDR(&dest_ipaddr);
		  LOG_INFO_("\n");
		  simple_udp_sendto(&udp_conn, dataPackage, sizeof(dataPackage), &dest_ipaddr);
		  tx_count++;
		} else {
		  LOG_INFO("Not reachable yet\n");
		  if(tx_count > 0) {
			missed_tx_count++;
		  }
		}
	}
    /* Add some jitter */
    etimer_set(&periodic_timer, MEASURE_INTERVAL);
  }
  SENSORS_DEACTIVATE(sht11_sensor); // deactivate temp/hum sensor
  SENSORS_DEACTIVATE(light_sensor); // deactivate light sensor
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/

void measureData(uint32_t measure_count){
	dataPackage[measure_count].temp = sht11_sensor.value(SHT11_SENSOR_TEMP); // temperature
	dataPackage[measure_count].hum = sht11_sensor.value(SHT11_SENSOR_HUMIDITY); // humidity       
	dataPackage[measure_count].photo = light_sensor.value(LIGHT_SENSOR_PHOTOSYNTHETIC); // photo light      
	dataPackage[measure_count].solar = light_sensor.value(LIGHT_SENSOR_TOTAL_SOLAR); // solar light  
}