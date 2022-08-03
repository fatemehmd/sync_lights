#ifndef DATA_TYPES_H
#define DATA_TYPES_H

#include <Arduino.h>

namespace backpack {

typedef struct LightParams
{
  uint64_t time_delta_ms;
  uint8_t layer;
  uint8_t opacity;
  uint8_t pattern;
  char str[32];
};

}  // namespace backpack
#endif