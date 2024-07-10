/**
 * # Squareline Lvgl 8 ESP Panel for WT32-SC01 Board Project Template
 *
 * The example demonstrates how to use LVGL to rotate the display.
 *
 * ## How to Use
 *
 * To use this example, please firstly install the following dependent libraries:
 *
 * - lvgl (v8.3.x)
 *
 * Then follow the steps below to configure:
 *
 * 1. For **ESP32_Display_Panel**:
 *
 *     - [Configure drivers](https://github.com/esp-arduino-libs/ESP32_Display_Panel#configuring-drivers) if needed.
 *     - If using a supported development board, follow the [steps](https://github.com/esp-arduino-libs/ESP32_Display_Panel#using-supported-development-boards) to configure it.
 *     - If using a custom board, follow the [steps](https://github.com/esp-arduino-libs/ESP32_Display_Panel#using-custom-development-boards) to configure it.
 *
 * 2. Follow the [steps](https://github.com/esp-arduino-libs/ESP32_Display_Panel#configuring-lvgl) to configure the **lvgl**.
 * 3. Modify the macros in the [lvgl_port_v8.h](./lvgl_port_v8.h) file to configure the LVGL porting parameters.
 * 4. Navigate to the `Tools` menu in the Arduino IDE to choose a ESP board and configure its parameters, please refter to [Configuring Supported Development Boards](https://github.com/esp-arduino-libs/ESP32_Display_Panel#configuring-supported-development-boards)
 * 5. Verify and upload the example to your ESP board.
 *
 * ## Serial Output
 *
 * ```bash
 * ...
 * LVGL rotation example start
 * Initialize panel device
 * Initialize LVGL
 * Create UI
 * LVGL rotation example end
 * IDLE loop
 * IDLE loop
 * ...
 * ```
 *
 * ## Troubleshooting
 *
 * Please check the [FAQ](https://github.com/esp-arduino-libs/ESP32_Display_Panel#faq) first to see if the same question exists. If not, please create a [Github issue](https://github.com/esp-arduino-libs/ESP32_Display_Panel/issues). We will get back to you as soon as possible.
 *
 */

#include <Arduino.h>
#include <ESP_Panel_Library.h>
#include <lvgl.h>
#include "lvgl_port_v8.h"
#include "src/ui/ui.h"

/*Change to your screen resolution*/
static const uint16_t screenWidth  = 480;
static const uint16_t screenHeight = 320;

#if LVGL_PORT_AVOID_TEAR
    #error "This example does not support the avoid tearing function. Please use `LVGL_PORT_ROTATION_DEGREE` for rotation"
#endif

static lv_disp_rot_t rotation = LV_DISP_ROT_NONE;

static void rotateDisplay(lv_disp_t *disp, lv_disp_rot_t rotation)
{
    lvgl_port_lock(-1);
    lv_disp_set_rotation(disp, rotation);
    lvgl_port_unlock();
}

void setup()
{
    String title = "LVGL rotation example";

    Serial.begin(115200);
    Serial.println(title + " start");

    Serial.println("Initialize panel device");
    ESP_Panel *panel = new ESP_Panel();
    panel->init();
#if LVGL_PORT_AVOID_TEAR
    // When avoid tearing function is enabled, configure the RGB bus according to the LVGL configuration
    ESP_PanelBus_RGB *rgb_bus = static_cast<ESP_PanelBus_RGB *>(panel->getLcd()->getBus());
    rgb_bus->configRgbFrameBufferNumber(LVGL_PORT_DISP_BUFFER_NUM);
    rgb_bus->configRgbBounceBufferSize(LVGL_PORT_RGB_BOUNCE_BUFFER_SIZE);
#endif
    panel->begin();

    Serial.println("Initialize LVGL");
    lvgl_port_init(panel->getLcd(), panel->getTouch());
    
    /* Rotate display */
    if (screenWidth < screenHeight) {
      rotation = LV_DISP_ROT_NONE;
    } else {
      rotation = LV_DISP_ROT_270;
    }
    rotateDisplay(lv_disp_get_default(), rotation);

    Serial.println("Create UI");
    /* Lock the mutex due to the LVGL APIs are not thread-safe */
    lvgl_port_lock(-1);
    
    /* Rotate display */
    ui_init();
    
    /* Release the mutex */
    lvgl_port_unlock();

    Serial.println(title + " end");
}

void loop()
{
    Serial.println("IDLE loop");
    delay(1000);
}
