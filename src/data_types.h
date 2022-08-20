#ifndef DATA_TYPES_H
#define DATA_TYPES_H

#include <Arduino.h>

namespace backpack {

typedef struct PalleteParams {
  uint8_t hue = 125;
  uint8_t hue_span= 150;
};

typedef struct LayerData {
  uint8_t layerIdx;
  PalleteParams pallete_params;
  uint8_t opacity = 200;
  uint8_t pattern = 0;
};

typedef struct LightParams
{
  uint64_t time_delta_ms;
  LayerData layer_data[2] ;
  char message[32];
};




}  // namespace backpack
#endif