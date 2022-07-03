#ifndef DATA_TYPES_H
#define DATA_TYPES_H

#include <Arduino.h>

namespace backpack {

typedef struct LightParams
{
  uint64_t time_delta_ms;
  uint8_t par_1;
  uint8_t par_2;
  uint8_t par_3;
  char str[32];
};

}  // namespace backpack
#endif