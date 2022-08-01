#ifndef SETTINGS_H
#define SETTINGS_H

#include <Arduino.h>

#define MAC_ADDR_LEN 6


namespace backpack {
    static const uint8_t my_peers_addresses[][MAC_ADDR_LEN] = \
                {{0X08, 0x3A, 0xF2, 0x65, 0xE6, 0x50}, {0x24, 0x0A, 0xC4, 0xF8, 0x8C, 0x48}};
    static uint8_t my_mac[MAC_ADDR_LEN];
}
#endif