#include "renderer.h"

Renderer::Renderer(State &state)
{
    paramMetadata fadeMetadata[NUM_PARAMS_SIX];

    fadeMetadata[0] = {NORMAL, 32, "AMT"};
    fadeMetadata[1] = {NORMAL, 0, "LVL"};

    int bankIdx = state.registerFX("Fade", fadeMetadata);

    fadeFX = new Fade(bankIdx);

    for (int layerIdx = 0; layerIdx < NUM_LAYERS; layerIdx++)
        generators[layerIdx] = NULL;
}

Pattern *Renderer::makeGenerator(uint8_t idx)
{
    switch (idx)
    {
    case 0:
        return new Cloud();
    case 1:
        return new AnalogFeedback();
    case 2:
        return new RainbowBlast();
    case 3:
        return new Particles();
    case 4:
        return new Fire2012();
    case 5:
        return new Scarf();
    case 6:
        return new Sparkle();
    case 7:
        return new Cleave();
    case 8:
        return new Strobe();
    case 9:
        return new Piano();
    default:
        return new TestPattern();
    }
}

void Renderer::Render(State &state, unsigned long pulses)
{
    state.setupPalette();
    state.syncSnapshot();

    for (int layerIdx = 0; layerIdx < NUM_LAYERS; layerIdx++)
    {
        uint8_t patternIdx = state.layerParam(LayerParams::SelectedPattern, layerIdx);
        uint8_t mode = patternIdx >> 6;
        patternIdx = patternIdx % 64;

        if (mode > 0)
        {
            if (generators[layerIdx] != NULL)
                delete generators[layerIdx];
            generators[layerIdx] = makeGenerator(patternIdx);
            generators[layerIdx]->setLayerIdx(layerIdx);

            bool setDefaults = (mode == 1);
            state.setLayerParam(LayerParams::SelectedPattern, layerIdx, patternIdx);
            state.registerGenerator(layerIdx, generators[layerIdx]->getLabel(), generators[layerIdx]->getParamMetaData(), setDefaults);
        }

        bool render = (state.layerParam(LayerParams::Bypass, layerIdx) == 0) &&
                      (!state.anySolo() || (state.anySolo() && state.layerParam(LayerParams::Solo, layerIdx) > 0)) &&
                      (patternIdx != 255);
        if (render)
        {
            generators[layerIdx]->fill(layerOutput8bit[layerIdx], pulses, state);
            for (int i = 0; i < STRAND_LENGTH; i++)
                layerOutput[layerIdx][i] = state.getColor(layerIdx, layerOutput8bit[layerIdx][i]);
            if (state.layerParam(LayerParams::MixOpacity, layerIdx) < 255)
                fadeToBlackBy(layerOutput[layerIdx], STRAND_LENGTH, 255 - state.layerParam(LayerParams::MixOpacity, layerIdx));
        }
        else
        {
            fill_solid(layerOutput[layerIdx], STRAND_LENGTH, CRGB::Black);
        }
    }

    for (int i = 0; i < STRAND_LENGTH; i++)
    {
        mixerOutput[i] = CRGB::Black;
        for (int layerIdx = 0; layerIdx < NUM_LAYERS; layerIdx++)
            mixerOutput[i] += layerOutput[layerIdx][i];
    }

    CRGB dryOutput[STRAND_LENGTH];
    CRGB wetOutput[STRAND_LENGTH];

    memcpy(dryOutput, mixerOutput, sizeof(CRGB) * STRAND_LENGTH);

    // fade FX
    fadeFX->process(wetOutput, mixerOutput, STRAND_LENGTH, state);

    nblend(wetOutput, dryOutput, STRAND_LENGTH, 255 - state.globalParam(GlobalParams::FXOpacity));
    memcpy(mixerOutput, wetOutput, STRAND_LENGTH * sizeof(CRGB));

    if (state.globalParam(GlobalParams::MasterOpacity) < 255)
        fadeToBlackBy(mixerOutput, STRAND_LENGTH, 255 - state.globalParam(GlobalParams::MasterOpacity));
}

void Renderer::CopyOutput(CRGB *mOutput, CRGB *pOutput)
{
    memcpy(mOutput, mixerOutput, sizeof(CRGB) * STRAND_LENGTH);
    memcpy(pOutput, previewOutput, sizeof(CRGB) * STRAND_LENGTH);
}