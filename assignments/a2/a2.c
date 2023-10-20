#include "contiki.h"
#include "CC2420.h"
#include <stdio.h>

#define CHANNEL_START 11
#define CHANNEL_END 26
#define CHANNEL_INTERVAL 1

#define PASSES 50

PROCESS(channel_sensing_process, "channel_sensing_process");

AUTOSTART_PROCESSES(&channel_sensing_process);

PROCESS_THREAD(channel_sensing_process, ev, data)
{
  static struct etimer timer;

  PROCESS_BEGIN();

   /* Setup a periodic timer that expires after 3 seconds. */
  etimer_set(&timer, CLOCK_SECOND * 3);

  cc2420_init();
  while (1)
  {
    // measurements storage
    int measurements[CHANNEL_END - CHANNEL_START];
    int passes_sum;
    int channel;
    int i;
    int measure;
    // Turn on radio
    cc2420_on();
    for (channel = CHANNEL_START; channel <= CHANNEL_END; channel += CHANNEL_INTERVAL)
    {
      // select channel
      cc2420_set_channel(channel);
      passes_sum = 0;
      for (i = 0; i < PASSES; i++)
      {
        // measure in dbm using formula (p.48 of cc2420 datasheet):
        //  P = RSSI_VAL + RSSI_OFFSET
        //  [dBm]RSSI_OFFSET is approximately –45.
        measure = cc2420_rssi() - 45;
        // sum measurements
        passes_sum += measure;
      }
      // store averages
      measurements[channel - 11] = passes_sum / PASSES;
    }
    // turn off radio
    cc2420_off();

    // find the best channel aka find lowest number
    int lowest_pass = measurements[0];
    int lowest_channel = 0;
    for (i = 0; i < (CHANNEL_END - CHANNEL_START); i++)
    {
      if (measurements[i] < lowest_pass)
      {
        lowest_pass = measurements[i];
        lowest_channel = i + 11;
      }
      // print the averages and their associated channels
      printf("Avg. measuments = %d of channel = %d \n", measurements[i], i + 11);
    }
    printf("Lowest avg. = %d at channel = %d \n", lowest_pass, lowest_channel);
    cc2420_set_channel(lowest_channel);

    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));
    etimer_reset(&timer);
  }

  PROCESS_END();
}



