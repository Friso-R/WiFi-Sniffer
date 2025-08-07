#include <Arduino.h>
#include <WiFi.h>
extern "C" {
  #include "esp_wifi.h"
}

uint8_t channel   = 6;
uint8_t buttonPin = 22;
void buttonPress();

uint8_t targetMAC[6] = {0x14, 0x4F, 0x8A, 0xAA, 0x4C, 0xCC}; // Your server's MAC address

void snifferCallback(void* buf, wifi_promiscuous_pkt_type_t type) {
  if (type != WIFI_PKT_DATA) return;

  wifi_promiscuous_pkt_t *pkt = (wifi_promiscuous_pkt_t*)buf;
  uint8_t* payload = pkt->payload;
  if ((payload[1] & 0x03) != 0x02) return;

  uint8_t *dest = payload + 4; // destination MAC is at offset 4

  Serial.println("Packet destined for server detected!");
  buttonPress();
  /*
  if (memcmp(dest, targetMAC, 6) == 0) {
    // Extract Frame Control field's second byte (flags)
    uint8_t flags = payload[1];

    // ToDS = bit 0, FromDS = bit 1
    bool toDS = flags & 0x01;
    bool fromDS = flags & 0x02;

    Serial.println("Packet destined for server detected!");
    Serial.print("ToDS: ");
    Serial.println(toDS);
    Serial.print("FromDS: ");
    Serial.println(fromDS);

    buttonPress();
  }
  */
}

void setup() {
  Serial.begin(9600);
  delay(2000);
  pinMode(buttonPin, OUTPUT);

  WiFi.mode(WIFI_MODE_STA);
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_rx_cb(&snifferCallback);
  
  wifi_promiscuous_filter_t filter = { .filter_mask = WIFI_PROMIS_FILTER_MASK_DATA };
  esp_wifi_set_promiscuous_filter(&filter);

  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
}

void loop() {
  delay(1000);
}

void buttonPress() {
  digitalWrite(buttonPin, HIGH);
  delay(500); 
  digitalWrite(buttonPin, LOW);
}

/*
| ToDS | FromDS | Addr1           | Addr2           | Addr3           | Addr4      | Scenario              |
| ---- | ------ | --------------- | --------------- | --------------- | ---------- | --------------------- |
| 0    | 0      | Destination MAC | Source MAC      | BSSID           | —          | Ad-hoc (peer-to-peer) |
| 0    | 1      | Destination MAC | BSSID           | Source MAC      | —          | From AP → Client      |
| 1    | 0      | BSSID           | Source MAC      | Destination MAC | —          | From Client → AP      |
| 1    | 1      | Receiver MAC    | Transmitter MAC | Destination MAC | Source MAC | WDS (AP ↔ AP)         |
*/