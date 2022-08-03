#ifndef PATTERN_H
#define PATTERN_H

#ifndef FASTLED_VERSION
#include <FastLED.h>
#endif

#include "graphics/state.h"

class Pattern
{
public:
    Pattern(const char *labelStr);
    virtual ~Pattern();
    virtual void fill(uint8_t *output, long dt, State &state);
    const char *getLabel();
    paramMetadata *getParamMetaData();
    void setParamIdx(int idx);
    void setLayerIdx(uint8_t layerIdx);

protected:
    paramMetadata *pMetadata;
    uint8_t patternParam(State &state, int idx);
    float octave(State &state, int idx);

private:
    uint8_t _layerIdx;
    const char *label;
};

#endif