// contiki
#include "contiki.h"

// rpl routing
#include "net/routing/routing.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"
#include "sys/log.h"


#define LOG_MODULE "IntermediateMote"
#define LOG_LEVEL LOG_LEVEL_INFO

#define WITH_SERVER_REPLY  1
#define UDP_CLIENT_PORT 8765
#define UDP_SERVER_PORT 5678


static struct simple_udp_connection udp_conn_recv_clients;
static struct simple_udp_connection udp_conn_relay_sink;

uip_ipaddr_t sink_ipaddr;
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

  
  
}

static void
udp_rx_callback2(struct simple_udp_connection *c,
        const uip_ipaddr_t *sender_addr, uint16_t sender_port,
        const uip_ipaddr_t *receiver_addr, uint16_t receiver_port,
        const uint8_t *data, uint16_t datalen)
{

}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_server_process, ev, data)
{
  PROCESS_BEGIN();


  /* Initialize UDP connection */
  simple_udp_register (&udp_conn_recv_clients, UDP_SERVER_PORT, NULL, UDP_CLIENT_PORT, udp_rx_callback);
  simple_udp_register(&udp_conn_relay_sink, UDP_CLIENT_PORT, NULL, UDP_SERVER_PORT, udp_rx_callback2);

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/

