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
static Data dataPackages [DATA_PACKAGES_INTERMEDIATE][MEASURES]; // Receive Packages like this from the motes
static AggregatedData* aggData;

void bufferData(Data*);
void average(); // Calculate the average from all values in dataPackages and store it in aggData

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
  

  // Store the received data to dataPackages
  bufferData((Data *)data);
  
  
  // If dataPackages is full calculate the aggregatedData and forward it to the sink
  if (PackagesReceived == DATA_PACKAGES_INTERMEDIATE - 1){
    LOG_INFO("Relaying to sink address: ");
    LOG_INFO_6ADDR(&sink_ipaddr);
    LOG_INFO_("\n");
    average();
    PackagesReceived = 0;
    LOG_INFO("Test for aggData: %d", (int)aggData->avgTemp);
    simple_udp_sendto(&udp_conn_relay_sink, aggData, sizeof(aggData), &sink_ipaddr);
    
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
  // TODO: Maybe move this to defs.h?
  uip_ip6addr_u8(&sink_ipaddr, 0xfd, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x12, 0x74, 0x03, 0x00, 0x03, 0x03, 0x03);

  /* Initialize UDP connection */
  simple_udp_register(&udp_conn_recv_clients, UDP_SERVER_PORT, NULL, UDP_CLIENT_PORT, udp_rx_callback);
  simple_udp_register(&udp_conn_relay_sink, UDP_CLIENT_PORT, NULL, UDP_SERVER_PORT, udp_rx_callback2);

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/

void average (){
  float sumTemp = 0;
  float sumHum = 0;
  float sumPhoto = 0;
  float sumSolar = 0;

  for (int i = 0; i <DATA_PACKAGES_INTERMEDIATE; i++){
    for (int j = 0 ; j< MEASURES; j++){
        sumTemp += dataPackages[i][j].temp;
        sumHum += dataPackages[i][j].hum;
        sumPhoto += dataPackages[i][j].photo;
        sumSolar += dataPackages[i][j].solar;
    }
  }
  aggData->avgTemp = sumTemp /(DATA_PACKAGES_INTERMEDIATE*MEASURES);
  aggData->avgHum = sumHum /(DATA_PACKAGES_INTERMEDIATE*MEASURES);
  aggData->avgPhoto = sumPhoto /(DATA_PACKAGES_INTERMEDIATE*MEASURES);
  aggData->avgSolar = sumSolar /(DATA_PACKAGES_INTERMEDIATE*MEASURES);
}

void bufferData(Data* data){
  for (int j = 0; j < MEASURES; ++j) {
    dataPackages[PackagesReceived][j] = data[j];
  }
}