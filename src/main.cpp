#include <Arduino.h>

#include "cryo_sleep.h"
#include "cryo_radio.h"
#include "cryo_power.h"
#include "INA3221.h"

#ifndef __TIME__
#define __TIME__ "12:00:00"
#endif

/*
  Configuration Macros
  --------------------
  Adjust the values of these macros to modify behaviour of the receiver
*/
#define MAX_PACKET_LENGTH 200
#define BAUD_RATE 9600
// #define RECEIVER_MODE MODE_TEXT
#define RECEIVER_MODE MODE_BYTES

// Select between text-based output (for a serial monitor) a direct packet forwarder
enum receiver_mode {
  MODE_TEXT,
  MODE_BYTES
};


PseudoRTC* my_rtc;
cryo_radio_packet my_packet;
char timestamp[CRYO_RTC_TIMESTAMP_LENGTH];
char output_buffer[MAX_PACKET_LENGTH];

void receiver_message();

void setup() {

    Serial.begin(BAUD_RATE);

    // configure power meter (although not used)
    cryo_power_init();

    // configure clock
    Serial.println("# configuring clock");
    cryo_configure_clock(__DATE__, __TIME__);

    // initialise packet with dummy values
    my_packet.sensor_id = 0xc500;
    my_packet.packet_id = 0;

    my_rtc = cryo_get_rtc();

    // initialise radio
    Serial.println("# Initialising radio");
    if (!cryo_radio_init(0xC5009900, my_rtc))
        Serial.println("# Failed to initialise radio");
    else 
        digitalWrite(LED_BUILTIN, LOW);

    Serial.println("# Receiver mode!");
    Serial.println("# Adding receiver notification");
    cryo_add_alarm_every(60, receiver_message);

    Serial.print("# Program starting at ");    
    // Print timestamp
    my_rtc->get_timestamp(timestamp);
    Serial.print(timestamp);
    Serial.print(" in ");
    if (RECEIVER_MODE == MODE_TEXT) {
      Serial.println("text mode.");
    } else {
      Serial.println("receiver mode.");
    }

    delay(100);

}

void loop () {

    int32_t rssi = -999;
    if (cryo_radio_receive_packet(&my_packet, &rssi)) {
    
        if (RECEIVER_MODE == MODE_TEXT) {

            my_rtc->get_timestamp(timestamp);
            // 0
            Serial.print(timestamp);
            Serial.print("\t");
            // 1
            Serial.print(rssi);
            Serial.print("\t");
            // 2
            Serial.print(my_packet.timestamp);
            Serial.print("\t");
            // 3
            Serial.print(my_packet.packet_type); 
            Serial.print("\t");
            // 4
            Serial.print(my_packet.packet_length);
            Serial.print("\t");
            // 5
            Serial.print(my_packet.packet_id);
            Serial.print("\t");
            // 6
            Serial.print(my_packet.sensor_id);
            Serial.print("\t");
            // 7
            Serial.print(my_packet.ds18b20_temperature);
            Serial.print("\t");
            // 8
            Serial.print(my_packet.pt1000_temperature);
            Serial.print("\t");
            // 9
            Serial.print(my_packet.raw_adc_value);
            Serial.print("\t");
            // 10
            Serial.print(my_packet.battery_voltage);
            Serial.print("\t");
            // 11
            Serial.print(my_packet.battery_current*1000);
            Serial.print("\t");
            // 12
            Serial.print(my_packet.solar_panel_voltage);
            Serial.print("\t");
            // 13
            Serial.print(my_packet.solar_panel_current*1000);
            Serial.print("\t");
            // 14
            Serial.print(my_packet.load_voltage);
            Serial.print("\t");
            // 15
            Serial.print(my_packet.load_current*1000);
            Serial.println();

        } else {

          /* Need to create packet here
           * - do this by calculating the total packet length
           * (i.e. sum of packet, rssi, receiver timestamp and packet length)
           * 
           * Note that SAMD21 is little endian
           * 
           * [0,1] bytes - length
           * [2] byte - RSSI (signed byte)
           * [3,22] bytes - receiver timestamp (as ascii)
           * [23,94] packet
           */

            // flush output buffer
            for (uint8_t k = 0; k < MAX_PACKET_LENGTH; k++) {
              output_buffer[k] = 0;
            }

            my_rtc->get_timestamp(timestamp);

            uint16_t packet_length =
              sizeof(uint16_t) + // 2 magic word
              sizeof(uint16_t) + // 4 packet length
              sizeof(int32_t) +   // 5 rssi
              20 + // timestamp length, ideally replace with macro/const // 25
              sizeof(cryo_radio_packet); // 96

            uint8_t output_index = 0;
            uint16_t magic_word =  0xc5c5;
            memcpy(output_buffer + output_index, &magic_word, sizeof(uint16_t));
            output_index += sizeof(magic_word);
            
            memcpy(output_buffer + output_index, &packet_length, sizeof(uint16_t));
            // increment index
            output_index += sizeof(packet_length);

            memcpy(output_buffer + output_index, &rssi, sizeof(rssi));
            output_index += sizeof(rssi); 

            memcpy(output_buffer + output_index, &timestamp, 20); // replace 20 with non-fixed width
            output_index += 20;

            memcpy(output_buffer + output_index, &my_packet, sizeof(cryo_radio_packet));
            //

            // Then we write to the output
            Serial.write(output_buffer, packet_length);

        }
 
    }

    digitalWrite(LED_BUILTIN, LOW);

}

void receiver_message() {
    my_rtc->get_timestamp(timestamp);
    Serial.print("# Receiver ON @ ");
    Serial.println(timestamp);
}