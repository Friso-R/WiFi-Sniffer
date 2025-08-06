#include <Arduino.h>
#include <WiFi.h>
extern "C" {
  #include "esp_wifi.h"
}

int buttonPin = 2;
void buttonPress();

uint8_t targetMAC[6] = {0x14, 0x4F, 0x8A, 0xAA, 0x4C, 0xCC}; // Your server's MAC address

void snifferCallback(void* buf, wifi_promiscuous_pkt_type_t type) {
  if (type != WIFI_PKT_DATA) return;

  wifi_promiscuous_pkt_t *pkt = (wifi_promiscuous_pkt_t*)buf;
  uint8_t *dest = pkt->payload + 4; // destination MAC is at offset 4

  if (memcmp(dest, targetMAC, 6) == 0) {
    Serial.println("Packet destined for server detected!");

    buttonPress();
  }
}

void setup() {
  Serial.begin(9600);
  delay(2000);

  WiFi.mode(WIFI_MODE_STA);
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_rx_cb(&snifferCallback);
  esp_wifi_set_promiscuous_filter(nullptr);
  esp_wifi_set_channel(6, WIFI_SECOND_CHAN_NONE);
}

void loop() {
  delay(1000);
}

void buttonPress() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500); 
  digitalWrite(LED_BUILTIN, LOW);
}