#include <WiFi.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include "unit_rolleri2c.hpp"
UnitRollerI2C RollerI2C;
const char* WIFI_SSID = "ChaiHuo_MeetingRoom_2.4G";
const char* WIFI_PASS = "make0314";
const int OSC_PORT = 8000;

WiFiUDP Udp;
uint8_t buf[512];


void setup() {
  
  Serial.begin(9600);
  // WiFi Setup
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("WiFi connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected. IP: ");
  Serial.println(WiFi.localIP());
  // OSC UDP
  Udp.begin(OSC_PORT);

  // Motor Setup
  RollerI2C.begin(&Wire, 0x64, 5, 6, 400000);
  RollerI2C.setOutput(0);
  RollerI2C.setMode(ROLLER_MODE_POSITION);
  RollerI2C.setPosMaxCurrent(100000);
  RollerI2C.setOutput(1);
}

void loop() {
  int n = Udp.parsePacket();
  if (n <= 0) return;

  int len = Udp.read(buf, (n > (int)sizeof(buf)) ? (int)sizeof(buf) : n);
  if (len <= 0) return;

  OSCMessage msg;
  msg.fill(buf, len);
  if (msg.hasError()) return;

  if (msg.fullMatch("/deg/") && msg.size() > 0) {
    float deg;
    if (msg.isFloat(0)) deg = msg.getFloat(0);
    else if (msg.isInt(0)) deg = (float)msg.getInt(0);
    else return;
    RollerI2C.setPos((int32_t)(deg * 100.0f));
  }
}
