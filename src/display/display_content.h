#ifndef DISPLAY_CONTENT_H
#define DISPLAY_CONTENT_H

#include "graphic_controller.h"
#include <lvgl.h>

namespace display {
    struct LayerUserData {
        GraphicController* controller = nullptr;
        int layerIdx = 0;
    };
}
class DisplayContent
{
public:
    DisplayContent(GraphicController* graphic_controller)
    :graphic_controller_(graphic_controller)
    {};
    ~DisplayContent(){};
    void setup();
    void update();
    //static GraphicController* getGraphicController();
private:
    GraphicController* graphic_controller_ = nullptr;
    void createLayerContent(lv_obj_t* tab_ptr, int layerIdx);
    void createWelcomeContent(lv_obj_t* tab_ptr);
    const char *TAG = "DisplayContent";
};

#endif