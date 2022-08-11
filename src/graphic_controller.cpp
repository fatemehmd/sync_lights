#include "graphic_controller.h"

#include <esp_log.h>
#include "graphics/pattern_generator.h"

const char *gTAG = "gcntroller";


void GraphicController::setup() {
  transport.Reset();

  delay(2000);

  outputs = new Outputs(&state);
  renderer = new Renderer(state);

 ESP_LOGI(gTAG, "controller setup complete");

    for (int layerIdx = 0; layerIdx < NUM_LAYERS; layerIdx++)
    {
        setPattern(layerIdx, layerIdx);
        state.setLayerParam(LayerParams::MixOpacity, layerIdx, 0);
    }
    setPattern(0, 4);
    state.setLayerParam(LayerParams::MixOpacity, 0, 255);
    setPattern(1,6);
    state.setLayerParam(LayerParams::MixOpacity, 1, 255);
}

void GraphicController::update() {

    uint16_t pulses = transport.Update();
    state.advance(pulses);

    CRGB mixerOutput[STRAND_LENGTH];
    CRGB previewOutput[STRAND_LENGTH];

    renderer->Render(state, pulses);
    renderer->CopyOutput(mixerOutput, previewOutput);

    outputs->display(mixerOutput, previewOutput);

    uint8_t delayMS = state.globalParam(GlobalParams::FrameDelay);
}

void GraphicController::changePatternParam(int layerIdx, int paramIdx, int value) {
    state.changePatternParam(layerIdx, paramIdx, value);
}

void GraphicController::changeOpacity(int layerIdx, int value) {
    state.setLayerParam(LayerParams::MixOpacity, layerIdx, value);
}

void GraphicController::setPattern(int layerIdx, int patternIdx) {
    state.setSelectedPattern(layerIdx, patternIdx, true);

    Pattern* pattern = PatternGenerator::makePattern(patternIdx);
    renderer->setPattern(layerIdx, patternIdx, pattern);
    
    state.registerGenerator(layerIdx, pattern->getLabel(), pattern->getParamMetaData(), true);
}


void GraphicController::getPatternParams(int layerIdx) {
    //return state.getPatternParam(layerIdx);
}

