/*
   WebSocketClient.ino

    Created on: 24.05.2015
    Aurther Ben Conway
    Version v1
 */
extern "C" {
#include "osapi.h"
#include "user_interface.h"
}

#include <Adafruit_BNO055.h>
#include <Adafruit_Sensor.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <Hash.h>
#include <WebSocketsClient.h>

#define LED = 2;
#define ABS_IMU_SAMPLERATE_DELAY_MS (100)
#define MPU_9150_delay (10) // millis between samples 1/freq * 1000
#define ABS_IMU_OUT true
#define IMU_OUT false

// Update these with values suitable for your network.
/*const char *_ssid = "CPT Sensors";
const char *_password = "crossword";
const char *_server = "192.168.0.102";
*/
const char *_ssid = "W1F1";
const char *_password = "B3NR1CHJ0RD4N14N";
const char *_server = "192.168.0.21";

uint16_t _port = 81;
String data;

WiFiClient espClient;
WebSocketsClient webSocket;
Adafruit_BNO055 bno = Adafruit_BNO055(); // Init Sensor

os_timer_t ABS_IMU_timer;
volatile bool socket_connected = false;
void ABS_IMU();
/*
██ ███    ██ ████████ ███████ ██████  ██████  ██    ██ ██████  ████████
██ ████   ██    ██    ██      ██   ██ ██   ██ ██    ██ ██   ██    ██
██ ██ ██  ██    ██    █████   ██████  ██████  ██    ██ ██████     ██
██ ██  ██ ██    ██    ██      ██   ██ ██   ██ ██    ██ ██         ██
██ ██   ████    ██    ███████ ██   ██ ██   ██  ██████  ██         ██
*/
/**
 * [timerCallback description]
 * @param pArg [description]
 */
void ABS_IMU_timerCB(void *pArg) { ABS_IMU(); }
/*
 █████  ██████  ███████     ██ ███    ███ ██    ██
██   ██ ██   ██ ██          ██ ████  ████ ██    ██
███████ ██████  ███████     ██ ██ ████ ██ ██    ██
██   ██ ██   ██      ██     ██ ██  ██  ██ ██    ██
██   ██ ██████  ███████     ██ ██      ██  ██████
*/
/**
 * [ABS_IMU description]
 */
void ABS_IMU(void) {
  // get Quaternion
  sensors_event_t event;
  bno.getEvent(&event);
  float qy = event.orientation.y;
  char buf[10];
  dtostrf(qy, 5, 2, buf);
  data = "{\"Euler\":{";
  data += "\"y\":";
  data += buf;
  data += "}";
  webSocket.sendTXT(data);
  Serial.println("sent..");
}

/*
██     ██ ███████ ██████  ███████  ██████   ██████ ██   ██ ███████ ████████
██     ██ ██      ██   ██ ██      ██    ██ ██      ██  ██  ██         ██
██  █  ██ █████   ██████  ███████ ██    ██ ██      █████   █████      ██
██ ███ ██ ██      ██   ██      ██ ██    ██ ██      ██  ██  ██         ██
 ███ ███  ███████ ██████  ███████  ██████   ██████ ██   ██ ███████    ██
*/
/**
 * [webSocketEvent description]
 * @param type    [description]
 * @param payload [description]
 * @param length  [description]
 */
void webSocketEvent(WStype_t type, uint8_t *payload, size_t length) {

  switch (type) {
  case WStype_DISCONNECTED:
    Serial.printf("[WSc] Disconnected!\n");
    socket_connected = false;
    break;
  case WStype_CONNECTED:
    Serial.printf("[WSc] Connected to url: %s\n", payload);
    // Serial.printf("WS:   client [%i] connected from %d.%d.%d.%d url: %s\n",
    // num, ip[0], ip[1], ip[2], ip[3], payload
    //);
    // send message to server when Connected
    socket_connected = true;
    break;
  case WStype_TEXT:
    Serial.printf("[WSc] get text: %s\n", payload);
    // send message to server
    // webSocket.sendTXT("message here");
    break;
  case WStype_BIN:
    Serial.printf("[WSc] get binary length: %u\n", length);
    hexdump(payload, length);
    // send data to server
    // webSocket.sendBIN(payload, length);
    break;
  default:
    Serial.printf("WS:   unhandled event type: %i\n", type);
    break;
  }
}
/*
   ██     ██ ██ ███████ ██
   ██     ██ ██ ██      ██
   ██  █  ██ ██ █████   ██
   ██ ███ ██ ██ ██      ██
   ███   ███ ██ ██      ██
 */
/**
 * [setup_wifi description]
 */
void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(_ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(_ssid, _password);
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  // wifi_set_sleep_type(NONE_SLEEP_T);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    // ESP.restart();
    // WiFi.begin(_ssid, _password);
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  delay(100);
}
/*
   ██████  ███    ██  ██████   ██████  ███████ ███████
   ██   ██ ████   ██ ██    ██ ██  ████ ██      ██
   ██████  ██ ██  ██ ██    ██ ██ ██ ██ ███████ ███████
   ██   ██ ██  ██ ██ ██    ██ ████  ██      ██      ██
   ██████  ██   ████  ██████   ██████  ███████ ███████
 */
/**
 * [setup_BNO055 description]
 */
void setup_BNO055() {
  // Check if sensor started
  if (!bno.begin()) {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print(
        "Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    // while (1);
  }
  delay(100);
  bno.setExtCrystalUse(true);
}

/*
███████ ███████ ████████ ██    ██ ██████
██      ██         ██    ██    ██ ██   ██
███████ █████      ██    ██    ██ ██████
     ██ ██         ██    ██    ██ ██
███████ ███████    ██     ██████  ██
*/
/**
 * [setup description]
 */
void setup() {
  Serial.begin(460800);
  // Serial.setDebugOutput(true);
  Serial.println();
  Serial.println();
  Serial.println();
  system_update_cpu_freq(80);
  // os_update_cpu_frequency(160);
  os_timer_setfn(&ABS_IMU_timer, ABS_IMU_timerCB, NULL);
  /*for(uint8_t t = 4; t > 0; t--) {
      Serial.printf("[SETUP] BOOT WAIT %d...\n", t);
      Serial.flush();
      delay(1000);
     }*/
  if (ABS_IMU_OUT)
    setup_BNO055();
  // if (IMU_OUT) setup_MPU_9150();
  setup_wifi();
  webSocket.begin(_server, _port);
  // webSocket.setAuthorization("user", "Password"); // HTTP Basic Authorization
  webSocket.onEvent(webSocketEvent);
  // lastMsg = 0;
  delay(500);
  os_timer_arm(&ABS_IMU_timer, ABS_IMU_SAMPLERATE_DELAY_MS, true);
  Serial.println("Boot Done...");
}
/*
   ██       ██████   ██████  ██████
   ██      ██    ██ ██    ██ ██   ██
   ██      ██    ██ ██    ██ ██████
   ██      ██    ██ ██    ██ ██
   ███████  ██████   ██████  ██
 */
/**
 * [loop description]
 */
void loop() { yield(); }
