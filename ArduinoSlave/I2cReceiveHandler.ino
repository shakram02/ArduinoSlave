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

	char* token;	// Tokens of the message
	char* buf = (char*)malloc(sizeof(char) * messageSize);
	char* msg = (char*)malloc(sizeof(char) * (messageSize - MSG_HEADER_SIZE));

	I2cData* ret = (I2cData*)malloc(sizeof(I2cData));

	for (size_t i = 0; i < messageSize; i++)
	{
		buf[i] = Wire.read();
		Serial.print((byte)buf[i]);
		Serial.print(' ');
	}
	//Wire.readBytes(buf, messageSize);

	//Serial.print("Message size:");
	//Serial.print(messageSize);
	//Serial.println();

	//Serial.print("Full Message\n");
	//printString(buf, messageSize);
	//Serial.print("\n");

	// Get the first token of the string
	token = strtok(buf, "-");

	for (short i = MSG_HEADER_SIZE; i < messageSize; i++)
	{
		msg[i - MSG_HEADER_SIZE] = buf[i];
	}
	Serial.print(messageSize - MSG_HEADER_SIZE);

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

		int* shortRetPointer = (int*)malloc(sizeof(int));
		*shortRetPointer = readShortI2c(msg);
		ret->data = shortRetPointer;
		ret->typeOfData = ArduinoShort;

		Serial.print(*((short*)ret->data));
		Serial.println(" then Free resources");
		freeResources(buf, msg);

		return ret;
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
	else if (!strcmp(token, "byt"))
	{
		// Recieved a byte value
	}
	// Release the buffer after getting all tokens
	free(buf);
	free(msg);
}

void freeResources(char* resource0, char* resource1)
{
	free(resource0);
	free(resource1);
}

// Read C# short
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

	// Extract data from union
	Serial.println(longConverter.result);

#if DEBUG
	//Serial.print("Buff:");
	//Serial.print((byte)token[0]);
	//Serial.print(" ");
	//Serial.print((byte)token[1]);
#endif
}

void printString(char* str, int messageSize)
{
	for (size_t i = 0; i < messageSize; i++)
	{
		char curent = str[i];
		Serial.print((byte)curent);
		Serial.println();
	}
}