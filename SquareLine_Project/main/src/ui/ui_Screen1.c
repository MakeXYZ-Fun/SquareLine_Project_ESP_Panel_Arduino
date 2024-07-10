// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.4.1
// LVGL version: 8.3.11
// Project name: SquareLine_Project

#include "ui.h"

void ui_Screen1_screen_init(void)
{
ui_Screen1 = lv_obj_create(NULL);
lv_obj_clear_flag( ui_Screen1, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

ui_btnClickMe = lv_btn_create(ui_Screen1);
lv_obj_set_width( ui_btnClickMe, 140);
lv_obj_set_height( ui_btnClickMe, 50);
lv_obj_set_x( ui_btnClickMe, 0 );
lv_obj_set_y( ui_btnClickMe, -30 );
lv_obj_set_align( ui_btnClickMe, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_btnClickMe, LV_OBJ_FLAG_SCROLL_ON_FOCUS );   /// Flags
lv_obj_clear_flag( ui_btnClickMe, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

ui_lblClickMe = lv_label_create(ui_btnClickMe);
lv_obj_set_width( ui_lblClickMe, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_lblClickMe, LV_SIZE_CONTENT);   /// 1
lv_obj_set_align( ui_lblClickMe, LV_ALIGN_CENTER );
lv_label_set_text(ui_lblClickMe,"Click Me");

ui_lblCount = lv_label_create(ui_Screen1);
lv_obj_set_width( ui_lblCount, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_lblCount, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_lblCount, 0 );
lv_obj_set_y( ui_lblCount, 30 );
lv_obj_set_align( ui_lblCount, LV_ALIGN_CENTER );
lv_label_set_text(ui_lblCount,"0");

lv_obj_add_event_cb(ui_btnClickMe, ui_event_btnClickMe, LV_EVENT_ALL, NULL);

}