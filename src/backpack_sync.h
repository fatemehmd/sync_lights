#ifndef BACKPACKSYNC_H
#define BACKPACKSYNC_H

#include <Arduino.h>
#include "WiFi.h"
#include <esp_log.h>
#include <esp_now.h>
#include "data_types.h"

namespace backpack {



class BackpackSync
{


public:
   
    BackpackSync(){};
    ~BackpackSync(){};
    static bool SendData(LightParams& light_params);
    static void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
    static void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len);  

private:

};

}  // namespace backpack
#endif