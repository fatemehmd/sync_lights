#include "graphic_controller.h"

#include <esp_log.h>
#include "graphics/pattern_generator.h"

namespace backpack {
namespace graphics {

const char *TAG_GRAPHIC = "gcntroller";

std::string getPatternList() {
    std::string list = "";
    for (const std::string& s : graphics::patternList) {
        list += s + "\n";
    }
    return list;
}


void GraphicController::setup() {
  transport.Reset();

  delay(2000);

  outputs = new Outputs(&state);
  renderer = new Renderer(state);

 ESP_LOGI(TAG_GRAPHIC, "controller setup complete");

    for (int layerIdx = 0; layerIdx < NUM_LAYERS; layerIdx++)
    {
        setPattern(layerIdx, layerIdx);
        state.setLayerParam(LayerParams::MixOpacity, layerIdx, 0);
    }
    setPattern(0, 0);
    state.setLayerParam(LayerParams::MixOpacity, 0, 180);
    setPattern(1, 6);
    state.setLayerParam(LayerParams::MixOpacity, 1, 150);
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

void GraphicController::setPattern(int layerIdx, int patternIdx) {
    state.setSelectedPattern(layerIdx, patternIdx, true);

    Pattern* pattern = PatternGenerator::makePattern(patternIdx);
    renderer->setPattern(layerIdx, patternIdx, pattern);
    
    state.registerGenerator(layerIdx, pattern->getLabel(), pattern->getParamMetaData(), true);
}

int GraphicController::getPattern(int layerIdx) {
    return state.getSelectedPattern(layerIdx);
}


void GraphicController::setPatternParam(int layerIdx, int paramIdx, int value) {
    state.changePatternParam(layerIdx, paramIdx, value);
}

void GraphicController::getPatternParams(int layerIdx) {
    //return state.getPatternParam(layerIdx);
}

void GraphicController::setOpacity(int layerIdx, int value) {
    // lvgl value is between 0..100. Scaling up for fast led.
    state.setLayerParam(LayerParams::MixOpacity, layerIdx, value);
}

int GraphicController::getOpacity(int layerIdx) {
    // lvgl value is between 0..100. Scaling down.
    int opacity = state.layerParam(LayerParams::MixOpacity, layerIdx);
    return opacity;
}

void GraphicController::setHue(int layerIdx, int hue) {
    ESP_LOGI(TAG_GRAPHIC, "hue: %d", hue);
    state.setHue(layerIdx, hue);
}

int GraphicController::getHue(int layerIdx) {
    int hue = state.getHue(layerIdx);
    ESP_LOGI(TAG_GRAPHIC, "hue: fastled_hue: %d",  hue);
    return hue;
}

backpack::LightParams GraphicController::getLightParams() {
    // layer, color, pattern
    LightParams params;
    strcpy(params.message, "Hello!");
    params.time_delta_ms = xTaskGetTickCount() / configTICK_RATE_HZ * 1000;
    for (int i=0; i<M5_NUM_LAYERS; i++) {
        params.layer_data[i].layerIdx = i;
        params.layer_data[i].opacity = getOpacity(i);
        params.layer_data[i].pattern = getPattern(i);
        params.layer_data[i].hue = getHue(i);
    }

    return params;
}

void GraphicController::setLightParams(backpack::LightParams& params) {

      for (int i=0; i<M5_NUM_LAYERS; i++) {
        ESP_LOGI(TAG_GRAPHIC, "layer %d, pattern %d, opacity %d", 
                      params.layer_data[i].layerIdx, 
                      params.layer_data[i].pattern, 
                      params.layer_data[i].opacity);

        int layerIdx = params.layer_data[i].layerIdx;
        setPattern(layerIdx,  params.layer_data[i].pattern);
        setHue(layerIdx, params.layer_data[i].hue);
       // setHueSpan(layerIdx,params.layer_data[i].pallete_params.hue_span);
        setOpacity(layerIdx, params.layer_data[i].opacity);
      }
      newParams = true;
}

bool GraphicController::paramsUpdated() {
    return newParams;

}

LightParams GraphicController::readNewParams() {
    newParams = false;
    return getLightParams();
}

void GraphicController::setPowerOff() {
    powerOff = true;
}

bool GraphicController::getPowerOff() {
    return powerOff;
}

}  // namespace graphics
}  // namespace backpack 