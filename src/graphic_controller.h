#ifndef GRAPHICCONTROLLER_H
#define GRAPHICCONTROLLER_H

#include "graphics/state.h"
#include "graphics/renderer.h"
#include "graphics/outputs.h"
#include "graphics/transport.h"

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
}

class GraphicController
{
public:
    GraphicController(){};
    ~GraphicController() {};
    void changePatternParam(int layerIdx, int paramIdx, int value);
    void changeOpacity(int layerIdx, int value);
    void setPattern(int layerIdx, int patternIdx);
    void getPatternParams(int layerIdx);
    void setup();
    void update();

private:
    Renderer *renderer;
    Outputs *outputs;
    State state = State();
    Transport transport = Transport(125);

};
#endif