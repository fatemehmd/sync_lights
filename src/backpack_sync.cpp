#include "backpack_sync.h"

#include <Arduino.h>
#include <M5Core2.h>
#include "WiFi.h"
#include <esp_log.h>
#include <esp_now.h>
#include "data_types.h"
#include "singleton.h"

namespace backpack {

const char *TAG_BACKPACK = "BacpackSync";
int counter = 0;

bool BackpackSync::SendData(LightParams& light_params)
{

  counter++;
  ESP_LOGI(TAG_BACKPACK, " test: %d, max_length %d:",
  sizeof(light_params), ESP_NOW_MAX_DATA_LEN);
  for (int i= 0; i<M5_NUM_LAYERS; i++) {
  ESP_LOGI(TAG_BACKPACK, "message: %s pattern: %d, opacity: %d, hue: %d", 
    light_params.message, 
    light_params.layer_data[i].pattern, 
    light_params.layer_data[i].opacity, 
    light_params.layer_data[i].hue);
  }

  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(NULL, (uint8_t *)&light_params, sizeof(light_params));
  ESP_LOGI(TAG_BACKPACK, "result:%d", result);
  return (ESP_OK == result);
}

// callback when data is sent
void BackpackSync::OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  ESP_LOGI(TAG_BACKPACK, "\r\nLast Packet Send Status:\t");
  ESP_LOGI(TAG_BACKPACK, "%s", status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// callback function that will be executed when data is received
void BackpackSync::OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
  LightParams light_params;
  for (int i = 0; i < len / sizeof(light_params); i += sizeof(light_params))
  {
    memcpy(&light_params + i, incomingData, sizeof(light_params));
    for (int i= 0; i<M5_NUM_LAYERS; i++) {
      ESP_LOGI(TAG_BACKPACK, " Data recieved, layer: %d, message: %s pattern: %d, opacity: %d, hue: %d", 
        light_params.layer_data[i].layerIdx,
        light_params.message, 
        light_params.layer_data[i].pattern, 
        light_params.layer_data[i].opacity, 
        light_params.layer_data[i].hue);
    }
    if( Singleton::GetInstance()->GetParamsQueue() != 0 )
    {
        /* Send an unsigned long.  Wait for 10 ticks for space to become
        available if necessary. */
        if( xQueueSend( Singleton::GetInstance()->GetParamsQueue(),
                       &light_params,
                       ( TickType_t ) 10 ) != pdPASS )
        {
            /* Failed to post the message, even after 10 ticks. */
            ESP_LOGE(TAG_BACKPACK,"Failed To Push Msg to Queue!");
        }
    }
  }
}
}  // namespace backpack