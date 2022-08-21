
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
using backpack::graphics::GraphicController;
using backpack::display::DisplayContent;

TaskHandle_t lightHandle = NULL;

TFT_eSPI tft = TFT_eSPI();

GraphicController graphic_controller = GraphicController();
DisplayContent display_content = DisplayContent(&graphic_controller);

uint8_t layer = 0;
uint8_t opacity = 0;
uint8_t pattern = 0;


void light_task(void *PV_Parameters)
{
  uint32_t ulNotifiedValue;
  backpack::LightParams light_params;
  while (true)
  {
    /* Example code
    ulNotifiedValue = ulTaskNotifyTake(pdFALSE ,0);
    if( ulNotifiedValue != 0 )
    {
      ESP_LOGI(TAG, "recieved notification");
    }*/

    if(pdPASS == xQueueReceive( Singleton::GetInstance()->GetParamsQueue(), &light_params, (TickType_t)0))
    {
      ESP_LOGI(TAG, "recieved msg from queue");
      graphic_controller.setLightParams(light_params);
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
  backpack::LightParams light_params = graphic_controller.getLightParams();

  strcpy(light_params.message, "Hello!");
    if (BackpackSync::SendData(light_params))
    {
      ESP_LOGI(TAG,"Data sent successfully.");
    }
    else
    {
      ESP_LOGE(TAG,"Failed to send data.");
    }
  }

  graphic_controller.update();
  display_content.update();
  delay(5);

}
