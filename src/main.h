#ifndef _MAIN_H_
#define _MAIN_H_

#include <stdint.h>

#define ESPNOW_CONNECTED_ID (char *)"$$$$"

void ExecuteSerial(char *str);
void Reset(void);
void ESPNowBroadcast(char *message);
void OnDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len);
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus);

#endif  // _MAIN_H_