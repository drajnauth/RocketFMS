

/*
 ESP-12 Blink
 by m0xpd

 Sign of life test for the
 'DDS on the Internet of Things'
 Board

 The internal blue LED on the ESP-12 Module
 is connected to GPIO2.
 It is wired active LOW
 (i.e. setting digital output 2 low lights the LED)

*/

#include "main.h"

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>  // esp_now does not exist for 8266
#include <stdint.h>

#include "UartFunctions.h"

extern char commands[MAX_COMMAND_ENTRIES];
extern uint8_t command_entries;
extern uint32_t numbers[MAX_COMMAND_ENTRIES];
extern char prompt[6];
extern char ovflmsg[9];
extern char errmsg[4];
char espnowBuf[300];

void setup() {
  pinMode(2, OUTPUT);  // Initialize the LED pin as an output
  Serial.begin(115200);

  Serial.println("Rocket FMS ESPNOW BaseStation v1 (VE3OOI)");

  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    while (1);
  }

  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
  esp_now_register_recv_cb(OnDataRecv);
  esp_now_register_send_cb(OnDataSent);

  Reset();

  Serial.println("\r\n");
  Serial.write(prompt);
}

// the loop function runs forever
void loop() { ProcessSerial(); }

void ExecuteSerial(char *str) {
  // This function called when serial input in present in the serial buffer
  // The serial buffer is parsed and characters and numbers are scraped and
  // entered in the commands[] and numbers[] variables.
  ParseSerial(str);

  // Process the commands
  // Note: Whenever a parameter is stated as [CLK] the square brackets are not
  // entered. The square brackets means that this is a command line parameter
  // entered after the command. E.g. F [n] [m] would be mean "F 0 7000000" is
  // entered (no square brackets entered)

  ESPNowBroadcast(str);

  switch (commands[0]) {
    case 'R':  // Reset
      Reset();
      break;
  }
}

void ESPNowBroadcast(char *message)
// Emulates a broadcast
{
  // Broadcast a message to every device in range
  // E8:6B:EA:D4:82:7C
  uint8_t broadcastAddress[] = {0xE8, 0x6B, 0xEA, 0xD4, 0x82, 0x7C};
  // uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

  if (!esp_now_is_peer_exist(broadcastAddress)) {
    esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
  }
  esp_now_send(broadcastAddress, (uint8_t *)message, strlen(message));
}

void Reset(void) {
  ResetSerial();
  Serial.println("ESP8266 Testing");
  Serial.print("ESP8266 Board MAC Address:  ");
  Serial.println(WiFi.macAddress());
}

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {}

// Callback function that will be executed when data is received
void OnDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len) {
  memset(espnowBuf, 0, sizeof(espnowBuf));
  if (len > sizeof(espnowBuf)) {
    Serial.println("ESPNOW Buffer Ovfl");
    Serial.printf("Message recv len: %d\r\n", len);
    return;
  }
  memcpy(espnowBuf, (char *)incomingData, len);
  if (strstr(espnowBuf, ESPNOW_CONNECTED_ID)) {
    Serial.println("ESPNOW connect request received");
    ESPNowBroadcast(ESPNOW_CONNECTED_ID);
  } else if (espnowBuf[0] > 0) {
    Serial.print(">> ");
    Serial.print((char *)espnowBuf);
  }
}
