#include "esp_camera.h"
#include <WiFi.h>
#include <Arduino.h>
#include <ArduinoWebsockets.h>
#include <ArduinoJson.h>

/*
  All Serial.print is commented out so the bot doesn't go crazy with GPIO 1 TX activity
*/

// specific to my board yours might be different if
// not using the board listed in the BOM
#define CAMERA_MODEL_AI_THINKER
#include "camera_pins.h"

// network creds and server info
const char *ssid = "Wu Tang LAN";
const char *password = "SloppyBiscuits";
const char *websocket_server_host = "192.168.0.18";
const uint16_t websocket_server_port = 5000;

// create buffer to receive data comming from other ws clients
StaticJsonDocument<255> jsonBuffer;

// init the websocket client that connects to the ws server
using namespace websockets;
WebsocketsClient client;

// quick and dirty for now
// needs refactor and PWM for movement
void forward()
{
  digitalWrite(12, HIGH);
  digitalWrite(13, LOW);
  digitalWrite(15, HIGH);
  digitalWrite(14, LOW);
}
void reverse()
{
  digitalWrite(12, LOW);
  digitalWrite(13, HIGH);
  digitalWrite(15, LOW);
  digitalWrite(14, HIGH);
}
void hault()
{
  digitalWrite(12, LOW);
  digitalWrite(13, LOW);
  digitalWrite(15, LOW);
  digitalWrite(14, LOW);
}
void left()
{
  digitalWrite(12, LOW);
  digitalWrite(13, HIGH);
  digitalWrite(15, HIGH);
  digitalWrite(14, LOW);
}
void right()
{
  digitalWrite(12, HIGH);
  digitalWrite(13, LOW);
  digitalWrite(15, LOW);
  digitalWrite(14, HIGH);
}
void a_button_on()
{
  digitalWrite(2, HIGH);
}
void a_button_off()
{
  digitalWrite(2, LOW);
}
void b_button_on()
{
  digitalWrite(4, HIGH);
}
void b_button_off()
{
  digitalWrite(4, LOW);
}
void y_button_on()
{
  digitalWrite(1, HIGH);
}
void y_button_off()
{
  digitalWrite(1, LOW);
}

void setup()
{
  Serial.begin(115200);
  // Serial.setDebugOutput(true);
  // Serial.println();

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

  // init with high specs to pre-allocate larger buffers
  config.frame_size = FRAMESIZE_VGA;
  config.jpeg_quality = 40;
  config.fb_count = 2;

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK)
  {
    // Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  // connect to wifi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    // Serial.print(".");
  }
  // Serial.println("");
  // Serial.println("WiFi connected");

  // connect to websocket
  // Serial.println("Attempting WebSocket Connection");
  while (!client.connect(websocket_server_host, websocket_server_port, "/"))
  {
    delay(500);
    // Serial.print(".");
  }
  // Serial.println("Websocket Connected!");

  int res = 0;
  sensor_t *s = esp_camera_sensor_get();
  res = s->set_framesize(s, (framesize_t)9);

  // handle json data from web interface
  client.onMessage([](WebsocketsMessage msg)
                   {
    String raw_data = msg.data();
    handle_json(raw_data); });

  pinMode(12, OUTPUT); // IN1 MA
  pinMode(13, OUTPUT); // IN2 MA
  pinMode(15, OUTPUT); // IN3 MB
  pinMode(14, OUTPUT); // IN4 MB
  pinMode(2, OUTPUT);  // A
  pinMode(4, OUTPUT);  // B
  pinMode(1, OUTPUT);  // Y

  // set everything low so the bot doesn't go crazy on startup
  digitalWrite(12, LOW);
  digitalWrite(13, LOW);
  digitalWrite(15, LOW);
  digitalWrite(14, LOW);
  digitalWrite(2, LOW);
  digitalWrite(4, LOW);
  digitalWrite(1, LOW);
}

// handle incoming websocket messages
void handle_json(String raw_data)
{
  DeserializationError error = deserializeJson(jsonBuffer, raw_data);
  if (error)
  {
    // Serial.print(F("deserializeJson() failed: "));
    // Serial.println(error.f_str());
    return;
  }

  // quick and dirty for now
  // needs refactor
  const char *message = jsonBuffer["message"];

  // check message sent and deligate functions to execute
  if (strcmp(message, "forward") == 0)
  {
    forward();
  }
  if (strcmp(message, "reverse") == 0)
  {
    reverse();
  }
  if (strcmp(message, "hault") == 0)
  {
    hault();
  }
  if (strcmp(message, "left") == 0)
  {
    left();
  }
  if (strcmp(message, "right") == 0)
  {
    right();
  }

  if (strcmp(message, "AON") == 0)
  {
    a_button_on();
  }
  if (strcmp(message, "AOFF") == 0)
  {
    a_button_off();
  }
  if (strcmp(message, "BON") == 0)
  {
    b_button_on();
  }
  if (strcmp(message, "BOFF") == 0)
  {
    b_button_off();
  }
  if (strcmp(message, "YOFF") == 0)
  {
    y_button_on();
  }
  if (strcmp(message, "YOFF") == 0)
  {
    y_button_off();
  }
}

void loop()
{
  // check for any incoming messages
  client.poll();

  // get new image from camera
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb)
  {
    // Serial.println("Camera capture failed");
    esp_camera_fb_return(fb);
    return;
  }

  // check it's in jpeg, needed I think for the raw blob binary?
  if (fb->format != PIXFORMAT_JPEG)
  {
    // Serial.println("Non-JPEG data not implemented");
    return;
  }

  // send to websocket
  client.sendBinary((const char *)fb->buf, fb->len);
  esp_camera_fb_return(fb);
}
