// contiki
#include "contiki.h"

// rpl routing
#include "net/routing/routing.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"
#include "sys/log.h"

// other
#include <stdlib.h>
#include "../defs.h"

#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

#define WITH_SERVER_REPLY  1
#define UDP_CLIENT_PORT	8765
#define UDP_SERVER_PORT	5678

static struct simple_udp_connection udp_conn;
static uint32_t rx_count = 0;

PROCESS(udp_server_process, "UDP server");
AUTOSTART_PROCESSES(&udp_server_process);
/*---------------------------------------------------------------------------*/
static void
udp_rx_callback(struct simple_udp_connection *c,
        const uip_ipaddr_t *sender_addr, uint16_t sender_port,
        const uip_ipaddr_t *receiver_addr, uint16_t receiver_port,
        const uint8_t *data, uint16_t datalen)
{
  LOG_INFO("Received request %"PRIu32" from ", rx_count);
  LOG_INFO_6ADDR(sender_addr);
  LOG_INFO_("\n");
  
  Data *rec_data = (struct Data *) data;
  LOG_INFO("\nReceived data from node %s:\n\ttemperature = %s,\n\thumidity: %s,\n\tphoto: %s,\n\tsolar: %s\n", 
    rec_data->nodeID, 
    rec_data->temp, 
    rec_data->hum, 
    rec_data->photo, 
    rec_data->solar);
  rx_count++;
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_server_process, ev, data)
{
  PROCESS_BEGIN();

  /* Initialize DAG root */
  NETSTACK_ROUTING.root_start();

  /* Initialize UDP connection */
  simple_udp_register(&udp_conn, UDP_SERVER_PORT, NULL, UDP_CLIENT_PORT, udp_rx_callback);

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
