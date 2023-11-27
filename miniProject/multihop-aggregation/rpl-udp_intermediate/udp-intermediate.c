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

#define LOG_MODULE "IntermediateMote"
#define LOG_LEVEL LOG_LEVEL_INFO


static struct simple_udp_connection udp_conn_recv_clients;
static struct simple_udp_connection udp_conn_relay_sink;

uip_ipaddr_t sink_ipaddr;
static uint32_t rx_count = 0;
static uint32_t PackagesReceived = 0;
static Data dataPackages[DATA_PACKAGES_INTERMEDIATE][MEASURES]; // Receive Packages like this from the motes

void bufferData(Data*);

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
  
  Data *d = (struct Data *) data;

  // Store the received data to dataPackages
  bufferData(d);
  
  // If dataPackages is full calculate the aggregatedData and forward it to the sink
  if (PackagesReceived == DATA_PACKAGES_INTERMEDIATE - 1){
    LOG_INFO("Relaying to sink address: ");
    LOG_INFO_6ADDR(&sink_ipaddr);
    LOG_INFO_("\n");

    Data aggData = getAverage(dataPackages);
    PackagesReceived = 0;
    LOG_INFO("Test for aggData: %d %d %d %d \n", (int)aggData.temp, (int)aggData.hum, (int)aggData.photo, (int)aggData.solar);
    simple_udp_sendto(&udp_conn_relay_sink, &aggData, sizeof(aggData), &sink_ipaddr);
    
  }
  else{
    PackagesReceived ++;
  }
  rx_count++;
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

  // sink address: fd00::212::7400::1cdf::a959
  uip_ip6addr_u8(&sink_ipaddr, 0xfd, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x12, 0x74, 0x00, 0x1c, 0xdf, 0xa9, 0x59);
  // cooja sink:
  // uip_ip6addr_u8(&sink_ipaddr, 0xfd, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x12, 0x74, 0x03, 0x00, 0x03, 0x03, 0x03);

  /* Initialize UDP connection */
  simple_udp_register(&udp_conn_recv_clients, UDP_SERVER_PORT, NULL, UDP_CLIENT_PORT, udp_rx_callback);
  simple_udp_register(&udp_conn_relay_sink, UDP_CLIENT_PORT, NULL, UDP_SERVER_PORT, udp_rx_callback2);

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/

void bufferData(Data* data){
  for (int j = 0; j < MEASURES; ++j) {
    dataPackages[PackagesReceived][j] = data[j];
  }
}