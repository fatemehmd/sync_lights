#ifndef LCD_FUNCTIONS_H
#define LCD_FUNCTIONS_H

#include <M5Core2.h>
#include <lvgl.h>
extern TFT_eSPI tft;

//=====================================================================
/*Read the touchpad*/
void my_touchpad_read(lv_indev_drv_t * indev_driver,
                      lv_indev_data_t * data){
  TouchPoint_t pos = M5.Touch.getPressPoint();
  bool touched = ( pos.x == -1 ) ? false : true;
  if(!touched) {
    data->state = LV_INDEV_STATE_REL;
  } else {
    data->state = LV_INDEV_STATE_PR; 
    /*Set the coordinates*/
    data->point.x = pos.x;
    data->point.y = pos.y;
  }
}

//=====================================================================
/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, 
  const lv_area_t *area, lv_color_t *color_p){
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);
  tft.startWrite();
  tft.setAddrWindow(area->x1, area->y1, w, h);
  tft.pushColors(&color_p->full, w * h, true);
  tft.endWrite();
  lv_disp_flush_ready(disp);
}
//=====================================================================


#endif