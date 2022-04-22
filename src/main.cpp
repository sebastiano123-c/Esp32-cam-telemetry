/** 
 * @file main.cpp
 * @author @sebastiano123-c
 * @brief wifi based telemtry system using your esp3-cam
 * @note board: AI thinker
 * @todo include pid elements as status
 * @version 0.1
 * @date 2022-02-18
 * 
 * @copyright Copyright (c) 2022
 */

#include <Arduino.h>
#include "esp_camera.h"
#include <WiFi.h>


#define CAMERA_MODEL_AI_THINKER // Has PSRAM


#include "camera_pins.h"
#include "app_httpd.h"
#include "telemetry.h"
#include "camSD.h"

//          (WIFI)
const char* ssid                = "Esp32CamTelemetry";   // change with your project name
const char* password            = "esp32cam";            // be sure to have at least 8 characters otherwise the wifi do not start

const int timeDelay             = 0.;

//    (ROLL) 
float PID_P_GAIN_ROLL           = 0.;
float PID_I_GAIN_ROLL           = 0.;
float PID_D_GAIN_ROLL           = 0.;
                                            
//    (PITCH)                                             
float PID_P_GAIN_PITCH          = 0.;
float PID_I_GAIN_PITCH          = 0.;
float PID_D_GAIN_PITCH          = 0.;
                                            
//    (YAW)                                             
float PID_P_GAIN_YAW            = 0.;
float PID_I_GAIN_YAW            = 0.;
float PID_D_GAIN_YAW            = 0.;

//    (GYROSCOPE)
float GYROSCOPE_ROLL_FILTER     = 0.;
float GYROSCOPE_ROLL_CORR       = 0.;
float GYROSCOPE_PITCH_CORR      = 0.;

//    (ALTITUDE)                                      
float PID_P_GAIN_ALTITUDE       = 0.;
float PID_I_GAIN_ALTITUDE       = 0.;
float PID_D_GAIN_ALTITUDE       = 0.;

//    (TELEMETRY)
float rollAngle                 = 1.;
float pitchAngle                = 1.;
int flightMode                  = 1;
float batteryPercentage         = 1.;
float altitudeMeasure           = 1.;

//    (RECEIVER TRIMS)
float rollTrim                  = 1;
float pitchTrim                 = 1;
float yawTrim                   = 1;
float throttleTrim              = 1;

//    (TEMPERATURE)
float temperature               = 1;

//    (GPS TELEM.)
float latitude                  = 0.; // @todo = NULL;
float longitude                 = 0.; // @todo = NULL;
const char* timeUTC             = "";


void setup() {

  // Serial.begin(115200);
  beginUARTCOM();

  // setup the SD card
  setupSD();


  // setup camera settings
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  

  // if PSRAM IC present, init with UXGA resolution and higher JPEG quality
  //                      for larger pre-allocated frame buffer.
  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }


  // camera initialization
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    return;
  }

  sensor_t * s = esp_camera_sensor_get();
  // initial sensors are flipped vertically and colors are a bit saturated
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1); // flip it back
    s->set_brightness(s, 1); // up the brightness just a bit
    s->set_saturation(s, -2); // lower the saturation
  }

  // drop down frame size for higher initial frame rate
  s->set_framesize(s, FRAMESIZE_QVGA);
  

  // begin wifi AP
  WiFi.softAP(ssid, password);


  // start camera web server
  startCameraServer();


  // wait DroneIno to start sending data
  while( abs(rollTrim - 1) < 1e-10 && abs(pitchTrim - 1) < 1e-10 && abs(yawTrim - 1) < 1e-10 && abs(throttleTrim - 1) < 1e-10){
    readDataTransfer();
    delay(10);
  }


  // initialize PID and sent back it to DroneIno
  readConfigFile(SD_MMC);

}

void loop() {

  // read the incoming message from DroneIno
  readDataTransfer();

  // update flightData
  switch (isConnectedSD){                                  // check if SD in the previous loop result connected
  
    case 1:                                                // if is connected, then update the data log file in the SD
      writeDataLogFlight(SD_MMC);
      /**
       * @bug no detection if the SD is disconnected  
       * 
      if(!SD_MMC.begin("/sdcard", true)) isConnectedSD=0;  // check if the connection is lost
       */
      break;
    
    default:                                               // else, check if (in the current loop) the SD is connected
      setupSD();                                           // setup the SD card
      readConfigFile(SD_MMC);                              // initialize PID and sent back it to DroneIno
      break;

  }
  

  delay(timeDelay);
}