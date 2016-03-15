//
//
//

#include<Wire\Wire.h>
#define I2C_BUFFER_MAX_LEN 32
#define DEBUG 1

void setup() {
	Serial.begin(9600);
	initI2cSlave(4);
}

// the loop function runs over and over again until power down or reset
void loop() {
}

///<summary>Initializes the I2C communication bus</summary>
///<param name ='address'>Address for I2C communication</param>
void initI2cSlave(int address)
{
	// Flip the sign if the address is negative
	address < 0 ? address *= -1 : address;

	Wire.begin(address);
	Wire.onReceive(readDirectBytes);
}

void readDirectBytes(int messageSize)
{
	Serial.println();
	Serial.print(messageSize);

#ifdef DEBUG
	Serial.println(" Got msg");
#endif

	// Ignore large messages
	if (messageSize > I2C_BUFFER_MAX_LEN)
	{
#ifdef DEBUG
		Serial.println("Message exceeded size limit");
#endif
		return;
	}

	char* buf = (char*)malloc(sizeof(char) * messageSize);
	char* token;
	Wire.readBytes(buf, messageSize);
	printString(buf, messageSize);

	// Get the first token of the string
	token = strtok(buf, "-");

#if DEBUG
	Serial.print("token: \n");
	printString(token, 3);
	Serial.print("\n\n--------\n");
#endif
	/*
	if Return value < 0 then it indicates str1 is less than str2.
	if Return value > 0 then it indicates str2 is less than str1.
	if Return value = 0 then it indicates str1 is equal to str2.
	*/

	// See the category of the message
	if (!strcmp(token, "str"))
	{
		Serial.println("Got string msg\nresult:");

		// Get the second token of the string
		token = strtok(NULL, buf);
	}
	else if (!strcmp(token, "shr"))
	{
		Serial.println("Got positive short\nresult:");

		// Get the second token of the string
		token = strtok(NULL, buf);
		readShortI2c(token);
	}
	else if (!strcmp(token, "nsh"))
	{
		Serial.println("Got negative short\nresult:");

		// Get the second token of the string
		token = strtok(NULL, buf);
		Serial.println((int)(readShortI2c(token)*-1));
	}

	// Release the buffer after getting all tokens
	free(buf);
}

void printString(char* str, int messageSize)
{
	for (size_t i = 0; i < messageSize; i++)
	{
		char cuurent = str[i];
		Serial.print((byte)cuurent);
		Serial.print("    char:");
		Serial.print(cuurent);
		Serial.println();
	}
}

unsigned int readShortI2c(char* token)
{
	unsigned int result = 0;

	result |= (byte)token[0];	// LSB
	result |= ((unsigned int)((byte)token[1] << 8));	// MSB
#if DEBUG
	Serial.print("Buff:");
	Serial.print((byte)token[0]);
	Serial.print(" ");
	Serial.print((byte)token[1]);
	Serial.print("   ");

#endif

	return result;
}