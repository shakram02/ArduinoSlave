//
//
//

#include <Wire\Wire.h>
#define I2C_BUFFER_MAX_LEN 32
#define MSG_HEADER_SIZE 4
#define DEBUG 0

void setup() {

	Serial.begin(9600);
	initI2cSlave(4);
}

// the loop function runs over and over again until power down or reset
void loop() {
}