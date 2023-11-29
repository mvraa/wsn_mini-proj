// contiki
#include "contiki.h"

// rpl routing
#include "uip.h"
#include "net/routing/routing.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"
#include "sys/log.h"

// other
#include "random.h"
#include <stdint.h>
#include <inttypes.h>
#include "../defs.h"

#define LOG_MODULE "clientMote"
#define LOG_LEVEL LOG_LEVEL_INFO

static struct simple_udp_connection udp_conn;
static uint32_t rx_count = 0;
//Package to send on every send interval
static Data dataPackage[MEASURES];

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
PROCESS_THREAD(udp_client_process, ev, data){
  static struct etimer periodic_timer;
  
  uip_ipaddr_t dest_ipaddr;

  // intermediate address: fd00::212::7400::146e::c03e
  uip_ip6addr_u8(&dest_ipaddr, 0xfd, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x12, 0x74, 0x00, 0x14, 0x6e, 0xc0, 0x3e);
  // cooja intermediate:
  // uip_ip6addr_u8(&dest_ipaddr, 0xfd, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x12, 0x74, 0x02, 0x00, 0x02, 0x02, 0x02);

  static uint32_t tx_count;

  static uint32_t measure_count = 0;

  PROCESS_BEGIN();

  SENSORS_ACTIVATE(sht11_sensor); // activate temp/hum sensor
  SENSORS_ACTIVATE(light_sensor); // activate light sensor

  /* Initialize UDP connection */
  /* a udp connection requires the client- and server port, and what to respond with, udp_rx_callback */
  simple_udp_register(&udp_conn, UDP_CLIENT_PORT, NULL, UDP_SERVER_PORT, udp_rx_callback);

  // init periodic interval.
  etimer_set(&periodic_timer, random_rand() % MEASURE_INTERVAL);

  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
  
    /*Record the data*/
    dataPackage[measure_count] = getSensorData();

    #if PRINT_AUX
      LOG_INFO("Data measure collected: ");
    #endif
    printRawData(&dataPackage[measure_count]);
    //printEnergyMeasurements();
    LOG_INFO_("\n");

    measure_count ++;
    if (measure_count == MEASURES){
      measure_count=0; // reset counter
      
      #if PRINT_AUX
        /* Send to DAG root */
        LOG_INFO("Sending collected measurements %"PRIu32" to ", tx_count);
        LOG_INFO_6ADDR(&dest_ipaddr);
        LOG_INFO_("\n");
      #endif

      printf("\n");

      simple_udp_sendto(&udp_conn, dataPackage, sizeof(dataPackage), &dest_ipaddr);
      tx_count++;
    } 
    
  /* Reset Timer */
  etimer_set(&periodic_timer, MEASURE_INTERVAL);
  }

SENSORS_DEACTIVATE(sht11_sensor); // deactivate temp/hum sensor
SENSORS_DEACTIVATE(light_sensor); // deactivate light sensor

PROCESS_END();
}
/*---------------------------------------------------------------------------*/