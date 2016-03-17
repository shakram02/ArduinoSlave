//
//
//

#include <Wire\Wire.h>
#define I2C_BUFFER_MAX_LEN 32
#define DATA_LIST_MAX_COUNT 32
#define MSG_HEADER_SIZE 4
#define DEBUG 0
#define DEBUG1 0

enum Type : uint8_t
{

	ArduinoLong,
	ArduinoNegativeLong,
	ArduinoShort,
	ArduinoNegativeShort,
	ArduinoString,
	ArduinoByte,
};
typedef struct I2cData
{
	Type typeOfData;
	uint8_t sizeInBytes;	// Max 32 bytes ( I2c buffer size )
	void* data;
}I2cData;


void setup() {

	Serial.begin(9600);
	initI2cSlave(4);
}

// the loop function runs over and over again until power down or reset
void loop() {
}