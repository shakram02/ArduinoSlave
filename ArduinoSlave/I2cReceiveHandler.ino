///<summary>Initializes the I2C communication bus, converts address to positive
/// if a negative address is provided </summary>
///<param name ='address'>Address for I2C communication</param>
void initI2cSlave(int address)
{
	// Flip the sign if the address is negative
	address < 0 ? address *= -1 : address;

	Wire.begin(address);
	Wire.onReceive(addToList);
}

void addToList(int messageSize)
{
	//Serial.println("r");

	void* readData = readDirectBytes(messageSize);

	// Bad message
	if (readData == NULL)
		return;

	Serial.println("Done reading message");
	I2cData convertedData = *((I2cData*)readData);
	Serial.print(*((short*)convertedData.data));
	Serial.println();
}

///<summary>Performs the actual data reading and parsing from the I2C bus</summary>
void* readDirectBytes(int messageSize)
{
	// Ignore large messages
	if (messageSize > I2C_BUFFER_MAX_LEN)
	{
#ifdef DEBUG
		Serial.println("Message exceeded size limit");
#endif
		return NULL;
	}

	char* token;	// Stores the type of the message
	char* messageBuffer = (char*)malloc(sizeof(char) * messageSize);	// Contains the full payload
	char* messageContent = (char*)malloc(sizeof(char) * (messageSize - MSG_HEADER_SIZE));	// Contains the data in the payload

	// Stores the return value of the function
	I2cData* ret = (I2cData*)malloc(sizeof(I2cData));

	// Size of the actual message in bytes
	ret->sizeInBytes = messageSize - MSG_HEADER_SIZE;

	// Read the payload
	for (size_t i = 0; i < messageSize; i++)
	{
		messageBuffer[i] = Wire.read();
		Serial.print((byte)messageBuffer[i]);
		Serial.print(' ');
	}

#if DEBUG1
	Serial.print("Message size:");
	Serial.print(messageSize);
	Serial.println();

	Serial.print("Full Message\n");
	printString(messageBuffer, messageSize);
	Serial.print("\n");
#endif

	// Get the first token of the string
	// Contains the type of payload content
	token = strtok(messageBuffer, "-");

	// Get the contents of the message
	for (short i = MSG_HEADER_SIZE; i < messageSize; i++)
	{
		messageContent[i - MSG_HEADER_SIZE] = messageBuffer[i];
	}

	/*
		The buffer and message content arrays will always be freed except for the case
		of having a string, so the freeing operation will occur before returning the
		value of the message only once.
	*/

	// See the category of the message
	if (!strcmp(token, "str"))
	{
		Serial.println("Got string msg:");

		// Return the message
		ret->data = messageContent;
		ret->typeOfData = ArduinoString;

		free(messageBuffer);

		return ret;
	}
	else if (!strcmp(token, "shr"))
	{
		Serial.println("Got positive short:");

		/* Note that the arduino short and int are equivalent in terms of size */

		int* shortRetPointer = (int*)malloc(sizeof(int));
		*shortRetPointer = readShortI2c(messageContent);
		ret->data = shortRetPointer;
		ret->typeOfData = ArduinoShort;

		//Serial.print(*((short*)ret->data));
	}
	else if (!strcmp(token, "nsh"))
	{
		Serial.println("Got negative short:");

		int* shortRetPointer = (int*)malloc(sizeof(int));
		*shortRetPointer = readShortI2c(messageContent)*-1;
		ret->data = shortRetPointer;
		ret->typeOfData = ArduinoNegativeShort;

		Serial.println(*shortRetPointer);
	}
	else if (!strcmp(token, "int"))
	{
		Serial.println("Got positive int:");

		/* Note that the int in C# is 32-bits which is equivalent to a long on Arduino */
		unsigned long* longRetPointer = (unsigned long*)malloc(sizeof(unsigned long));
		*longRetPointer = readLongI2c(messageContent);

		ret->data = longRetPointer;
		ret->typeOfData = ArduinoLong;
	}
	else if (!strcmp(token, "nin"))
	{
		Serial.println("Got negative int:");

		long* longRetPointer = (long*)malloc(sizeof(long));
		*longRetPointer = (long)(readLongI2c(messageContent) * -1);

		ret->data = longRetPointer;
		ret->typeOfData = ArduinoNegativeLong;
	}
	else if (!strcmp(token, "byt"))
	{
		// Recieved a byte value
		byte* byteRetPointer = (byte*)malloc(sizeof(byte));
		*byteRetPointer = messageContent[0];
		ret->data = byteRetPointer;
		ret->typeOfData = ArduinoByte;

		// Free message buffer only because the msgContent value is used in storing the incomming byte
		free(messageBuffer);

		return ret;
	}
	// Release the buffer after getting all tokens
	freeResources(messageBuffer, messageContent);

	return ret;
}

///<summary>De allocates memory used in reading I2C message</summary>
void freeResources(char* resource0, char* resource1)
{
	free(resource0);
	free(resource1);
}

// Read C# short
///<summary>Reads a 16bit numeric value</summary>
unsigned int readShortI2c(char* token)
{
	unsigned int result = 0;

	result |= ((unsigned int)((byte)token[1] << 8));	// MSB
	result |= (byte)token[0];	// LSB

#if DEBUG1
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
}ByteArrayToLongConverter;

///<summary>Reads an int 32bit value from Win IoT I2C into long</summary>
///<param name='inMsg'>Incoming message</param>
///<returns>Converted long</returns>
unsigned long readLongI2c(char* inMsg)
{
	unsigned long result = 0;

	// Convert to byte array pointer
	byte* msg = (byte*)inMsg;

	ByteArrayToLongConverter longConverter;

	for (short i = 0; i < 4; i++)
	{
		// Copy data to the union
		longConverter.data[i] = msg[i];
	}

#if DEBUG
	Serial.println(longConverter.result);
	//Serial.print("Buff:");
	//Serial.print((byte)token[0]);
	//Serial.print(" ");
	//Serial.print((byte)token[1]);
#endif

	// Extract data from union
	return longConverter.result;
}

///<summary>Prints a given string to the serial monitor</summary>
void printString(char* str, int messageSize)
{
	for (size_t i = 0; i < messageSize; i++)
	{
		char curent = str[i];
		Serial.print((byte)curent);
		Serial.println();
	}
}