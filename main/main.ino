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

//extern uint16_t count;
//extern uint16_t preCount;

// I2S
#define I2S_DOUT  05
#define I2S_BCLK  26
#define I2S_LRCK  25

#include "FS.h"
#include "SPIFFS.h"
#define FORMAT_SPIFFS_IF_FAILED true

#include <Audio.h>
Audio audio;

static bool isPlaying = true;
static bool isSelectedSongChanged = false;
static int song_count = 0;
static char textBuf[6];
static uint32_t currentSongDuration = 0;
static uint32_t currentTimeProgress = 0;

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

void cleanup_value();
void play_selected_song();
void Task_Audio(void *pvParameters);

void audioSetup()
{
  /*
   * pvTaskCode: Con trỏ đến hàm sẽ được thực thi bởi tác vụ. Hàm này phải có kiểu trả về là void và không có tham số.
   * pcName: Con trỏ đến chuỗi ký tự đại diện cho tên của tác vụ. Tên này được sử dụng cho mục đích gỡ lỗi và theo dõi.
   * ulStackDepth: Kích thước ngăn xếp (stack) được cấp phát cho tác vụ, tính bằng đơn vị byte. Kích thước ngăn xếp cần đủ lớn để chứa các biến cục bộ và thông tin về trạng thái của tác vụ.
   * pvParameters: Con trỏ đến bất kỳ dữ liệu nào bạn muốn truyền vào hàm của tác vụ.
   * uxPriority: Ưu tiên của tác vụ. Giá trị ưu tiên càng cao, tác vụ càng được ưu tiên thực thi.
   * pxCreatedTask: Con trỏ đến biến TaskHandle_t nơi sẽ lưu trữ tay cầm (handle) của tác vụ được tạo. Tay cầm này có thể được sử dụng để điều khiển tác vụ, chẳng hạn như xóa nó.
   * xCoreID: ID của lõi mà tác vụ sẽ được gán. ESP32 có hai lõi, được đánh số là 0 và 1.
   * 
   */
    // xTaskCreatePinnedToCore(Task_Audio, "Task_Audio", 10240, NULL, 3, NULL, 1); 
    audio.setPinout(I2S_BCLK, I2S_LRCK, I2S_DOUT);
    audio.setVolume(20);
}

void setup()
{
    String title = "LVGL rotation example";

    Serial.begin(115200);
    Serial.println(title + " start");

    if(!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)){
      Serial.println("SPIFFS Mount Failed");
      return;
    }

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

    audioSetup();

    audio.connecttoFS(SPIFFS, "/mixkit-game-click.wav"); // SPIFFS

    Serial.println(title + " end");
}

void loop()
{
  if (isPlaying)
  {
    audio.loop();
  }
  delay(4);
//    Serial.println("IDLE loop");
//    delay(1000);
//    if (count > preCount) {
//      preCount = count;
//      Serial.println(count);
//    }
}

////////////////////////////////////////////
void cleanup_value()
{
  isPlaying = false;
  currentSongDuration = 0;
  currentTimeProgress = 0;
}

void play_selected_song()
{
  cleanup_value();
  char song_filename[256];
  Serial.printf("Play: %s\n", song_filename);
  audio.connecttoFS(SD_MMC, song_filename);
  isPlaying = true;
}

////////////////////////////////////////////
void Task_Audio(void *pvParameters) // This is a task.
{
  while (true)
  {
    if (isPlaying)
    {
      audio.loop();
//      if(Serial.available()){ // put streamURL in serial monitor
//          audio.stopSong();
//          String r=Serial.readString(); r.trim();
//          if(r.length()>5) audio.connecttohost(r.c_str());
//          log_i("free heap=%i", ESP.getFreeHeap());
//      }
      
//      if (currentSongDuration == 0)
//      {
//        currentSongDuration = audio.getAudioFileDuration();
//        if (currentSongDuration > 0)
//        {
//          // Serial.printf("currentSongDuration: %d\n", currentSongDuration);
//          lv_slider_set_range(ui_ScaleProgress, 0, currentSongDuration);
//          sprintf(textBuf, "%02d:%02d", currentSongDuration / 60, currentSongDuration % 60);
//          lv_label_set_text(ui_LabelDuration, textBuf);
//        }
//      }
//      uint32_t currentTime = audio.getAudioCurrentTime();
//      if (currentTime != currentTimeProgress)
//      {
//        currentTimeProgress = currentTime;
//        // Serial.printf("currentTime: %d\n", currentTime);
//        lv_slider_set_value(ui_ScaleProgress, currentTimeProgress, LV_ANIM_ON);
//        sprintf(textBuf, "%02d:%02d", currentTimeProgress / 60, currentTimeProgress % 60);
//        lv_label_set_text(ui_LabelProgress, textBuf);
//        for (int i = 0; i < syncTimeLyricsCount; ++i)
//        {
//          if (syncTimeLyricsSec[i] == currentTime)
//          {
//            lv_roller_set_selected(ui_RollerLyrics, syncTimeLyricsLineIdx[i], LV_ANIM_ON);
//            break;
//          }
//        }
//      }
    }
    delay(1);
  }
}
