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


std::string getPatternList();
}  // namespace graphics

class GraphicController
{
public:
    GraphicController(){};
    ~GraphicController() {};
    void changePatternParam(int layerIdx, int paramIdx, int value);
    void changeOpacity(int layerIdx, int value);
    void setPattern(int layerIdx, int patternIdx);
    void getPatternParams(int layerIdx);
    int getLayerPattern(int layerIdx);
    void setHue(int layerIx, int hue);
    void setup();
    void update();

private:
    Renderer *renderer;
    Outputs *outputs;
    State state = State();
    Transport transport = Transport(125);

};
#endif