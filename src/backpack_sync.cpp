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

bool BackpackSync::SendData()
{
  LightParams2 tmp_msg;

 
   tmp_msg.time_delta_ms = xTaskGetTickCount() / configTICK_RATE_HZ * 1000;

  counter++;
  ESP_LOGI(TAG_BACKPACK, " test: %d, max_length %d:",
    sizeof(tmp_msg), ESP_NOW_MAX_DATA_LEN);
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(NULL, (uint8_t *)&tmp_msg, sizeof(tmp_msg));
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
  LightParams2 tmp_msg;
  for (int i = 0; i < len / sizeof(tmp_msg); i += sizeof(tmp_msg))
  {
    memcpy(&tmp_msg + i, incomingData, sizeof(tmp_msg));
    if( Singleton::GetInstance()->GetParamsQueue() != 0 )
    {
        /* Send an unsigned long.  Wait for 10 ticks for space to become
        available if necessary. */
        if( xQueueSend( Singleton::GetInstance()->GetParamsQueue(),
                       &tmp_msg,
                       ( TickType_t ) 10 ) != pdPASS )
        {
            /* Failed to post the message, even after 10 ticks. */
            ESP_LOGE(TAG_BACKPACK,"Failed To Push Msg to Queue!");
        }
    }
  }
}
}  // namespace backpack