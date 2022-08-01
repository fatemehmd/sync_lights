#include "pattern.h"

Pattern::Pattern(const char *labelStr)
{
    label = labelStr;
    pMetadata = new paramMetadata[NUM_PARAMS_SIX];
}

Pattern::~Pattern()
{
    delete pMetadata;
}

const char *Pattern::getLabel()
{
    return label;
}

void Pattern::setLayerIdx(uint8_t layerIdx)
{
    _layerIdx = layerIdx;
}

uint8_t Pattern::patternParam(State &state, int idx)
{
    return state.effectiveParam(_layerIdx, idx);
}

float Pattern::octave(State &state, int idx)
{
    switch (patternParam(state, idx))
    {
    case 0 ... 31:
        return 1 / 8.0;
    case 32 ... 63:
        return 1 / 4.0;
    case 64 ... 95:
        return 1 / 2.0;
    case 96 ... 127:
        return 1;
    case 128 ... 159:
        return 2;
    case 160 ... 191:
        return 4;
    case 192 ... 223:
        return 8;
    case 224 ... 255:
        return 16;
    }
}

paramMetadata *Pattern::getParamMetaData()
{

    return pMetadata;
}

void Pattern::fill(uint8_t *output, long dt, State &state){};