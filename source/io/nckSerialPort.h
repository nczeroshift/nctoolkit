
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_SERIAL_PORT_H
#define NCK_SERIAL_PORT_H

#include "nckIOConfig.h"

#include <string>
#include <vector>

_IO_BEGIN

enum BaudRate{
	BAUDRATE_9600 = 9600,
	BAUDRATE_19200 = 19200,
	BAUDRATE_38400 = 38400,
	BAUDRATE_57600 = 57600,
	BAUDRATE_115200 = 115200,
};

class SerialCallback{
public:
	virtual ~SerialCallback(){};
	virtual void ReceiveData(unsigned char * data,unsigned int size) = 0;
};

class Serial{
public:
	virtual ~Serial(){};

	virtual unsigned int SendData(unsigned char * data,unsigned int size) = 0;

	virtual void SetCallback(SerialCallback * callback) = 0;
	virtual SerialCallback * GetCallback() = 0;

	virtual void Start() = 0;
	virtual void Stop() = 0;
};

std::vector<std::string> ListSerialPorts();
Serial * OpenSerial(const std::string & portName,BaudRate baudrate);

_IO_END

#endif // #ifndef NCK_SERIAL_PORT_H


