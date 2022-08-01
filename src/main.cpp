#include <Arduino.h>
#include <M5Core2.h>
#include "WiFi.h"
#include <esp_log.h>
#include <esp_now.h>
#include "freertos/FreeRTOS.h"
#include "patterns.h"
#include "communication_helper.h"
#include "backpack_sync.h"
#include "data_types.h"
#include "singleton.h"
#include "state.h"
#include "outputs.h"
#include "renderer.h"
#include "transport.h"

const char *TAG = "Mahan";


using backpack::PatternId;
using backpack::Patterns;
using backpack::BackpackSync;
using backpack::Singleton;

Patterns patterns;
PatternId pattern_id;
TaskHandle_t xHandle = NULL;

Renderer *renderer;
Outputs *outputs;
State state = State();
Transport transport = Transport(125);

void light_task(void *PV_Parameters)
{
  uint32_t ulNotifiedValue;
  backpack::LightParams tmp_msg;
  while (true)
  {
    ulNotifiedValue = ulTaskNotifyTake(pdFALSE ,0);

    if( ulNotifiedValue != 0 )
    {
      ESP_LOGI(TAG, "recieved notification");

      patterns.TogglePattern();
    }

    if(pdPASS == xQueueReceive( Singleton::GetInstance()->GetParamsQueue(), &tmp_msg, (TickType_t)0))
    {
      ESP_LOGI(TAG, "recieved msg from queue");
      patterns.TogglePattern();
    }
    patterns.RunPattern();
    // vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void setup()
{
  M5.begin();
 

  esp_log_level_set(TAG, ESP_LOG_DEBUG);
  ESP_LOGI(TAG, "Starting Initialization...");
  // put your setup code here, to run once:
  ESP_LOGI(TAG, "Settuing Up Display...");

  M5.Lcd.setCursor(0, 0);     // Move the cursor position to (x,y).
  M5.Lcd.setTextColor(WHITE); // Set the font color to white.
  M5.Lcd.setTextSize(2);      // Set the font size.
  M5.Lcd.println("Display Test!");

  backpack::SetupWifi();


  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(BackpackSync::OnDataSent);
  esp_now_register_recv_cb(BackpackSync::OnDataRecv);

  // setup lights
  patterns.Setup();
  pattern_id = PatternId::kRainbow;

  BaseType_t xReturned;

  transport.Reset();

  delay(2000);

  outputs = new Outputs(&state);
  renderer = new Renderer(state);

  /* Create the task, storing the handle. */
  xReturned = xTaskCreate(
      light_task,       /* Function that implements the task. */
      "lightzzzz",      /* Text name for the task. */
      4096,             /* Stack size in words, not bytes. */
      (void *)1,        /* Parameter passed into the task. */
      tskIDLE_PRIORITY, /* Priority at which the task is created. */
      &xHandle);        /* Used to pass out the created task's handle. */



  
}



void loop()
{
  // put your main code here, to run repeatedly:
  M5.update(); // Read the press state of the key.
  if (M5.BtnA.wasReleased() || M5.BtnA.pressedFor(1000, 200))
  {
    if (BackpackSync::SendData())
    {
      M5.Lcd.setTextColor(GREEN);
      M5.Lcd.println("Broadcasted.");
    }
    else
    {
      M5.Lcd.setTextColor(RED);
      M5.Lcd.println("Broadcast Failed!");
    }
  }
  else if (M5.BtnB.wasReleased() || M5.BtnB.pressedFor(1000, 200))
  {
    M5.Lcd.println("B");
    xTaskNotify( xHandle,
                  0,
                  eIncrement);
  }
  else if (M5.BtnC.wasReleased() || M5.BtnC.pressedFor(1000, 200))
  {
    M5.Lcd.println("C");
  }
  else if (M5.BtnB.wasReleasefor(700))
  {
    M5.Lcd.clear(WHITE); // Clear the screen and set white to the background color.
    M5.Lcd.setCursor(0, 0);
  }

    uint16_t pulses = transport.Update();
    state.advance(pulses);

    CRGB mixerOutput[STRAND_LENGTH];
    CRGB previewOutput[STRAND_LENGTH];

    renderer->Render(state, pulses);
    renderer->CopyOutput(mixerOutput, previewOutput);

    outputs->display(mixerOutput, previewOutput);

    uint8_t delayMS = state.globalParam(GlobalParams::FrameDelay);
}