// vagrant@ubuntu-bionic:~/contiki-ng/examples/a2$ sudo make TARGET=sky MOTES=/dev/ttyUSB0 login
// rlwrap ../../tools/serial-io/serialdump -b115200 /dev/ttyUSB0
// connecting to /dev/ttyUSB0 [OK]
// �Cc�RoCcCR^CR�C{C�Co�Ck�V&^�F�^&�S&^&�S�F�^&^�F�^�F�^&�g��ێN�F�^&^�F�F�^&^&^���F�F�^&^�^��S�Nk�^&^�F�^&^&^&^�F�F�^&^&^&^��g�Nk�C�^&^&^&^�^���F�F�F�F�F�F��g�Nk�^�^&^&^&^&^&^&^&^���^�F�F��g��S&>k�F�F�F�F�F�F�F�^;^;^&^&^&^��g��C�Nk�F�^�F�F�F�F����&^�Avg. measuments = -136 of channel = 11
// Avg. measuments = -138 of channel = 12
// Avg. measuments = -135 of channel = 13
// Avg. measuments = -138 of channel = 14
// Avg. measuments = -136 of channel = 15
// Avg. measuments = -139 of channel = 16
// Avg. measuments = -137 of channel = 17
// Avg. measuments = -136 of channel = 18
// Avg. measuments = -134 of channel = 19
// Avg. measuments = -138 of channel = 20
// Avg. measuments = -132 of channel = 21
// Avg. measuments = -136 of channel = 22
// Avg. measuments = -134 of channel = 23
// Avg. measuments = -138 of channel = 24
// Avg. measuments = -136 of channel = 25
// Lowest avg. = -139 at channel = 16
// Avg. measuments = -130 of channel = 11
// Avg. measuments = -124 of channel = 12
// Avg. measuments = -135 of channel = 13
// Avg. measuments = -137 of channel = 14
// Avg. measuments = -137 of channel = 15
// Avg. measuments = -138 of channel = 16
// Avg. measuments = -136 of channel = 17
// Avg. measuments = -136 of channel = 18
// Avg. measuments = -132 of channel = 19
// Avg. measuments = -138 of channel = 20
// Avg. measuments = -133 of channel = 21
// Avg. measuments = -136 of channel = 22
// Avg. measuments = -135 of channel = 23
// Avg. measuments = -138 of channel = 24
// Avg. measuments = -136 of channel = 25
// Lowest avg. = -138 at channel = 16
// Avg. measuments = -136 of channel = 11
// Avg. measuments = -119 of channel = 12
// Avg. measuments = -135 of channel = 13
// Avg. measuments = -137 of channel = 14
// Avg. measuments = -137 of channel = 15
// Avg. measuments = -138 of channel = 16
// Avg. measuments = -137 of channel = 17
// Avg. measuments = -132 of channel = 18
// Avg. measuments = -133 of channel = 19
// Avg. measuments = -138 of channel = 20
// Avg. measuments = -133 of channel = 21
// Avg. measuments = -136 of channel = 22
// Avg. measuments = -127 of channel = 23
// Avg. measuments = -138 of channel = 24
// Avg. measuments = -136 of channel = 25
// Lowest avg. = -138 at channel = 16
// Avg. measuments = -136 of channel = 11
// Avg. measuments = -138 of channel = 12
// Avg. measuments = -135 of channel = 13
// Avg. measuments = -135 of channel = 14
// Avg. measuments = -136 of channel = 15
// Avg. measuments = -139 of channel = 16
// Avg. measuments = -130 of channel = 17
// Avg. measuments = -136 of channel = 18
// Avg. measuments = -134 of channel = 19
// Avg. measuments = -138 of channel = 20
// Avg. measuments = -132 of channel = 21
// Avg. measuments = -136 of channel = 22
// Avg. measuments = -132 of channel = 23
// Avg. measuments = -138 of channel = 24
// Avg. measuments = -136 of channel = 25
// Lowest avg. = -139 at channel = 16
// Avg. measuments = -136 of channel = 11
// Avg. measuments = -137 of channel = 12
// Avg. measuments = -135 of channel = 13
// Avg. measuments = -122 of channel = 14
// Avg. measuments = -134 of channel = 15
// Avg. measuments = -135 of channel = 16
// Avg. measuments = -136 of channel = 17
// Avg. measuments = -136 of channel = 18
// Avg. measuments = -134 of channel = 19
// Avg. measuments = -137 of channel = 20
// Avg. measuments = -133 of channel = 21
// Avg. measuments = -135 of channel = 22
// Avg. measuments = -135 of channel = 23
// Avg. measuments = -138 of channel = 24
// Avg. measuments = -136 of channel = 25
// Lowest avg. = -138 at channel = 24
// Avg. measuments = -136 of channel = 11
// Avg. measuments = -138 of channel = 12
// Avg. measuments = -135 of channel = 13
// Avg. measuments = -137 of channel = 14
// Avg. measuments = -137 of channel = 15
// Avg. measuments = -138 of channel = 16
// Avg. measuments = -137 of channel = 17
// Avg. measuments = -136 of channel = 18
// Avg. measuments = -134 of channel = 19
// Avg. measuments = -138 of channel = 20
// Avg. measuments = -123 of channel = 21
// Avg. measuments = -135 of channel = 22
// Avg. measuments = -134 of channel = 23
// Avg. measuments = -138 of channel = 24
// Avg. measuments = -136 of channel = 25
// Lowest avg. = -138 at channel = 12
// Avg. measuments = -134 of channel = 11
// Avg. measuments = -138 of channel = 12
// Avg. measuments = -135 of channel = 13
// Avg. measuments = -137 of channel = 14
// Avg. measuments = -137 of channel = 15
// Avg. measuments = -138 of channel = 16
// Avg. measuments = -137 of channel = 17
// Avg. measuments = -136 of channel = 18
// Avg. measuments = -134 of channel = 19
// Avg. measuments = -138 of channel = 20
// Avg. measuments = -133 of channel = 21
// Avg. measuments = -135 of channel = 22
// Avg. measuments = -135 of channel = 23
// Avg. measuments = -138 of channel = 24
// Avg. measuments = -134 of channel = 25
// Lowest avg. = -138 at channel = 12
// Avg. measuments = -136 of channel = 11
// Avg. measuments = -138 of channel = 12
// Avg. measuments = -135 of channel = 13
// Avg. measuments = -137 of channel = 14
// Avg. measuments = -137 of channel = 15
// Avg. measuments = -138 of channel = 16
// Avg. measuments = -136 of channel = 17
// Avg. measuments = -136 of channel = 18
// Avg. measuments = -134 of channel = 19
// Avg. measuments = -138 of channel = 20
// Avg. measuments = -133 of channel = 21
// Avg. measuments = -135 of channel = 22
// Avg. measuments = -135 of channel = 23
// Avg. measuments = -138 of channel = 24
// Avg. measuments = -136 of channel = 25
// Lowest avg. = -138 at channel = 12
// Avg. measuments = -136 of channel = 11
// Avg. measuments = -133 of channel = 12
// Avg. measuments = -135 of channel = 13
// Avg. measuments = -137 of channel = 14
// Avg. measuments = -128 of channel = 15
// Avg. measuments = -134 of channel = 16
// Avg. measuments = -136 of channel = 17
// Avg. measuments = -136 of channel = 18
// Avg. measuments = -134 of channel = 19
// Avg. measuments = -135 of channel = 20
// Avg. measuments = -133 of channel = 21
// Avg. measuments = -136 of channel = 22
// Avg. measuments = -130 of channel = 23
// Avg. measuments = -131 of channel = 24
// Avg. measuments = -136 of channel = 25
// Lowest avg. = -137 at channel = 14
// Avg. measuments = -134 of channel = 11
// Avg. measuments = -138 of channel = 12
// Avg. measuments = -133 of channel = 13
// Avg. measuments = -137 of channel = 14
// Avg. measuments = -136 of channel = 15
// Avg. measuments = -138 of channel = 16
// Avg. measuments = -134 of channel = 17
// Avg. measuments = -134 of channel = 18
// Avg. measuments = -134 of channel = 19
// Avg. measuments = -138 of channel = 20
// Avg. measuments = -133 of channel = 21
// Avg. measuments = -135 of channel = 22
// Avg. measuments = -134 of channel = 23
// Avg. measuments = -136 of channel = 24
// Avg. measuments = -136 of channel = 25
// Lowest avg. = -138 at channel = 12
// Avg. measuments = -136 of channel = 11
// Avg. measuments = -138 of channel = 12
// Avg. measuments = -132 of channel = 13
// Avg. measuments = -138 of channel = 14
// Avg. measuments = -136 of channel = 15
// Avg. measuments = -138 of channel = 16
// Avg. measuments = -137 of channel = 17
// Avg. measuments = -136 of channel = 18
// Avg. measuments = -134 of channel = 19
// Avg. measuments = -137 of channel = 20
// Avg. measuments = -131 of channel = 21
// Avg. measuments = -135 of channel = 22
// Avg. measuments = -134 of channel = 23
// Avg. measuments = -138 of channel = 24
// Avg. measuments = -136 of channel = 25
// Lowest avg. = -138 at channel = 12
// Avg. measuments = -136 of channel = 11
// Avg. measuments = -127 of channel = 12
// Avg. measuments = -127 of channel = 13
// Avg. measuments = -137 of channel = 14
// Avg. measuments = -137 of channel = 15
// Avg. measuments = -138 of channel = 16
// Avg. measuments = -136 of channel = 17
// Avg. measuments = -136 of channel = 18
// Avg. measuments = -134 of channel = 19
// Avg. measuments = -138 of channel = 20
// Avg. measuments = -133 of channel = 21
// Avg. measuments = -135 of channel = 22
// Avg. measuments = -135 of channel = 23
// Avg. measuments = -138 of channel = 24
// Avg. measuments = -136 of channel = 25
// Lowest avg. = -138 at channel = 16
// Avg. measuments = -135 of channel = 11
// Avg. measuments = -138 of channel = 12
// Avg. measuments = -120 of channel = 13
// Avg. measuments = -137 of channel = 14
// Avg. measuments = -137 of channel = 15
// Avg. measuments = -139 of channel = 16
// Avg. measuments = -136 of channel = 17
// Avg. measuments = -135 of channel = 18
// Avg. measuments = -134 of channel = 19
// Avg. measuments = -138 of channel = 20
// Avg. measuments = -133 of channel = 21
// Avg. measuments = -135 of channel = 22
// Avg. measuments = -131 of channel = 23
// Avg. measuments = -138 of channel = 24
// Avg. measuments = -136 of channel = 25
// Lowest avg. = -139 at channel = 16
// Avg. measuments = -136 of channel = 11
// Avg. measuments = -138 of channel = 12
// Avg. measuments = -135 of channel = 13
// Avg. measuments = -137 of channel = 14
// Avg. measuments = -134 of channel = 15
// Avg. measuments = -137 of channel = 16
// Avg. measuments = -126 of channel = 17
// Avg. measuments = -129 of channel = 18
// Avg. measuments = -134 of channel = 19
// Avg. measuments = -138 of channel = 20
// Avg. measuments = -133 of channel = 21
// Avg. measuments = -135 of channel = 22
// Avg. measuments = -135 of channel = 23
// Avg. measuments = -138 of channel = 24
// Avg. measuments = -136 of channel = 25
// Lowest avg. = -138 at channel = 12
// Avg. measuments = -136 of channel = 11
// Avg. measuments = -138 of channel = 12
// Avg. measuments = -135 of channel = 13
// Avg. measuments = -137 of channel = 14
// Avg. measuments = -136 of channel = 15
// Avg. measuments = -138 of channel = 16
// Avg. measuments = -136 of channel = 17
// Avg. measuments = -136 of channel = 18
// Avg. measuments = -134 of channel = 19
// Avg. measuments = -138 of channel = 20
// Avg. measuments = -133 of channel = 21
// Avg. measuments = -136 of channel = 22
// Avg. measuments = -135 of channel = 23
// Avg. measuments = -138 of channel = 24
// Avg. measuments = -136 of channel = 25
// Lowest avg. = -138 at channel = 12
// Avg. measuments = -136 of channel = 11
// Avg. measuments = -138 of channel = 12
// Avg. measuments = -130 of channel = 13
// Avg. measuments = -136 of channel = 14
// Avg. measuments = -137 of channel = 15
// Avg. measuments = -138 of channel = 16
// Avg. measuments = -137 of channel = 17
// Avg. measuments = -136 of channel = 18
// Avg. measuments = -134 of channel = 19
// Avg. measuments = -138 of channel = 20
// Avg. measuments = -133 of channel = 21
// Avg. measuments = -119 of channel = 22
// Avg. measuments = -134 of channel = 23
// Avg. measuments = -138 of channel = 24
// Avg. measuments = -136 of channel = 25
// Lowest avg. = -138 at channel = 12
// Avg. measuments = -136 of channel = 11
// Avg. measuments = -134 of channel = 12
// Avg. measuments = -134 of channel = 13
// Avg. measuments = -135 of channel = 14
// Avg. measuments = -136 of channel = 15
// Avg. measuments = -133 of channel = 16
// Avg. measuments = -136 of channel = 17
// Avg. measuments = -134 of channel = 18
// Avg. measuments = -134 of channel = 19
// Avg. measuments = -138 of channel = 20
// Avg. measuments = -132 of channel = 21
// Avg. measuments = -135 of channel = 22
// Avg. measuments = -134 of channel = 23
// Avg. measuments = -138 of channel = 24
// Avg. measuments = -136 of channel = 25
// Lowest avg. = -138 at channel = 20
// Avg. measuments = -136 of channel = 11
// Avg. measuments = -138 of channel = 12
// Avg. measuments = -135 of channel = 13
// Avg. measuments = -138 of channel = 14
// Avg. measuments = -137 of channel = 15
// Avg. measuments = -138 of channel = 16
// Avg. measuments = -136 of channel = 17
// Avg. measuments = -136 of channel = 18
// Avg. measuments = -134 of channel = 19
// Avg. measuments = -138 of channel = 20
// Avg. measuments = -132 of channel = 21
// Avg. measuments = -135 of channel = 22
// Avg. measuments = -134 of channel = 23
// Avg. measuments = -138 of channel = 24
// Avg. measuments = -136 of channel = 25
// Lowest avg. = -138 at channel = 12
