#ifndef GRAPHICCONTROLLER_H
#define GRAPHICCONTROLLER_H

#include "graphics/state.h"
#include "graphics/renderer.h"
#include "graphics/outputs.h"
#include "graphics/transport.h"
#include "data_types.h"

namespace graphics {
    static const std::string patternList[10] = {"Cloud", 
    "AnalogFeedback", 
    "RainbowBlast", 
    "Particles", 
    "Fire2012",
    "Scarf", 
    "Sparkle",
    "Cleave",
    "Strobe",
    "Piano"};


std::string getPatternList();
}  // namespace graphics

class GraphicController
{
public:
    GraphicController(){};
    ~GraphicController() {};
    void setPatternParam(int layerIdx, int paramIdx, int value);
    void getPatternParams(int layerIdx);

    void setOpacity(int layerIdx, int value);
    int getOpacity(int layerIdx);
  
    void setPattern(int layerIdx, int patternIdx);
    int getPattern(int layerIdx);

    void setHue(int layerIdx, int hue);
    int getHue(int layerIdx);
    
    //void setHueSpan(int layerIdx, int span);
    
    void setLightParams(backpack::LightParams& params);
    backpack::LightParams getLightParams();

    void setup();
    void update();


private:
    Renderer *renderer;
    Outputs *outputs;
    State state = State();
    Transport transport = Transport(125);

};
#endif