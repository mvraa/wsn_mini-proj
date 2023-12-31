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


static struct simple_udp_connection udp_conn;
static uint32_t rx_count = 0;
static Data AggDataPackage[AGG_DATA_PACKAGE_LENGTH]; //This stores the aggrated data packages 
static uint32_t AggData_count = 0;

PROCESS(udp_server_process, "UDP server");
AUTOSTART_PROCESSES(&udp_server_process);
/*---------------------------------------------------------------------------*/
static void
udp_rx_callback(struct simple_udp_connection *c,
         const uip_ipaddr_t *sender_addr, uint16_t sender_port,
         const uip_ipaddr_t *receiver_addr, uint16_t receiver_port,
         const uint8_t *data, uint16_t datalen)
{
    #if PRINT_AUX
        LOG_INFO("Received request %"PRIu32" from ", rx_count);
        LOG_INFO_6ADDR(sender_addr);
        LOG_INFO_("\n");
        LOG_INFO("Received aggregated data: ");
    #endif

    Data agg_data;
    memcpy(&agg_data, data, sizeof(agg_data));
    printRawData(&agg_data);
    printf("\n");

    rx_count++;
    AggDataPackage[AggData_count] = agg_data;
  
    if(AggData_count == AGG_DATA_PACKAGE_LENGTH - 1){
        //reset counter to override data
        AggData_count = 0;
        // Todo: at this place we could write the AggDataPackage to a file or send it over the serial port
    }
    else AggData_count++;
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
