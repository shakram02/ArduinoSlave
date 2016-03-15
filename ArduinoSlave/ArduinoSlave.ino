//
//
//

#include<Wire\Wire.h>
#define I2C_BUFFER_MAX_LEN 32
#define MSG_HEADER_SIZE 4
#define DEBUG 1

void setup() {
#if DEBUG
	Serial.begin(9600);
#endif
	initI2cSlave(4);
}

// the loop function runs over and over again until power down or reset
void loop() {
}

///<summary>Initializes the I2C communication bus, converts address to positive
/// if a negative address is provided </summary>
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
	// Ignore large messages
	if (messageSize > I2C_BUFFER_MAX_LEN)
	{
#ifdef DEBUG
		Serial.println("Message exceeded size limit");
#endif
		return;
	}

	char* token;	// Tokens of the message
	char* buf = (char*)malloc(sizeof(char) * messageSize);
	char* msg = (char*)malloc(sizeof(char) * (messageSize - MSG_HEADER_SIZE));

	Wire.readBytes(buf, messageSize);

	Serial.print("Message size:");
	Serial.print(messageSize);
	Serial.println();

	Serial.print("Full Message\n");
	printString(buf, messageSize);
	Serial.print("\n");

	// Get the first token of the string
	token = strtok(buf, "-");

	for (short i = MSG_HEADER_SIZE; i < messageSize; i++)
	{
		msg[i - MSG_HEADER_SIZE] = buf[i];
	}
	Serial.print(messageSize - MSG_HEADER_SIZE);
	Serial.println("Message content:");
	printString(msg, (messageSize - MSG_HEADER_SIZE));

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
	}
	else if (!strcmp(token, "shr"))
	{
		Serial.println("Got positive short\nresult:");

		// Get the second token of the string

		readShortI2c(msg);
	}
	else if (!strcmp(token, "nsh"))
	{
		Serial.println("Got negative short\nresult:");

		// Get the second token of the string

		Serial.println((int)(readShortI2c(msg)*-1));
	}
	else if (!strcmp(token, "int"))
	{
		// Positive C# integer ( 32-bits )
		Serial.println("Got positive int\n");
		printString(msg, (messageSize - MSG_HEADER_SIZE));
		readLongI2c(msg);
	}
	else if (!strcmp(token, "nin"))
	{
		// Negative C# integer ( 32-bits )
	}
	// Release the buffer after getting all tokens
	free(buf);
	free(msg);
}

void printString(char* str, int messageSize)
{
	for (size_t i = 0; i < messageSize; i++)
	{
		char curent = str[i];
		Serial.print((byte)curent);
		Serial.println();
	}
	Serial.flush();
}

unsigned int readShortI2c(char* token)
{
	unsigned int result = 0;

	result |= ((unsigned int)((byte)token[1] << 8));	// MSB
	result |= (byte)token[0];	// LSB

#if DEBUG
	Serial.print("Buff:");
	Serial.print((byte)token[0]);
	Serial.print(" ");
	Serial.print((byte)token[1]);
	Serial.print("   ");
	Serial.println(result);
#endif
	return result;
}

///<summary>This union is used to convert bytes to long</summary>
typedef union
{
	unsigned long result;
	byte data[4];
}Dummy;

///<summary>Reads an int 32bit value from Win IoT I2C into long</summary>
///<param name='inMsg'>Incoming message</param>
///<returns>Converted long</returns>
unsigned long readLongI2c(char* inMsg)
{
	unsigned long result = 0;

	// Convert to byte array pointer
	byte* msg = (byte*)inMsg;

	Dummy d;
	
	for (short i = 0; i < 4; i++)
	{
		// Copy data to the union
		d.data[i] = msg[i];
	}

	// Extract data from union
	Serial.println(d.result);
#if DEBUG
	//Serial.print("Buff:");
	//Serial.print((byte)token[0]);
	//Serial.print(" ");
	//Serial.print((byte)token[1]);
#endif
}