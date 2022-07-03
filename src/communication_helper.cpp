#include "communication_helper.h"

#include <Arduino.h>
#include <M5Core2.h>
#include "WiFi.h"
#include <esp_log.h>
#include <esp_now.h>
#include "freertos/FreeRTOS.h"
#include "patterns.h"
#include "freertos/queue.h"

namespace backpack {

const char *TAG_COMMS = "CommunicationHelper";


void SetupWifi() {
  // WiFi Setup For ESP-NOW
  WiFi.mode(WIFI_MODE_STA);
  esp_read_mac(my_mac, ESP_MAC_WIFI_STA);
  // Init ESP-NOW
  if (esp_now_init() != ESP_OK)
  {
    ESP_LOGE(TAG_COMMS, "Error initializing ESP-NOW");
    return;
  }

  // Add all the Peers
  esp_now_peer_info_t peerInfo;
  memset(&peerInfo, 0, sizeof(peerInfo));
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Register peers one by one (except our selves)
  for (int i = 0; i < sizeof(my_peers_addresses) / MAC_ADDR_LEN; i++)
  {
    if (0 == memcmp(my_peers_addresses[i], my_mac, MAC_ADDR_LEN))
      continue;
    memcpy(peerInfo.peer_addr, my_peers_addresses[i], 6);
    if (esp_now_add_peer(&peerInfo) != ESP_OK)
      ESP_LOGE(TAG_COMMS, "Failed to add peer: %02X %02x %02X %02X %02X %02X", \
                my_peers_addresses[i][0], my_peers_addresses[i][1], \
                my_peers_addresses[i][2], my_peers_addresses[i][3], \
                my_peers_addresses[i][4], my_peers_addresses[i][5]);
    else
      ESP_LOGI(TAG_COMMS, "Added peer: %02X %02x %02X %02X %02X %02X", 
                my_peers_addresses[i][0], my_peers_addresses[i][1], \
                my_peers_addresses[i][2], my_peers_addresses[i][3], \
                my_peers_addresses[i][4], my_peers_addresses[i][5]);
  }

  ESP_LOGI(TAG_COMMS, "ESP_ERR_ESPNOW_BASE: %d", ESP_ERR_ESPNOW_BASE);

  }

}  // namespace backpack