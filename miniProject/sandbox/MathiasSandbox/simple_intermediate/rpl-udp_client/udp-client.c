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

// sensors
#include "sht11-sensor.h" // temperature and humidity
#include "dev/light-sensor.h" // light

#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

#define WITH_SERVER_REPLY  1
#define UDP_CLIENT_PORT 8765
#define UDP_SERVER_PORT 5678

#define SEND_INTERVAL     (5 * CLOCK_SECOND)

static struct simple_udp_connection udp_conn;
static uint32_t rx_count = 0;

void packageData(Data *d, int nodeID);

/*---------------------------------------------------------------------------*/
PROCESS(udp_client_process, "UDP client");
AUTOSTART_PROCESSES(&udp_client_process);
/*---------------------------------------------------------------------------*/
static void
udp_rx_callback(
        struct simple_udp_connection *c,
        const uip_ipaddr_t *sender_addr, uint16_t sender_port, 
        const uip_ipaddr_t *receiver_addr, uint16_t receiver_port, 
        const uint8_t *data, uint16_t datalen)
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
  // intermediate address: fd00::212::7400::1cdf::a959
  uip_ip6addr_u8(&dest_ipaddr, 0xfd, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x12, 0x74, 0x00, 0x14, 0x6e, 0xc0, 0x3e);

  static int nodeID = 3;
  static uint32_t tx_count;

  PROCESS_BEGIN();

  SENSORS_ACTIVATE(sht11_sensor); // activate temp/hum sensor
  SENSORS_ACTIVATE(light_sensor); // activate light sensor

  /* Initialize UDP connection */
  /* a udp connection requires the client- and server port, and what to respond with, udp_rx_callback */
  simple_udp_register(&udp_conn, UDP_CLIENT_PORT, NULL, UDP_SERVER_PORT, udp_rx_callback);

  // init periodic interval.
  etimer_set(&periodic_timer, random_rand() % SEND_INTERVAL);

  while(1) {
    // wait on timer
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));

    /* Send to DAG root */
    LOG_INFO("Sending sensor data %"PRIu32" to ", tx_count);
    LOG_INFO_6ADDR(&dest_ipaddr);

    // construct packet
    Data d;
    packageData(&d, nodeID);

    LOG_INFO("\nCollected data:\n\ttemperature = %s,\n\thumidity: %s,\n\tphoto: %s,\n\tsolar: %s\n",
    d.temp, 
    d.hum, 
    d.photo, 
    d.solar);
    LOG_INFO_("\n\n");

    simple_udp_sendto(&udp_conn, &d, sizeof(d), &dest_ipaddr);
    tx_count++;

    /* Add some jitter */
    etimer_set(&periodic_timer, SEND_INTERVAL - CLOCK_SECOND + (random_rand() % (2 * CLOCK_SECOND)));
  }

  SENSORS_DEACTIVATE(sht11_sensor); // deactivate temp/hum sensor
  SENSORS_DEACTIVATE(light_sensor); // deactivate light sensor

  PROCESS_END();
}

void packageData(Data *d, int nodeID){
  snprintf(d->nodeID, sizeof(d->nodeID), "%d", nodeID);
  snprintf(d->temp, sizeof(d->temp), "%d", sht11_sensor.value(SHT11_SENSOR_TEMP));
  snprintf(d->hum, sizeof(d->hum), "%d", sht11_sensor.value(SHT11_SENSOR_HUMIDITY));
  snprintf(d->photo, sizeof(d->photo), "%d", light_sensor.value(LIGHT_SENSOR_PHOTOSYNTHETIC));
  snprintf(d->solar, sizeof(d->solar), "%d", light_sensor.value(LIGHT_SENSOR_TOTAL_SOLAR));
}

/*---------------------------------------------------------------------------*/
