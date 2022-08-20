#ifndef DATA_TYPES_H
#define DATA_TYPES_H

#include <Arduino.h>

// only using 2 of the layers for m5 stack. Trying to change num layers in state.h
// crashes the code so not touching that for now.
#define M5_NUM_LAYERS 2

namespace backpack {  


typedef struct LayerData {
  uint8_t layerIdx;
  uint8_t opacity = 200;
  uint8_t pattern = 0;
  uint8_t hue = 125;
  uint8_t hue_span= 150;
};

typedef struct LightParams
{
  uint64_t time_delta_ms;
  LayerData layer_data[M5_NUM_LAYERS] ;
  char message[32];
};
}  // namespace backpack
#endif