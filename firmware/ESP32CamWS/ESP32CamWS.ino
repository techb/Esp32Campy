#include "esp_camera.h"
#include <WiFi.h>
#include <Arduino.h>
#include <ArduinoWebsockets.h>
#include <ArduinoJson.h>

// specific to my board yours might be different if
// not using the board listed in the BOM
#define CAMERA_MODEL_AI_THINKER
#include "camera_pins.h"

// network creds and server info
const char* ssid = "Wu Tang LAN";
const char* password = "ILoveCheeseSticks";
const char* websocket_server_host = "192.168.0.13";
const uint16_t websocket_server_port = 5000;

// create buffer to receive data comming from other ws clients
StaticJsonDocument<255> jsonBuffer;

// init the websocket client that connects to the ws server
using namespace websockets;
WebsocketsClient client;


void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  pinMode(15, OUTPUT);

  // set camera pins and settings
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
  config.xclk_freq_hz = 10000000;
  config.pixel_format = PIXFORMAT_JPEG;

  //init with high specs to pre-allocate larger buffers
  if(psramFound()) {
    config.frame_size = FRAMESIZE_VGA;
    config.jpeg_quality = 40;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  // connect to wifi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // connect to websocket
  Serial.println("Attempting WebSocket Connection");
  while(!client.connect(websocket_server_host, websocket_server_port, "/")) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Websocket Connected!");

  // handle json data from web interface
  client.onMessage([](WebsocketsMessage msg) {
    String raw_data = msg.data();
    handle_json(raw_data);
    // DeserializationError error = deserializeJson(jsonBuffer, raw_data);
    // if (error) {
    //  Serial.print(F("deserializeJson() failed: "));
    //  Serial.println(error.f_str());
    //  return;
    // }

    // const char* message = jsonBuffer["message"];
    // Serial.println(message);
    // digitalWrite(15, HIGH);
  });
}


void handle_json(String raw_data) {
  DeserializationError error = deserializeJson(jsonBuffer, raw_data);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  const char* message = jsonBuffer["message"];
  Serial.println(message);
}


void loop() {
  client.poll();
  // get new image from camera
  camera_fb_t *fb = esp_camera_fb_get();
  if(!fb) {
    Serial.println("Camera capture failed");
    esp_camera_fb_return(fb);
    return;
  }

  // check it's in jpeg, needed I think for the raw blob binary?
  if(fb->format != PIXFORMAT_JPEG) {
    Serial.println("Non-JPEG data not implemented");
    return;
  }

  // send to websocket
  client.sendBinary((const char*) fb->buf, fb->len);
  esp_camera_fb_return(fb);
}
