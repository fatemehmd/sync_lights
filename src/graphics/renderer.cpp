#include "renderer.h"

Renderer::Renderer(State &state)
{
    paramMetadata fadeMetadata[NUM_PARAMS_SIX];

    fadeMetadata[0] = {NORMAL, 32, "AMT"};
    fadeMetadata[1] = {NORMAL, 0, "LVL"};

    int bankIdx = state.registerFX("Fade", fadeMetadata);

    fadeFX = new Fade(bankIdx);

    for (int layerIdx = 0; layerIdx < NUM_LAYERS; layerIdx++)
        patterns[layerIdx] = NULL;
}

void Renderer::setPattern(int layerIdx, int patternIdx, Pattern* pattern) {
    if (patterns[layerIdx] != NULL)
        delete patterns[layerIdx];
    patterns[layerIdx] = pattern;
    patterns[layerIdx]->setLayerIdx(layerIdx);
}

void Renderer::Render(State &state, unsigned long pulses)
{
    state.setupPalette();
    state.syncSnapshot();

    for (int layerIdx = 0; layerIdx < NUM_LAYERS; layerIdx++)
    {
        if (patterns[layerIdx] == NULL)
            continue;
        uint8_t patternIdx = state.layerParam(LayerParams::SelectedPattern, layerIdx);

        bool render = (state.layerParam(LayerParams::Bypass, layerIdx) == 0) &&
                      (!state.anySolo() || (state.anySolo() && state.layerParam(LayerParams::Solo, layerIdx) > 0)) &&
                      (patternIdx != 255);
        if (render)
        {
            patterns[layerIdx]->fill(layerOutput8bit[layerIdx], pulses, state);
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