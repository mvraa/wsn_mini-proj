#include "contiki.h"
#include "net/routing/routing.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"
#include "defs.h"

#include "sys/log.h"
#define LOG_MODULE "sinkNode"
#define LOG_LEVEL LOG_LEVEL_INFO

static struct simple_udp_connection udp_conn;
//Package to send on every send interval
//static Data dataPackage [SEND_INTERVAL/MEASURE_INTERVAL];

void printf_float(float, int);


PROCESS(udp_server_process, "UDP server");
AUTOSTART_PROCESSES(&udp_server_process);
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
  LOG_INFO("Received request from ");
  LOG_INFO_6ADDR(sender_addr);
  LOG_INFO_("\n");
  Data* myData = (Data *)data;
  printf("Received (First data in package): \n");
  printf("\t temp: ");
  printf_float(myData[0].temp, 3);
  printf("\n\t hum: ");
  printf_float(myData[0].hum, 3);
  printf("\n\t photo: ");
  printf_float(myData[0].photo, 3);
  printf("\n\t solar: ");
  printf_float(myData[0].solar, 3);  

#if WITH_SERVER_REPLY
  /* send back the same string to the client as an echo reply */
  LOG_INFO("Sending response.\n");
  simple_udp_sendto(&udp_conn, data, datalen, sender_addr);
#endif /* WITH_SERVER_REPLY */
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_server_process, ev, data)
{
  PROCESS_BEGIN();

  /* Initialize DAG root */
  NETSTACK_ROUTING.root_start();

  /* Initialize UDP connection */
  simple_udp_register(&udp_conn, UDP_SERVER_PORT, NULL,
                      UDP_CLIENT_PORT, udp_rx_callback);

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
// Reused from assignment 1. Changed it to take floats as inputs. We probably don't need to work with doubles for this project
void printf_float(float x, int p) {
    // Check if x is negative
    if (x < 0) {
        printf("-");
        x = -x;
    }
    int i = (int)x; // integer part
    printf("%d", i); // print integer part

    // is p is positive
    if (p > 0) {
        printf("."); // decimal point
        float f = x - i; // the fractional part of x

        for (int j = 0; j < p; j++) {
            // multiply f by 10 and get the integer part
            f = f * 10;
            i = (int)f;

            printf("%d", i); // print integer part
            f = f - i; // fractional part
        }
    }
}