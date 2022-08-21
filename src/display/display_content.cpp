#include "display_content.h"

#include <esp_log.h>
#include <esp_now.h>

#include "lcd_functions.hpp"


namespace {

static lv_disp_draw_buf_t  disp_buf;
static lv_color_t  buf[LV_HOR_RES_MAX * 10];
#define RATE_MCS4_SP 28 // N 43.3Mb MCS4 SP
static lv_style_t small_style;
const char* TAG_DISPLAY = "Display Content";
}

namespace backpack {

using graphics::GraphicController;

namespace display {

/*Will be called when the styles of the base theme are already added
  to add new styles*/
static void new_theme_apply_cb(lv_theme_t * th, lv_obj_t * obj)
{
  lv_obj_add_style(obj, &small_style, 0);
}


static void new_theme_init_and_set(void)
{
    lv_style_init(&small_style);
    /*Initialize the styles*/
    lv_style_set_radius(&small_style, 2);
    lv_style_set_border_width(&small_style, 0);
    /*Initialize the new theme from the current theme*/
    lv_theme_t * th_act = lv_disp_get_theme(NULL);
    static lv_theme_t th_new;
    th_new = *th_act;
    /*Set the parent theme and the style apply callback for the new theme*/
    lv_theme_set_parent(&th_new, th_act);
    lv_theme_set_apply_cb(&th_new, new_theme_apply_cb);
    /*Assign the new theme to the current display*/
    lv_disp_set_theme(NULL, &th_new);
}

void create_tab3_content(lv_obj_t* tab_ptr) {
  lv_obj_t *label = lv_label_create(tab_ptr);
  lv_label_set_text(label, "Colors");
}

static void list_event_handler(lv_event_t * e)
{

    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    if(code == LV_EVENT_VALUE_CHANGED) {

        char buf[32];
        lv_dropdown_get_selected_str(obj, buf, sizeof(buf));
        LV_LOG_USER("Option: %s", buf);
        int patternIdx = lv_dropdown_get_selected(obj);
        display::LayerUserData* user_data = (display::LayerUserData*) lv_event_get_user_data(e);
        user_data->controller->setPattern(user_data->layerIdx, patternIdx);
    }
}

static void slider_event_handler(lv_event_t * e)
{
    lv_obj_t * slider = lv_event_get_target(e);
    lv_obj_t * slider_label = (lv_obj_t*) lv_event_get_user_data(e);
    char buf[8];
    lv_snprintf(buf, sizeof(buf), "%d%%", (int)lv_slider_get_value(slider));
    lv_label_set_text(slider_label, buf);
    lv_obj_align_to(slider_label, slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
}

static void cw_event_handler(lv_event_t* e) {
  lv_obj_t* cw = lv_event_get_target(e);
  display::LayerUserData* user_data = (display::LayerUserData*) lv_event_get_user_data(e);
  lv_color_hsv_t hsv = lv_colorwheel_get_hsv(cw);
  user_data->controller->setHue(user_data->layerIdx, hsv.h);
}

void create_slider(lv_obj_t* parent, int id) {
    static lv_style_t style_main;
    static lv_style_t style_indicator;
    static lv_style_t style_knob;
    static lv_style_t style_label;

    lv_style_init(&style_label);
    lv_style_set_pad_all(&style_label, 0);

    lv_style_init(&style_main);
    lv_style_set_radius(&style_main, 2);
    lv_style_set_width(&style_main, 200);
    lv_style_set_height(&style_main, 5);
    lv_style_set_pad_ver(&style_main, 0); 

    lv_style_init(&style_indicator);
    lv_style_set_radius(&style_indicator, 2);
    lv_style_set_width(&style_indicator, 200);
    lv_style_set_height(&style_indicator, 5);

    lv_style_init(&style_knob);
    lv_style_set_radius(&style_knob, 2);
    lv_style_set_pad_all(&style_knob, 0); /*Makes the knob larger*/
  
    lv_obj_t* param_label = lv_label_create(parent);
    char buf[6];
    lv_snprintf(buf, sizeof(buf), "MM%d", id);
    lv_label_set_text(param_label, buf);
    lv_obj_add_style(param_label, &style_label,LV_PART_MAIN);

    /*Create a slider in the center of the display*/
    lv_obj_t* slider = lv_slider_create(parent);

    lv_obj_add_style(slider, &style_main, LV_PART_MAIN);
    lv_obj_add_style(slider, &style_indicator, LV_PART_INDICATOR);
    lv_obj_add_style(slider, &style_knob, LV_PART_KNOB);

    lv_obj_t* slider_label = lv_label_create(parent);
    lv_label_set_text(slider_label, "0%");
    lv_obj_add_style(slider_label, &style_label,LV_PART_MAIN);

    lv_obj_add_event_cb(slider, slider_event_handler, LV_EVENT_VALUE_CHANGED, slider_label);
}

void DisplayContent::createLayerContent(lv_obj_t* tab_ptr, int layerIdx) {


  display::LayerUserData* layer_user_data = new display::LayerUserData();
  layer_user_data->controller = graphic_controller_;
  layer_user_data->layerIdx = layerIdx;

  lv_obj_t *label = lv_label_create(tab_ptr);
  lv_label_set_text(label, "Pattern:");
  lv_obj_align(label, LV_ALIGN_TOP_LEFT, 0, 10);

  /*Create a normal drop down list*/
  ddlist[layerIdx] = lv_dropdown_create(tab_ptr);
  const char* options = graphics::getPatternList().c_str();
  lv_dropdown_set_options(ddlist[layerIdx], options);
  int pattern = graphic_controller_->getPattern(layerIdx);
  ESP_LOGI(TAG, "Settuing Up List for layer %d with pattern %d", layerIdx, pattern);
  lv_dropdown_set_selected(ddlist[layerIdx], pattern);

  lv_obj_align(ddlist[layerIdx], LV_ALIGN_TOP_MID, 5,5);
  lv_obj_add_event_cb(ddlist[layerIdx], list_event_handler, LV_EVENT_ALL, layer_user_data);

  cw[layerIdx] = lv_colorwheel_create(tab_ptr, true);
  lv_obj_set_size(cw[layerIdx], 150, 150);
  lv_obj_center(cw[layerIdx]);
  lv_obj_align_to(cw[layerIdx], ddlist[layerIdx], LV_ALIGN_OUT_BOTTOM_MID, 0, 15);
  lv_obj_add_event_cb(cw[layerIdx], cw_event_handler, LV_EVENT_ALL, layer_user_data);

  int slider_num = 6;
  lv_obj_t* slider[slider_num];
  lv_obj_t* slider_lbl[slider_num];
  lv_obj_t* slider_val[slider_num];
  lv_obj_t* cont_row[slider_num];

  static lv_style_t cont_style;
  lv_style_init(&cont_style);
  lv_style_set_pad_all(&cont_style, 0);

  lv_obj_t* cont_col = lv_obj_create(tab_ptr);
  lv_obj_set_size(cont_col, 300, 150);
  lv_obj_align_to(cont_col, cw[layerIdx], LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
  lv_obj_set_flex_flow(cont_col, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(cont_col, LV_FLEX_ALIGN_CENTER , LV_FLEX_ALIGN_CENTER ,LV_FLEX_ALIGN_CENTER );
  //lv_obj_clear_flag(tab_ptr, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_add_style(cont_col, &cont_style, 0);

  uint32_t i;

  for(i = 0; i < slider_num; i++) {
    /*Create a container with ROW flex direction*/
    
    lv_obj_t * cont_row = lv_obj_create(cont_col);
    lv_obj_set_size(cont_row, 280, 30);
    lv_obj_set_flex_flow(cont_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(cont_row, LV_FLEX_ALIGN_SPACE_EVENLY , LV_FLEX_ALIGN_CENTER ,LV_FLEX_ALIGN_CENTER );
    lv_obj_clear_flag(cont_row, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(cont_row, &cont_style, -2);

    /*Add items to the row*/
    create_slider(cont_row, i);
  }
}


void DisplayContent::createWelcomeContent(lv_obj_t* tab_ptr) {
  
  //-------------------------------------------------------------------
  /*Gum-like button*/
  /*Properties to transition*/
    static lv_style_prop_t props[] = {
        LV_STYLE_TRANSFORM_WIDTH, LV_STYLE_TRANSFORM_HEIGHT, LV_STYLE_TEXT_LETTER_SPACE, LV_STYLE_PROP_INV
    };

    /*Transition descriptor when going back to the default state.
     *Add some delay to be sure the press transition is visible even if the press was very short*/
    static lv_style_transition_dsc_t transition_dsc_def;
    lv_style_transition_dsc_init(&transition_dsc_def, props, lv_anim_path_overshoot, 250, 100, NULL);

    /*Transition descriptor when going to pressed state.
     *No delay, go to presses state immediately*/
    static lv_style_transition_dsc_t transition_dsc_pr;
    lv_style_transition_dsc_init(&transition_dsc_pr, props, lv_anim_path_ease_in_out, 250, 0, NULL);

    /*Add only the new transition to he default state*/
    static lv_style_t style_def;
    lv_style_init(&style_def);
    lv_style_set_transition(&style_def, &transition_dsc_def);

    /*Add the transition and some transformation to the presses state.*/
    static lv_style_t style_pr;
    lv_style_init(&style_pr);
    lv_style_set_transform_width(&style_pr, 10);
    lv_style_set_transform_height(&style_pr, -10);
    lv_style_set_text_letter_space(&style_pr, 10);
    lv_style_set_transition(&style_pr, &transition_dsc_pr);

    lv_obj_t * btn1 = lv_btn_create(tab_ptr);
    lv_obj_align(btn1, LV_ALIGN_CENTER, 0, -80);
    lv_obj_add_style(btn1, &style_pr, LV_STATE_PRESSED);
    lv_obj_add_style(btn1, &style_def, 0);

    lv_obj_t * label = lv_label_create(btn1);
    lv_label_set_text(label, "Gum");
 
}

void DisplayContent::update() {
  if (graphic_controller_->paramsUpdated()) {
    LightParams params = graphic_controller_->readNewParams();
    ESP_LOGI(TAG_DISPLAY, "Reading new params.");
    for (int i=0; i<M5_NUM_LAYERS; i++) {
      ESP_LOGI(TAG_DISPLAY, "Params for layer %d pattern %d hue %d",
        i, params.layer_data[i].pattern, params.layer_data[i].hue);
      LayerUserData user_data(graphic_controller_, i);

      lv_dropdown_set_selected(ddlist[i], params.layer_data[i].pattern);
      lv_color_hsv_t hsv = {params.layer_data[i].hue, 200, params.layer_data[i].opacity};
      lv_colorwheel_set_hsv(cw[i], hsv);
    }
  }
  lv_task_handler();
  delay(5);
}

void DisplayContent::setup() {
   //-------------------------------------------------------------------
  lv_disp_draw_buf_init(&disp_buf, buf, NULL, LV_HOR_RES_MAX * 10);
  lv_init();

  
  //-------------------------------------------------------------------
  /*Initialize the display*/
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res  = LV_HOR_RES_MAX;
  disp_drv.ver_res  = LV_VER_RES_MAX;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf   = &disp_buf;
  lv_disp_drv_register(&disp_drv);
  
  //-------------------------------------------------------------------
  /*Initialize the (dummy) input device driver*/
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_touchpad_read;
  lv_indev_drv_register(&indev_drv);
  
  //-------------------------------------------------------------------
  /*Create New Theme*/
  new_theme_init_and_set();

  //-------------------------------------------------------------------
  /*Create a Tab view object*/
  lv_obj_t *tabview;
  tabview = lv_tabview_create(lv_scr_act(), LV_DIR_TOP, 30);
  
  //-------------------------------------------------------------------
  /*Add 3 tabs (the tabs are page (lv_page) and can be scrolled*/
  lv_obj_t *tab1 = lv_tabview_add_tab(tabview, "Test");
   tab[0] = lv_tabview_add_tab(tabview, "Layer1");
   tab[1] = lv_tabview_add_tab(tabview, "Layer2");
  
  
  createWelcomeContent(tab1);
  createLayerContent(tab[0], 0);
  createLayerContent(tab[1], 1);
}

}  // namespace display
}  // namespace backpack