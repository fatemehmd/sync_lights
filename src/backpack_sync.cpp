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
  LightParams tmp_msg;
  tmp_msg.time_delta_ms = xTaskGetTickCount() / configTICK_RATE_HZ * 1000;
  tmp_msg.layer = counter;
  tmp_msg.opacity = 0;
  tmp_msg.pattern = 0;
  strcpy(tmp_msg.str, "Hello!");
  counter++;
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(0, (uint8_t *)&tmp_msg, sizeof(tmp_msg));
  ESP_LOGI(TAG_BACKPACK, "result:%d", result);
  return (ESP_OK == result);
}

// callback when data is sent
void BackpackSync::OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  ESP_LOGI(TAG_BACKPACK, "\r\nLast Packet Send Status:\t");
  ESP_LOGI(TAG_BACKPACK, "%s", status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status == ESP_NOW_SEND_SUCCESS)
  {
   // M5.Lcd.setTextColor(GREEN);
   // M5.Lcd.println("Delivery Success");
  }
  else
  {
   // M5.Lcd.setTextColor(RED);
    // M5.Lcd.println("Delivery Fail");
  }
}

// callback function that will be executed when data is received
void BackpackSync::OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
  LightParams tmp_msg;
  for (int i = 0; i < len / sizeof(tmp_msg); i += sizeof(tmp_msg))
  {
    memcpy(&tmp_msg + i, incomingData, sizeof(tmp_msg));
    //M5.Lcd.setTextColor(WHITE);
   // M5.Lcd.printf("Got Data:%d\r\n", i);
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