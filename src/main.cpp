#include <Arduino.h>
#include <M5Core2.h>
#include "WiFi.h"
#include <esp_log.h>
#include <esp_now.h>
#include "freertos/FreeRTOS.h"
#include "patterns.h"
#include "freertos/queue.h"

const char *TAG = "Mahan";

#define MAC_ADDR_LEN 6

using backpack::PatternId;
using backpack::Patterns;

static const uint8_t my_peers_addresses[][MAC_ADDR_LEN] = {{0X08, 0x3A, 0xF2, 0x65, 0xE6, 0x50}, {0x24, 0x0A, 0xC4, 0xF8, 0x8C, 0x48}};
static uint8_t my_mac[MAC_ADDR_LEN];
int counter = 0;
Patterns patterns;
PatternId pattern_id;
TaskHandle_t xHandle = NULL;
QueueHandle_t ParamsQueue = NULL;


typedef struct message_s
{
  uint64_t time_delta_ms;
  uint8_t par_1;
  uint8_t par_2;
  uint8_t par_3;
  char str[32];
};

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  ESP_LOGI(TAG, "\r\nLast Packet Send Status:\t");
  ESP_LOGI(TAG, "%s", status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status == ESP_NOW_SEND_SUCCESS)
  {
    M5.Lcd.setTextColor(GREEN);
    M5.Lcd.println("Delivery Success");
  }
  else
  {
    M5.Lcd.setTextColor(RED);
    M5.Lcd.println("Delivery Fail");
  }
}

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
  message_s tmp_msg;
  for (int i = 0; i < len / sizeof(tmp_msg); i += sizeof(tmp_msg))
  {
    memcpy(&tmp_msg + i, incomingData, sizeof(tmp_msg));
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.printf("Got Data:%d\r\n", i);
    if( ParamsQueue != 0 )
    {
        /* Send an unsigned long.  Wait for 10 ticks for space to become
        available if necessary. */
        if( xQueueSend( ParamsQueue,
                       &tmp_msg,
                       ( TickType_t ) 10 ) != pdPASS )
        {
            /* Failed to post the message, even after 10 ticks. */
            ESP_LOGE(TAG,"Failed To Push Msg to Queue!");
        }
    }
  }
}

void light_task(void *PV_Parameters)
{
  uint32_t ulNotifiedValue;
  message_s tmp_msg;
  while (true)
  {
    ulNotifiedValue = ulTaskNotifyTake(pdFALSE ,0);

    if( ulNotifiedValue != 0 )
    {
      ESP_LOGI(TAG, "recieved notification");

      patterns.TogglePattern();
    }

    if(pdPASS == xQueueReceive( ParamsQueue, &tmp_msg, (TickType_t)0))
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
  ParamsQueue = xQueueCreate( 10, sizeof(struct message_s) );

  esp_log_level_set(TAG, ESP_LOG_DEBUG);
  ESP_LOGI(TAG, "Starting Initialization...");
  // put your setup code here, to run once:
  ESP_LOGI(TAG, "Settuing Up Display...");

  M5.Lcd.setCursor(0, 0);     // Move the cursor position to (x,y).
  M5.Lcd.setTextColor(WHITE); // Set the font color to white.
  M5.Lcd.setTextSize(2);      // Set the font size.
  M5.Lcd.println("Display Test!");

  // WiFi Setup For ESP-NOW
  WiFi.mode(WIFI_MODE_STA);
  esp_read_mac(my_mac, ESP_MAC_WIFI_STA);
  // Init ESP-NOW
  if (esp_now_init() != ESP_OK)
  {
    ESP_LOGE(TAG, "Error initializing ESP-NOW");
    return;
  }
  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);
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
      ESP_LOGE(TAG, "Failed to add peer: %02X %02x %02X %02X %02X %02X", my_peers_addresses[i][0], my_peers_addresses[i][1], my_peers_addresses[i][2], my_peers_addresses[i][3], my_peers_addresses[i][4], my_peers_addresses[i][5]);
    else
      ESP_LOGI(TAG, "Added peer: %02X %02x %02X %02X %02X %02X", my_peers_addresses[i][0], my_peers_addresses[i][1], my_peers_addresses[i][2], my_peers_addresses[i][3], my_peers_addresses[i][4], my_peers_addresses[i][5]);
  }

  ESP_LOGI(TAG, "ESP_ERR_ESPNOW_BASE: %d", ESP_ERR_ESPNOW_BASE);

  // setup lights
  patterns.Setup();
  pattern_id = PatternId::kRainbow;

  BaseType_t xReturned;


  /* Create the task, storing the handle. */
  xReturned = xTaskCreate(
      light_task,       /* Function that implements the task. */
      "lightzzzz",      /* Text name for the task. */
      2048,             /* Stack size in words, not bytes. */
      (void *)1,        /* Parameter passed into the task. */
      tskIDLE_PRIORITY, /* Priority at which the task is created. */
      &xHandle);        /* Used to pass out the created task's handle. */
}

bool send_data()
{
  message_s tmp_msg;
  tmp_msg.time_delta_ms = xTaskGetTickCount() / configTICK_RATE_HZ * 1000;
  tmp_msg.par_1 = counter;
  tmp_msg.par_2 = 0;
  tmp_msg.par_3 = 0;
  strcpy(tmp_msg.str, "Hello!");
  counter++;
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(0, (uint8_t *)&tmp_msg, sizeof(tmp_msg));
  ESP_LOGI(TAG, "result:%d", result);
  return (ESP_OK == result);
}

void loop()
{
  // put your main code here, to run repeatedly:
  M5.update(); // Read the press state of the key.
  if (M5.BtnA.wasReleased() || M5.BtnA.pressedFor(1000, 200))
  {
    if (send_data())
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
}
