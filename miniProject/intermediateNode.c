#include "contiki.h"
#include "net/routing/routing.h"
#include "random.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"
#include <stdint.h>
#include <inttypes.h>

#include "sht11-sensor.h" // temperature and humidity
#include "dev/light-sensor.h" // light

#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

#define WITH_SERVER_REPLY  1
//Client and server on the same mote. Local instances prefixed with LOCAL
#define LOCAL_UDP_CLIENT_PORT	8780
#define LOCAL_UDP_SERVER_PORT	5678

#define REMOTE_UDP_SERVER_PORT 8770
#define REMOTE_UDP_CLIENT_PORT	8765

#define SEND_INTERVAL		  (10 * CLOCK_SECOND)

static struct simple_udp_connection udp_conn;
static uint32_t rx_count = 0;

/*---------------------------------------------------------------------------*/
PROCESS(udp_client_process, "UDP client");
PROCESS(udp_server_process, "UDP server");
AUTOSTART_PROCESSES(&udp_client_process,&udp_server_process);
/*---------------------------------------------------------------------------*/
static void
udp_rx_callback_client(struct simple_udp_connection *c,
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

static void
udp_rx_callback_server(struct simple_udp_connection *c,
         const uip_ipaddr_t *sender_addr,
         uint16_t sender_port,
         const uip_ipaddr_t *receiver_addr,
         uint16_t receiver_port,
         const uint8_t *data,
         uint16_t datalen)
{
  LOG_INFO("Received request '%.*s' from ", datalen, (char *) data);
  LOG_INFO_6ADDR(sender_addr);
  LOG_INFO_("\n");
#if WITH_SERVER_REPLY
  /* send back the same string to the client as an echo reply */
  LOG_INFO("Sending response.\n");
  simple_udp_sendto(&udp_conn, data, datalen, sender_addr);
#endif /* WITH_SERVER_REPLY */
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_client_process, ev, data)
{
  static struct etimer periodic_timer;
  static char str[64]; // increased from 32 to 64 to adjust for extra size of sensor data
  uip_ipaddr_t dest_ipaddr;
  static uint32_t tx_count;
  static uint32_t missed_tx_count;

  PROCESS_BEGIN();
  SENSORS_ACTIVATE(sht11_sensor); // activate temp/hum sensor
  SENSORS_ACTIVATE(light_sensor); // activate light sensor

  /* Initialize UDP connection */
  simple_udp_register(&udp_conn, LOCAL_UDP_CLIENT_PORT, NULL,
                      REMOTE_UDP_SERVER_PORT, udp_rx_callback_client);

  etimer_set(&periodic_timer, random_rand() % SEND_INTERVAL);
  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));

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
      snprintf(str, sizeof(str), "Packet: %" PRIu32 " temp: %d hum: %d photo: %d solar: %d ", 
            tx_count, // packet n
            sht11_sensor.value(SHT11_SENSOR_TEMP), // temperature
            sht11_sensor.value(SHT11_SENSOR_HUMIDITY), // humidity
            light_sensor.value(LIGHT_SENSOR_PHOTOSYNTHETIC), // photo light
            light_sensor.value(LIGHT_SENSOR_TOTAL_SOLAR) // solar light
      );
      simple_udp_sendto(&udp_conn, str, strlen(str), &dest_ipaddr);
      tx_count++;
    } else {
      LOG_INFO("Not reachable yet\n");
      if(tx_count > 0) {
        missed_tx_count++;
      }
    }

    SENSORS_DEACTIVATE(sht11_sensor); // deactivate temp/hum sensor
    SENSORS_DEACTIVATE(light_sensor); // deactivate light sensor

    /* Add some jitter */
    etimer_set(&periodic_timer, SEND_INTERVAL
      - CLOCK_SECOND + (random_rand() % (2 * CLOCK_SECOND)));
  }



  PROCESS_END();
}
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_server_process, ev, data)
{
  PROCESS_BEGIN();

  /* Initialize DAG root */
  NETSTACK_ROUTING.root_start();

  /* Initialize UDP connection */
  simple_udp_register(&udp_conn, LOCAL_UDP_SERVER_PORT, NULL,
                      REMOTE_UDP_CLIENT_PORT, udp_rx_callback_server);

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
