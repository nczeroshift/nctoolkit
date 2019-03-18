
/**
 * NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckSerialPort.h"

#include <stdlib.h>
#include <stdio.h>
#include "nckException.h"
#include "nckThread.h"
#include "nckUtils.h"
#include <string.h>
#include "nckMathUtils.h"

#if defined(NCK_LINUX) || defined(NCK_MACOSX)
	#include <termios.h>
	#include <sys/ioctl.h>
	#include <unistd.h>
	#include <fcntl.h>
	#include <sys/types.h>
	#include <sys/stat.h>
 	#include <sys/signal.h>
	#include <limits.h>
#elif defined(NCK_WINDOWS)
	#include <windows.h>
#undef CreateMutex
#endif

_IO_BEGIN

#if defined(NCK_SERIAL_PORT)
class SerialPortDevice : public virtual Serial{
public:
	virtual ~SerialPortDevice(){
		m_TearDown = true;
		SafeDelete(m_Thread);
		if(commPort != NULL)
			CloseHandle(commPort);

#if defined(NCK_LINUX) || defined(NCK_MACOSX)
		tcsetattr(fd, TCSANOW,&previousSettings);

		if(fd)
			close(fd);
#endif
	}

	static void * ReadThread(void * data){
		SerialPortDevice * dev = (SerialPortDevice*)data;
		unsigned char tmpData[4096];
		int noDataCount = 0;
		while(!dev->m_TearDown)
		{
			unsigned int n = dev->ReadData(tmpData,4096);

			if(n == 0)
				noDataCount++;
			else{
				if(dev->m_Callback!=NULL)
					dev->m_Callback->ReceiveData(tmpData,n);

				noDataCount = 0;
			}

			if(noDataCount >= 50)
				Core::Thread::Wait(1);
		}

		return NULL;
	}

	SerialPortDevice()
	{
		m_Callback = NULL;
		fd = 0;
		m_Thread = Core::CreateThread(ReadThread,this);
		m_Mutex = Core::CreateMutex();

#ifdef NCK_WINDOWS
		commPort = NULL;
#endif
	}

	bool Open(const std::string & portName, BaudRate br)
	{
#if defined(NCK_LINUX) || defined(NCK_MACOSX)
		int baudr = B9600;

		switch(br){
		case BAUDRATE_9600:
			baudr = B9600;
			break;
		case BAUDRATE_19200:
			baudr = B19200;
			break;
		case BAUDRATE_38400:
			baudr = B38400;
			break;
		case BAUDRATE_57600:
			baudr = B57600;
			break;
		case BAUDRATE_115200:
			baudr = B115200;
			break;
		default:
			break;
		}

		fd = open(portName.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
		if(fd == -1)
		{
			THROW_EXCEPTION("Unable to open port");
		}

		int error = tcgetattr(fd, &previousSettings);

		if(error==-1)
		{
			close(fd);
			THROW_EXCEPTION("Unable to read port settings");
		}

		struct termios settings;

		memset(&settings, 0, sizeof(struct termios));

		settings.c_cflag = baudr | CS8 | CLOCAL | CREAD;
		settings.c_iflag = IGNPAR;
		settings.c_oflag = 0;
		settings.c_lflag = 0;
		settings.c_cc[VMIN] = 0;
		settings.c_cc[VTIME] = 0;

		error = tcsetattr(fd, TCSANOW, &settings);

		if(error==-1)
		{
			close(fd);
			THROW_EXCEPTION("Unable to set port settings");
		}


#else
		BOOL status;

        // Windows
		commPort = CreateFile(portName.c_str(),          // for COM1—COM9 only
			GENERIC_READ | GENERIC_WRITE, //Read/Write
			0,               // No Sharing
			NULL,            // No Security
			OPEN_EXISTING,   // Open existing port only
			0,               // Non Overlapped I/O
			NULL);

		if (commPort == INVALID_HANDLE_VALUE)
			THROW_EXCEPTION("Unable to open serial port");

		DCB dcbSerialParams = { 0 }; // Initializing DCB structure
		dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
		
		status = GetCommState(commPort, &dcbSerialParams);

		dcbSerialParams.BaudRate = CBR_9600;

		switch (br) {
			case BAUDRATE_19200:
				dcbSerialParams.BaudRate = CBR_19200;
				break;
			case BAUDRATE_38400:
				dcbSerialParams.BaudRate = CBR_38400;
				break;
			case BAUDRATE_57600:
				dcbSerialParams.BaudRate = CBR_57600;
				break;
			case BAUDRATE_115200:
				dcbSerialParams.BaudRate = CBR_115200;
				break;
			default:
				break;
		}

		dcbSerialParams.ByteSize = 8;         // Setting ByteSize = 8
		dcbSerialParams.StopBits = ONESTOPBIT;// Setting StopBits = 1
		dcbSerialParams.Parity = NOPARITY;  // Setting Parity = None
		
		SetCommState(commPort, &dcbSerialParams);

		COMMTIMEOUTS timeouts = { 0 };
		timeouts.ReadIntervalTimeout = 1;
		timeouts.ReadTotalTimeoutConstant = 1; 
		timeouts.ReadTotalTimeoutMultiplier = 0;
		timeouts.WriteTotalTimeoutConstant = 1; 
		timeouts.WriteTotalTimeoutMultiplier = 0;

		status = SetCommTimeouts(commPort,&timeouts);

		status = SetCommMask(commPort, EV_RXCHAR);

		return true;
#endif

	}

	unsigned int SendData(unsigned char * data,unsigned int size){
#if defined(NCK_LINUX) || defined(NCK_MACOSX)
		int n = (int)write(fd,data, size);

		if(n<0)
			return 0;
		else
			return n;
#else
		DWORD dNoOfBytesWritten = 0;  
		m_Mutex->Lock();
		BOOL status = WriteFile(commPort,      
			data,     
			size,  
			&dNoOfBytesWritten,
			NULL);
		m_Mutex->Unlock();
		if (!status || dNoOfBytesWritten != size)
			return 0;
		else
			return dNoOfBytesWritten;
#endif
	}

	unsigned int ReadData(unsigned char * data, int maxSize){
#if defined(NCK_LINUX) || defined(NCK_MACOSX)
		if(maxSize>4096)
			maxSize = 4096;
		int n = (int)read(fd, data, maxSize);
		if(n<0)
			return 0;
		return n;
#else
	
		DWORD dwEventMask;
		BOOL status;
		/*m_Mutex->Lock();
		BOOL status = WaitCommEvent(commPort, &dwEventMask, NULL);
		m_Mutex->Unlock();
		
		if ((dwEventMask & EV_RXCHAR) != EV_RXCHAR)
			return 0;*/

		m_Mutex->Lock();
		DWORD NoBytesRead;
		status = ReadFile(commPort,         
				data,  
				maxSize,
				&NoBytesRead, 
				NULL);
		m_Mutex->Unlock();
		if (NoBytesRead > 0)
			return NoBytesRead;
