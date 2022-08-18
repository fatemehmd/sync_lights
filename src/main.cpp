
#include <Arduino.h>
#include <esp_log.h>
#include <esp_now.h>
#include <lvgl.h>
#include <M5Core2.h>
#include <SPI.h>



#include "freertos/FreeRTOS.h"
#include "WiFi.h"

#include "communication_helper.h"
#include "backpack_sync.h"
#include "data_types.h"
#include "singleton.h"
#include "graphics/state.h"
#include "graphics/outputs.h"
#include "graphics/renderer.h"
#include "graphics/transport.h"
#include "data_types.h"
#include "graphic_controller.h"
#include "display/display_content.h"


const char *TAG = "Main";


using backpack::BackpackSync;
using backpack::Singleton;

TaskHandle_t lightHandle = NULL;

TFT_eSPI tft = TFT_eSPI();

GraphicController graphic_controller = GraphicController();
DisplayContent display_content = DisplayContent(&graphic_controller);

uint8_t layer = 0;
uint8_t opacity = 0;
uint8_t pattern = 0;


void changePattern(int button) {
  switch (button) {
    case 1:
      layer = (layer + 1) %4;
      break;
    case 2:
      pattern = (pattern + 1) %6;
      break;
    default:
    break;
  }

  // M5.Lcd.printf("Switch to layer %d with pattern %d\n", layer, pattern);
  backpack::LightParams tmp_msg;
  tmp_msg.time_delta_ms = xTaskGetTickCount() / configTICK_RATE_HZ * 1000;
  tmp_msg.layer = layer;
  tmp_msg.pattern = pattern;
  if( Singleton::GetInstance()->GetParamsQueue() != 0 )
  {
      /* Send an unsigned long.  Wait for 10 ticks for space to become
      available if necessary. */
      if( xQueueSend( Singleton::GetInstance()->GetParamsQueue(),
                      &tmp_msg,
                      ( TickType_t ) 10 ) != pdPASS )
      {
          /* Failed to post the message, even after 10 ticks. */
          ESP_LOGE(TAG,"Failed To Push Msg to Queue!");
      }
  }
}

void light_task(void *PV_Parameters)
{
  uint32_t ulNotifiedValue;
  backpack::LightParams tmp_msg;
  while (true)
  {
    /*
    ulNotifiedValue = ulTaskNotifyTake(pdFALSE ,0);

    if( ulNotifiedValue != 0 )
    {
      ESP_LOGI(TAG, "recieved notification");
    }*/

    if(pdPASS == xQueueReceive( Singleton::GetInstance()->GetParamsQueue(), &tmp_msg, (TickType_t)0))
    {
      ESP_LOGI(TAG, "recieved msg from queue");
      ESP_LOGI(TAG, "layer %d, pattern %d, opacity %d", tmp_msg.layer, tmp_msg.pattern, tmp_msg.opacity);
      graphic_controller.setPattern(tmp_msg.layer, tmp_msg.pattern);
    }
     vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void setup()
{
  M5.begin();


  esp_log_level_set(TAG, ESP_LOG_DEBUG);
  ESP_LOGI(TAG, "Starting Initialization...");
  // put your setup code here, to run once:
  ESP_LOGI(TAG, "Settuing Up Display...");

  backpack::SetupWifi();


  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(BackpackSync::OnDataSent);
  esp_now_register_recv_cb(BackpackSync::OnDataRecv);


  BaseType_t xReturned;

  tft.begin();
  tft.setRotation(1);

  graphic_controller.setup();
  display_content.setup();

  /* Create the task, storing the handle. */
  xReturned = xTaskCreate(
      light_task,       /* Function that implements the task. */
      "lightzzzz",      /* Text name for the task. */
      2048,             /* Stack size in words, not bytes. */
      (void *)1,        /* Parameter passed into the task. */
      tskIDLE_PRIORITY, /* Priority at which the task is created. */
      &lightHandle);        /* Used to pass out the created task's handle. */
}



void loop()
{
  // put your main code here, to run repeatedly:
  M5.update(); // Read the press state of the key.
  if (M5.BtnA.wasReleased() || M5.BtnA.pressedFor(1000, 200))
  {
    if (BackpackSync::SendData())
    {
      /* M5.Lcd.setTextColor(GREEN);
      M5.Lcd.println("Broadcasted."); */
    }
    else
    {
/*       M5.Lcd.setTextColor(RED);
      M5.Lcd.println("Broadcast Failed!"); */
    }
  }
  else if (M5.BtnB.wasReleased() || M5.BtnB.pressedFor(1000, 200))
  {
    //M5.Lcd.println("Changing Layer");
    changePattern(1);
  }
  else if (M5.BtnC.wasReleased() || M5.BtnC.pressedFor(1000, 200))
  {
    //M5.Lcd.println("Changing Pattern");
    changePattern(2);

  }
  else if (M5.BtnB.wasReleasefor(700))
  {
   // M5.Lcd.clear(WHITE); // Clear the screen and set white to the background color.
   // M5.Lcd.setCursor(0, 0);
  }

  graphic_controller.update();
  display_content.update();
  delay(5);

}
