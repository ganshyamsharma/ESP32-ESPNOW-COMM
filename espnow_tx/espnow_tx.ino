#define level_sw 4
#define test_pb 21
#define tank_full_led 2

#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>

uint8_t broadcastAddress[] = {0xA0, 0xB7, 0x65, 0x62, 0x43, 0x44};

bool tank_full = 0;

esp_now_peer_info_t peerInfo;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
 
void setup() {
  //Pull up level switch input
  pinMode(level_sw, INPUT_PULLUP);
  pinMode(test_pb, INPUT_PULLUP);
  pinMode(tank_full_led, OUTPUT);

  // Init Serial Monitor
  Serial.begin(115200);
 
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  esp_wifi_set_protocol(WIFI_IF_STA, WIFI_PROTOCOL_LR);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(esp_now_send_cb_t(OnDataSent));
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 1;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}
 
void loop() {
  // Set values to send
  if(digitalRead(level_sw) & digitalRead(test_pb) == HIGH) {
	tank_full = 0;
  digitalWrite(tank_full_led, LOW);  
  }
  else {
	tank_full = 1;  
  digitalWrite(tank_full_led, HIGH);
  }
  
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &tank_full, sizeof(tank_full));
   
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
  delay(1000);
}