#endif
		return 0;
	}

	void SetCallback(SerialCallback * callback){
		m_Callback = callback;
	}

	SerialCallback * GetCallback(){
		return m_Callback;
	}

	void Start(){
		m_TearDown = false;
		m_Thread->Start();
	}

	void Stop(){
		m_TearDown = true;
		m_Thread->Join();
	}


private:
	SerialCallback * m_Callback;
	int fd;
	Core::Thread * m_Thread;
	Core::Mutex * m_Mutex;
	bool m_TearDown;

#if defined(NCK_LINUX) || defined(NCK_MACOSX)
	struct termios previousSettings;
#else
	HANDLE commPort;
#endif

};

#endif // #if defined(NCK_SERIAL_PORT)

Serial * OpenSerial(const std::string & portName,BaudRate baudrate)
{
#if defined(NCK_SERIAL_PORT)
	SerialPortDevice * dev = new SerialPortDevice();

	try{
		dev->Open(portName,baudrate);
		return dev;
	}
	catch (const Core::Exception & e) {
		SafeDelete(dev);
		THROW_EXCEPTION_STACK("Unable to open serial port",e);
	}
#endif
	return NULL;
}

std::vector<std::string> ListSerialPorts()
{
    std::vector<std::string> ret;
    ret.reserve(10);
    
#if defined(NCK_LINUX) | defined(NCK_MACOSX)
    std::vector<std::string> commands;
    
#if defined(NCK_MACOSX)
    commands.push_back("ls /dev/tty.* ");
#elif defined(NCK_LINUX)
    commands.push_back("ls /dev/ttyS* ");
    commands.push_back("ls /dev/ttyUSB* ");
#endif
    
    for(int i = 0;i<commands.size();i++){
        FILE * f = popen(commands[i].c_str(),"r");

        char * buffer = new char[128];
        char * r = NULL;
        fflush(f);
        while((r =  fgets(buffer, 128, f)) != NULL){
            std::string tmp(r);
            ret.push_back(tmp.substr(0,tmp.size()-1));
        }
        
        pclose(f);
        delete [] buffer;
    }
#elif defined(NCK_WINDOWS) 
	for (int i = 0; i < 20; i++) {
		std::string portName = "COM" + Math::IntToString(i);
		HANDLE commPort = CreateFile(portName.c_str(),          // for COM1—COM9 only
			GENERIC_READ | GENERIC_WRITE, //Read/Write
			0,               // No Sharing
			NULL,            // No Security
			OPEN_EXISTING,   // Open existing port only
			0,               // Non Overlapped I/O
			NULL);

		if (commPort != INVALID_HANDLE_VALUE) {
			ret.push_back(portName);
			CloseHandle(commPort);
		}
	}
#endif
    
    return ret;
}

_IO_END