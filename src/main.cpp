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
#include <WiFi.h>

// Camera libraries
#include "esp_camera.h"

#define CAMERA_MODEL_AI_THINKER  // Has PSRAM

#include "app_httpd.h"
#include "camSD.h"
#include "camera_pins.h"
#include "telemetry.h"

//          (WIFI)
const char* ssid = "Esp32CamTelemetry";  // change with your project name
const char* password = "esp32cam";  // be sure to have at least 8 characters
                                    // otherwise the wifi do not start

const int timeDelay = 0.;

//    (ROLL)
float PID_P_GAIN_ROLL = 0.0f;
float PID_I_GAIN_ROLL = 0.0f;
float PID_D_GAIN_ROLL = 0.0f;

//    (PITCH)
float PID_P_GAIN_PITCH = 0.0f;
float PID_I_GAIN_PITCH = 0.0f;
float PID_D_GAIN_PITCH = 0.0f;

//    (YAW)
float PID_P_GAIN_YAW = 0.0f;
float PID_I_GAIN_YAW = 0.0f;
float PID_D_GAIN_YAW = 0.0f;

//    (GYROSCOPE)
float GYROSCOPE_ROLL_FILTER = 0.0f;
float GYROSCOPE_ROLL_CORR = 0.0f;
float GYROSCOPE_PITCH_CORR = 0.0f;

//    (ALTITUDE)
float PID_P_GAIN_ALTITUDE = 0.0f;
float PID_I_GAIN_ALTITUDE = 0.0f;
float PID_D_GAIN_ALTITUDE = 0.0f;

//    (TELEMETRY)
const char* rollAngle = "";
const char* pitchAngle = "";
const char* flightMode = "";
const char* batteryPercentage = "";
const char* altitudeMeasure = "";

//    (RECEIVER TRIMS)
const char* rollTrim = "";
const char* pitchTrim = "";
const char* yawTrim = "";
const char* throttleTrim = "";

//    (TEMPERATURE)
const char* temperature = "";

//    (GPS TELEM.)
const char* latitude = "";
const char* longitude = "";
const char* timeUTC = "";

//    (SELECTION LOOP VARIABLE)
uint8_t isConnectedSD = 0;
uint8_t isChangedPID = 0;

//    (LOOP TIMER)
unsigned long loopTimer;
unsigned long loopDuration = 50;  // ms

void setup() {
  // serial begin
  // Serial.begin(115200);
  beginUARTCOM();

  // pinMode(2, INPUT_PULLUP);

  // flash
  // pinMode(4, OUTPUT);

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
  if (psramFound()) {
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

  sensor_t* s = esp_camera_sensor_get();
  // initial sensors are flipped vertically and colors are a bit saturated
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1);        // flip it back
    s->set_brightness(s, 1);   // up the brightness just a bit
    s->set_saturation(s, -2);  // lower the saturation
  }

  // drop down frame size for higher initial frame rate
  s->set_framesize(s, FRAMESIZE_QVGA);

  // begin wifi AP
  WiFi.softAP(ssid, password);

  setupSD();

  // start camera web server
  startCameraServer();

  // wait DroneIno to start sending data
  while (strcmp(rollTrim, "") && strcmp(yawTrim, "") && strcmp(pitchTrim, "") &&
         strcmp(throttleTrim, "")) {
    readDataTransfer();
    delay(10);
  }

  // initialize PID and sent back it to DroneIno
  if (isConnectedSD) readConfigFile(SD_MMC);
}

void loop() {
  // get the initial timer
  loopTimer = millis();

  // read the incoming message from DroneIno
  readDataTransfer();

  // if the PID has been changed in the web app
  switch (isChangedPID) {
    case 1:

      // send back to DroneIno the pid parameters
      writeDataTransfer();

      // updates the config.txt on the SD card
      if (isConnectedSD) updateConfigFile(SD_MMC);

      isChangedPID = 0;
      break;

    default:
      break;
  }

  // wait until the timer expires
  while (millis() - loopTimer <= loopDuration)
    ;
}