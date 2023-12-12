// contiki
#include "contiki.h"

// rpl routing
#include "uip.h"
#include "net/routing/routing.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"
#include "sys/log.h"

#define WITH_SERVER_REPLY  1
#define UDP_CLIENT_PORT 8765
#define UDP_SERVER_PORT 5678

#define LOG_MODULE "clientMote"
#define LOG_LEVEL LOG_LEVEL_INFO

static struct simple_udp_connection udp_conn;


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

}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_client_process, ev, data){
PROCESS_BEGIN();

  

  /* Initialize UDP connection */
  /* a udp connection requires the client- and server port, and what to respond with, udp_rx_callback */
  simple_udp_register(&udp_conn, UDP_CLIENT_PORT, NULL, UDP_SERVER_PORT, udp_rx_callback);

  
PROCESS_END();
}
/*---------------------------------------------------------------------------*/