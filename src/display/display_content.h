#ifndef DISPLAY_CONTENT_H
#define DISPLAY_CONTENT_H

#include <lvgl.h>

#include "data_types.h"
#include "graphic_controller.h"



namespace backpack {
namespace display {
    struct LayerUserData {
        LayerUserData() {};
        LayerUserData(backpack::graphics::GraphicController* controller_ptr,
        int layer_id) : controller(controller_ptr), layerIdx(layer_id) {};

        backpack::graphics::GraphicController* controller = nullptr;
        int layerIdx = 0;
    };
class DisplayContent
{
public:
    DisplayContent(backpack::graphics::GraphicController* graphic_controller)
    :graphic_controller_(graphic_controller)
    {};
    ~DisplayContent(){};
    void setup();
    void update();
    //static GraphicController* getGraphicController();
private:
    backpack::graphics::GraphicController* graphic_controller_ = nullptr;
    void createLayerContent(lv_obj_t* tab_ptr, int layerIdx);
    void createWelcomeContent(lv_obj_t* tab_ptr);
    const char *TAG = "DisplayContent";
    // Objects that have callback. 
    lv_obj_t * tab[M5_NUM_LAYERS] = {};
    lv_obj_t * ddlist[M5_NUM_LAYERS] = {};
    lv_obj_t * cw[M5_NUM_LAYERS] = {};
};
}  // namespace display
}  // namespace backpack

#endif