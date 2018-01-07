
/**
 * NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#define NCK_SERVERSTRING "Server: nctoolkit\r\n"
#define NCK_DATABUFFERSIZE 1024

#include "nckHttpServer.h"

#if defined (NCK_WINDOWS)
	#include <WinSock2.h>
	#include <ws2tcpip.h>
	typedef SOCKET nckSocket;
	#undef CreateMutex
#else
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <unistd.h>
	#include <strings.h>
	#include <sys/wait.h>
	#include <netdb.h>
	typedef int nckSocket;
#endif

#include <stdio.h>
#include <sys/types.h>
#include <ctype.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdint.h>

#include "nckException.h"
#include "nckThread.h"
#include "nckUtils.h"
#include "nckDataIO.h"

#if defined (NCK_WINDOWS)
	#define CLOSE_SOCKET(s) closesocket(s)
#else
	#define CLOSE_SOCKET(s) close(s)
#endif

extern int errno;

_NETWORK_BEGIN

class HttpDispatchedThreadContext
{
public:
	HttpDispatchedThreadContext(){
		thread = NULL;
		context = NULL;
		clientSocket = 0;
	}
	Core::Thread * thread;
	HttpServerContext * context;
	int clientSocket;
    std::string clientAdress;
};

class HttpRequestDispatcher{
public:
	HttpRequestDispatcher(HttpServerContext * context){
		this->context = context;
		dispatchedMutex = Core::CreateMutex();
		activeMutex = Core::CreateMutex();
	}

	~HttpRequestDispatcher(){
		while(GetActiveThreadsCount()>0){
			Core::Thread::Wait(1);
		}

		ClearUp();

		SafeDelete(activeMutex);
		SafeDelete(dispatchedMutex);
	}

	// Dispatch a new thread to handle the request.
    void Dispatch(Core::Thread_Function function, int clientSocket, const std::string & clientAdress)
	{
		HttpDispatchedThreadContext * dContext = new HttpDispatchedThreadContext();
		dContext->context = context;
		dContext->clientAdress = clientAdress;

		Core::Thread * thread = Core::CreateThread(function,dContext);

		dContext->thread = thread;
		dContext->clientSocket = clientSocket;
		activeMutex->Lock();
		activeThreads.push_front(thread);
		activeMutex->Unlock();

		thread->Start();
	}

	// Clear old threads from memory.
	void ClearUp(){
		dispatchedMutex->Lock();
		ListFor(Core::Thread*,dispatchedThreads,i)
		{
			(*i)->Join(); // just for precaution.
			delete (*i);
		}
		dispatchedThreads.clear();
		dispatchedMutex->Unlock();
	}

	// Schedule thread class to be released from memory.
	void Dispose(Core::Thread* thread)
	{
		activeMutex->Lock();
		activeThreads.remove(thread);
		activeMutex->Unlock();

		dispatchedMutex->Lock();
		dispatchedThreads.push_front(thread);
		dispatchedMutex->Unlock();
	}

	int GetActiveThreadsCount(){
		activeMutex->Lock();
		int count = (int)activeThreads.size();
		activeMutex->Unlock();
		return count;
	}

private:
	HttpServerContext * context;
	std::list<Core::Thread*> activeThreads;
	std::list<Core::Thread*> dispatchedThreads;
	Core::Mutex * dispatchedMutex;
	Core::Mutex * activeMutex;
};

class HttpServerContext{
public:

	HttpServerContext();
	~HttpServerContext();
	void Start(int port, int connections);
	void Stop();

	nckSocket httpdSocket;
	int httpdPort;
	int maxConnections;
	bool connectionTearDown;

	Core::Thread * connectionThread;

	HttpCallbackHandler * callbackHandler;
	HttpRequestDispatcher * dispatcher;
	static void * ConnectionCallback(void * data);
	static void * HandleRequestCallback(void * data);

	static bool FetchLine(int socket, std::string  * line);
	void SendData(int client, unsigned char * data, int64_t size, int flags);

	void SendUnimplemented(int client);
	void SendNotFound(int client);
	void SendRootDocument(int client);
	bool SendFile(int client, const std::string filename,MIMEType type);
	void SendJSONText(int client, const std::string text);
	void SendImageData(int client, Core::ImageType type,unsigned char * data, size_t size);
	void SendBadRequest(int client);
	void SendHeaders(int client, MIMEType type);
	MIMEType GetMIMEType(const std::string & filename);
    void SendPostCreated(int client);
    void SendForbiddenRequest(int client);
};


HttpServerContext::HttpServerContext(){
	callbackHandler = NULL;
	httpdSocket = 0;
	httpdPort = 0;
	maxConnections = 0;
	connectionThread = NULL;
	connectionTearDown = false;
	dispatcher = new HttpRequestDispatcher(this);
}

HttpServerContext::~HttpServerContext(){
	connectionTearDown = true;
	if(connectionThread)
		connectionThread->Join();

	SafeDelete(dispatcher);

	SafeDelete(connectionThread);

	connectionTearDown = false;

	if(httpdSocket){
		CLOSE_SOCKET(httpdSocket);
		httpdSocket = 0;
	}
}

void HttpServerContext::Stop()
{
	connectionTearDown = true;

	if(connectionThread)
		connectionThread->Join();

	SafeDelete(connectionThread);

	connectionTearDown = false;

	if(httpdSocket){
		CLOSE_SOCKET(httpdSocket);
		httpdSocket = 0;
	}
}


void HttpServerContext::Start(int port, int connections)
{
	sockaddr_in name;
	
#if defined(NCK_WINDOWS)
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,2), &wsaData);
#else

	//http://stackoverflow.com/questions/6824265/sigpipe-broken-pipe
	signal(SIGPIPE, SIG_IGN);
#endif

	httpdSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	httpdPort = port;

	if (httpdSocket == -1)
		THROW_EXCEPTION("Unable to create server socket");

#if defined(NCK_MACOSX)
    int yes=1;
    if (setsockopt(httpdSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
        perror("setsockopt");
        exit(1);
    }
#endif

	memset(&name, 0, sizeof(name));

	name.sin_family = AF_INET;
	name.sin_port = htons(httpdPort);
	name.sin_addr = in4addr_any;
    
	if (bind(httpdSocket, (struct sockaddr *)&name, sizeof(name)) < 0){
		THROW_EXCEPTION("Unable to bind address family to socket");
    }
    
	if (httpdPort == 0)
	{
		int namelen = sizeof(name);

		if (getsockname(httpdSocket, (struct sockaddr *)&name,(socklen_t*) &namelen) == -1)
			THROW_EXCEPTION("Unable to get the bounded socket port");

		httpdPort = ntohs(name.sin_port);
	}

    struct timeval socket_tv;
    socket_tv.tv_sec = 10;
    socket_tv.tv_usec = 0;
    setsockopt(httpdSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&socket_tv,sizeof(struct timeval));
    
	maxConnections = connections;

	if (listen(httpdSocket, maxConnections) < 0)
        
		THROW_EXCEPTION("Unable to mark socket as listener");

	connectionThread = Core::CreateThread(ConnectionCallback,this);
	connectionThread->Start();
}

bool HttpServerContext::FetchLine(int socket, std::string  * line)
{
	int i = 0;
	char c = '\0';
	int n;

	*line = "";

	while (c != '\n')
	{
		n = (int)recv(socket, &c, 1, 0);
		/* DEBUG printf("%02X\n", c); */
		if (n > 0)
		{
			if (c == '\r')
			{

			n = (int)recv(socket, &c, 1, MSG_PEEK);

			/* DEBUG printf("%02X\n", c); */
			if ((n > 0) && (c == '\n'))
				recv(socket, &c, 1, 0);
			else
				c = '\n';
			}

			(*line) += c;
			i++;
		}
		else
			c = '\n';
	}

	return i>0?true:false;
}


void * HttpServerContext::ConnectionCallback(void * data)
{
	HttpServerContext * context = (HttpServerContext*)data;

	fd_set master;
	FD_ZERO(&master);
	FD_SET(context->httpdSocket, &master);

	int fdmax = context->httpdSocket;

	while(!context->connectionTearDown)
	{
		fd_set read_fds = master;

		struct timeval timeout;
		timeout.tv_sec  = 0;
		timeout.tv_usec = 1000;

		if(select(fdmax+1,&read_fds,NULL,NULL,&timeout)>0)
		{
			while(context->dispatcher->GetActiveThreadsCount()>5)
				Core::Thread::Wait(1);

			struct sockaddr_in clientAddr;
			int clientAddrLen = sizeof(clientAddr);

			int clientSocket = accept(context->httpdSocket,
							   (struct sockaddr *)&clientAddr,
							   (socklen_t*)&clientAddrLen);

            char addrStr[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(clientAddr.sin_addr), addrStr, INET_ADDRSTRLEN);
                        
            struct timeval socket_tv;
            socket_tv.tv_sec = 10;
            socket_tv.tv_usec = 0;
            setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&socket_tv,sizeof(struct timeval));
            
            
            context->dispatcher->Dispatch(HandleRequestCallback,clientSocket,std::string(addrStr));
		}

		context->dispatcher->ClearUp();
	}

	return NULL;
}

static std::string urlDecode(std::string src){
    std::string ret;
    for(int i = 0; i < src.length(); i++){
        std::string sub = src.substr(i,3);
        if(sub.size() == 3 && sub[0] == '%'){
            char tmp[3] = {sub[1],sub[2],'\0'};
            int number = (int)strtol(tmp, NULL, 16);
            ret += (char)number;
            i+=2;
        }
        else if(src[i] == '+')
            ret += " ";
        else
            ret += src[i];
    }
    return ret;
}

static std::map<std::string, std::string> mapHeader(const std::string & src){
    std::map<std::string, std::string> ret;
    size_t index ;
    std::string tmp = src;
    do{
        index = tmp.find_first_of("\n");
  
        std::string param;
        if(index != std::string::npos)
            param = tmp.substr(0,index);
        else
            param = tmp;
        
        size_t split = param.find_first_of(":");
        if(split!=std::string::npos){
            std::string type = param.substr(0,split);
            std::string value = param.substr(split+1,param.length());
            
            ret.insert(std::pair<std::string,std::string>(type,Core::ltrim(value)));
        }
                       
        if(index != std::string::npos)
            tmp = tmp.substr(index+1,tmp.length());
    }while(index);
    
    return ret;
}

static std::map<std::string,std::string> mapParamaters(const std::string & src){
    std::map<std::string,std::string> parameters;
    bool newParam = false;
    bool newValue = false;
    std::string strParam="";
    std::string strValue="";
    
    for(size_t i = 0; i < src.length(); i++)
    {
        if((src[i] == '&' || src[i]== '?' ) && !newParam)
        {
            if(strParam.length()!=0)
            {
                strValue = urlDecode(strValue);
                parameters.insert(std::pair<std::string,std::string>(strParam,strValue));
                strParam = "";
                strValue = "";
            }
            newParam = true;
        }
        else if(src[i] == '=' && newParam)
        {
            newParam = false;
            newValue = true;
        }
        else if(newParam){
            strParam+= src[i];
        }
        else if(newValue){
            strValue+= src[i];
        }
            
    }
        
    if(strParam.length()!=0)
    {
        strValue = urlDecode(strValue);
        parameters.insert(std::pair<std::string,std::string>(strParam,strValue));
        strParam = "";
        strValue = "";
    }
    return parameters;
}

static std::string ReadStream(HttpDispatchedThreadContext * dCtx){
    HttpServerContext * context = dCtx->context;
    std::string ret = "";
    while(true){
        std::string temp;
        if(!context->FetchLine(dCtx->clientSocket,&temp))
            break;
        ret += temp;
        if(temp=="\n")
            break;
    }
    return ret;
}

void * HttpServerContext::HandleRequestCallback(void * data)
{
	HttpDispatchedThreadContext * dCtx = (HttpDispatchedThreadContext*)data;
	HttpServerContext * context = dCtx->context;

	std::string line;

	if(FetchLine(dCtx->clientSocket,&line))
	{
        line = Core::ltrim(line);

		size_t firstSpace = line.find_first_of(" ");
		size_t endSpace = line.find_last_of(" ");

		std::string strType = line.substr(0,firstSpace);
		std::string strPath = Core::ltrim(line.substr(firstSpace,endSpace-firstSpace));
		std::string strVersion = Core::rtrim(Core::ltrim(line.substr(endSpace,line.length()-endSpace)));

		MIMEType mType = context->GetMIMEType(strPath);

		std::string headerStr = ReadStream(dCtx);
        std::map<std::string,std::string> headerMap = mapHeader(headerStr);
        std::map<std::string,std::string> paramMap;
        if(strPath.find_first_of("?") != std::string::npos)
            paramMap = mapParamaters(strPath);

		if(strType == "GET")
		{
            if(context->callbackHandler && !context->callbackHandler->GetRequest(dCtx->clientAdress, strPath, strVersion, headerMap)){
                // Content not accessible
                context->SendForbiddenRequest(dCtx->clientSocket);
            }
            else if(strPath == "/")
			{
                // Serve root document
				context->SendRootDocument(dCtx->clientSocket);
                
				if(context->callbackHandler)
					context->callbackHandler->SendFileEvent(dCtx->clientAdress, "index.html", mType);
			}
            else if(strPath.find("/x/") != std::string::npos)
            {
                // Extended server capabilites
                bool json = strPath.find(".json") != std::string::npos;
                bool bmp = strPath.find(".bmp") != std::string::npos;
                bool jpg = strPath.find(".jpg") != std::string::npos;
                
                if(bmp || jpg){
                    Core::ImageType imgType;
                    if(bmp) imgType = Core::IMAGE_TYPE_BMP;
                    else if(jpg) imgType = Core::IMAGE_TYPE_JPEG;
                    
                    if(paramMap.size() > 0)
                    {
                        if(context->callbackHandler)
                        {
                            unsigned char * data = NULL;
                            size_t size = context->callbackHandler->GetImage(dCtx->clientAdress, imgType, paramMap, &data);
                            if(size > 0 && data != NULL)
                                context->SendImageData(dCtx->clientSocket, imgType, data, size);
                            else
                                context->SendBadRequest(dCtx->clientSocket);
                        }
                    }
                }
                else if(json)
                {
                    if(context->callbackHandler)
                        context->SendJSONText(dCtx->clientSocket, context->callbackHandler->GetJSON(dCtx->clientAdress, paramMap));
                }
                else
                    context->SendBadRequest(dCtx->clientSocket);
            }
			else
			{
                // Send an existing file.
                bool sendFile = true;
                
				if(context->callbackHandler)
					sendFile = context->callbackHandler->SendFileEvent(dCtx->clientAdress, strPath, mType);
                
                if(sendFile)
                    context->SendFile(dCtx->clientSocket, strPath, mType);
                else
                    context->SendBadRequest(dCtx->clientSocket);
			}
		}
		else if(strType == "POST")
		{
            std::string dataStr = ReadStream(dCtx);
            std::map<std::string,std::string> dataMap = mapParamaters(dataStr);
            
            if(context->callbackHandler)
                context->callbackHandler->PostRequest(dCtx->clientAdress, strPath, strVersion, headerMap, dataMap);

            context->SendPostCreated(dCtx->clientSocket);
		}
		else
		{
			if(context->callbackHandler)
				context->callbackHandler->UnsupportedRequest(dCtx->clientAdress, strPath, strVersion, headerMap);

			printf("Unsupported request method (%s)\n",strType.c_str());
		}
	}

	CLOSE_SOCKET(dCtx->clientSocket);

	context->dispatcher->Dispose(dCtx->thread);

	delete dCtx;

	return NULL;
}

void HttpServerContext::SendPostCreated(int client){
    char buf[1024];
    strcpy(buf, "HTTP/1.0 201 CREATED\r\n");
    send(client, buf, strlen(buf), 0);
    
    //sprintf(buf,"Accept: application/json, text/javascript, */*; q=0.01\r\n");
    sprintf(buf,"Accept: application/x-www-form-urlencoded; charset=UTF-8");
    send(client, buf, strlen(buf), 0);
    
    sprintf(buf, "Content-Type: text/json\r\n");
    send(client, buf, strlen(buf), 0);
    
    sprintf(buf,"Accept-Encoding:\r\n");
    send(client, buf, strlen(buf), 0);
    
    sprintf(buf,"Cache-Control: max-age=0, no-cache, no-store\r\n");
    send(client, buf, strlen(buf), 0);
    
    sprintf(buf,"Pragma: no-cache\r\n");
    send(client, buf, strlen(buf), 0);
    
    strcpy(buf, "Content-Length: 0\r\n");
    send(client, buf, strlen(buf), 0);
    
    strcpy(buf, "\r\n");
    send(client, buf, strlen(buf), 0);

}

void HttpServerContext::SendNotFound(int client)
{
	char buf[1024];

	sprintf(buf, "HTTP/1.0 404 NOT FOUND\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, NCK_SERVERSTRING);
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "Content-Type: text/html\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "<HTML><TITLE>Not Found</TITLE>\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "<BODY><P>The server could not fulfill\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "your request because the resource specified\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "is unavailable or nonexistent.\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "</BODY></HTML>\r\n");
	send(client, buf, strlen(buf), 0);
}

void HttpServerContext::SendForbiddenRequest(int client)
{
    char buf[1024];
    
    sprintf(buf, "HTTP/1.0 403 BAD REQUEST\r\n");
    send(client, buf, sizeof(buf), 0);
    sprintf(buf, "Content-type: text/html\r\n");
    send(client, buf, sizeof(buf), 0);
    sprintf(buf, "\r\n");
    send(client, buf, sizeof(buf), 0);
}

void HttpServerContext::SendBadRequest(int client)
{
	char buf[1024];

	sprintf(buf, "HTTP/1.0 400 BAD REQUEST\r\n");
	send(client, buf, sizeof(buf), 0);
	sprintf(buf, "Content-type: text/html\r\n");
	send(client, buf, sizeof(buf), 0);
	sprintf(buf, "\r\n");
	send(client, buf, sizeof(buf), 0);
	sprintf(buf, "<P>Your browser sent a bad request, ");
	send(client, buf, sizeof(buf), 0);
	sprintf(buf, "such as a POST without a Content-Length.\r\n");
	send(client, buf, sizeof(buf), 0);
}

void HttpServerContext::SendUnimplemented(int client)
{
	char buf[1024];

	sprintf(buf, "HTTP/1.0 501 Method Not Implemented\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, NCK_SERVERSTRING);
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "Content-Type: text/html\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "<HTML><HEAD><TITLE>Method Not Implemented\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "</TITLE></HEAD>\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "<BODY><P>HTTP request method not supported.\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "</BODY></HTML>\r\n");
	send(client, buf, strlen(buf), 0);
}

void HttpServerContext::SendData(int client, unsigned char * data, int64_t size, int flags)
{
	if(size == 0){
		send(client,NULL,0,flags);
		return;
	}

	int64_t currentSize = size;
	int64_t currentOffset = 0;
	while(currentOffset < size && currentSize > 0)
	{
		int retSize = (int)send(client,(const char*)(data+currentOffset),currentSize,flags);

		if(retSize<0) // Ops ... socket gone
			break;

		currentSize-=retSize;
		currentOffset += retSize;

		if(retSize==0)
			break;
	}
}

bool HttpServerContext::SendFile(int client,const std::string filename,MIMEType type)
{
	std::string fullFilename = Core::GetDataFolder()+"httpd/" + filename;

	FILE * resource = fopen(fullFilename.c_str(),"rb");

	if(!resource){
		SendHeaders(client,MIME_HTML_TEXT);
		SendNotFound(client);
		return false;
	}

	SendHeaders(client,type);

	bool eof = false;
	char buf[NCK_DATABUFFERSIZE+1];

	do{
		memset(buf,0,NCK_DATABUFFERSIZE+1);
		size_t size = fread(buf,1,NCK_DATABUFFERSIZE,resource);

		if(!size)
			break;

		#if defined(NCK_WINDOWS)

		SendData(client,(unsigned char*)buf,size,0);

		if(feof(resource)){
			eof = true;
			SendData(client,(unsigned char*)NULL,0,0);
		}

		#else

		if(feof(resource)){
			eof = true;
			SendData(client,(unsigned char*)buf,size,0);
		}
		else{
#if defined(NCK_WINDOWS) || defined(NCK_APPLE)
            SendData(client,(unsigned char*)buf,size,0);
#else
			SendData(client,(unsigned char*)buf,size,MSG_MORE);
#endif
			
		}

		#endif

	}while(!eof);

	fclose(resource);

	return true;
}

void HttpServerContext::SendJSONText(int client, const std::string text){
	SendHeaders(client,MIME_JSON_TEXT);
	SendData(client,(unsigned char * )text.c_str(),text.length(),0);
}

void HttpServerContext::SendImageData(int client, Core::ImageType type, unsigned char * data, size_t size){
	if(type == Core::IMAGE_TYPE_JPEG)
		SendHeaders(client,MIME_JPEG_IMAGE);
	else if(type == Core::IMAGE_TYPE_BMP)
		SendHeaders(client,MIME_BMP_IMAGE);

	int p = 0;

	while(p<size){
		size_t remSize = size - p;

		if(remSize > NCK_DATABUFFERSIZE){
			remSize = NCK_DATABUFFERSIZE;
#if defined(NCK_WINDOWS) || defined(NCK_APPLE)
			SendData(client,data+p,remSize,0);
#else
			SendData(client,data+p,remSize,MSG_MORE);
#endif
		}
		else{
			SendData(client,data+p,remSize,0);
		}

		p+=remSize;
	}
}


void HttpServerContext::SendRootDocument(int client){
	SendFile(client, "index.html", MIME_HTML_TEXT);
}


void HttpServerContext::SendHeaders(int client,MIMEType type)
{
	char buf[1024];
	strcpy(buf, "HTTP/1.0 200 OK\r\n");
	send(client, buf, strlen(buf), 0);
	strcpy(buf, NCK_SERVERSTRING);
	send(client, buf, strlen(buf), 0);

	switch(type){
		case MIME_HTML_TEXT:
			sprintf(buf, "Content-Type: text/html\r\n");
			break;
		case MIME_JPEG_IMAGE:
			sprintf(buf, "Content-Type: image/jpeg\r\n");
			break;
		case MIME_BMP_IMAGE:
			sprintf(buf, "Content-Type: image/bmp\r\n");
			break;
		case MIME_PNG_IMAGE:
			sprintf(buf, "Content-Type: image/png\r\n");
			break;
		case MIME_PLAIN_TEXT:
			sprintf(buf, "Content-Type: text/plain\r\n");
			break;
		case MIME_XML_TEXT:
			sprintf(buf, "Content-Type: text/xml\r\n");
			break;
		case MIME_JSON_TEXT:
			sprintf(buf, "Content-Type: text/json\r\n");
			break;
		case MIME_JAVASCRIPT:
			sprintf(buf, "Content-Type: application/javascript\r\n");
			break;
		case MIME_CSS_TEXT:
			sprintf(buf, "Content-Type: text/css\r\n");
			break;
	}

	send(client, buf, strlen(buf), 0);
    
    sprintf(buf,"Cache-Control: max-age=0, no-cache, no-store\r\n");
    send(client, buf, strlen(buf), 0);
    
    sprintf(buf,"Pragma: no-cache\r\n");
    send(client, buf, strlen(buf), 0);
    
	strcpy(buf, "\r\n");
	send(client, buf, strlen(buf), 0);
}

MIMEType HttpServerContext::GetMIMEType(const std::string & filename){
	size_t paramPos = filename.find_first_of("?");

	MIMEType type = MIME_HTML_TEXT;

	std::string extensionName ="";

	if(paramPos == std::string::npos)
	{
		size_t extensionPos = filename.find_last_of(".");
		extensionName = filename.substr(extensionPos+1,filename.length()-extensionPos-1);
	}
	else{
		std::string tempName = filename.substr(0,paramPos);
		size_t extensionPos = tempName.find_last_of(".");
		extensionName = filename.substr(extensionPos+1,tempName.length()-extensionPos-1);
	}

	if(extensionName == "jpg")
		type = MIME_JPEG_IMAGE;
	else if(extensionName == "bmp")
		type = MIME_BMP_IMAGE;
	else if(extensionName == "png")
		type = MIME_PNG_IMAGE;
	else if(extensionName == "txt")
		type = MIME_PLAIN_TEXT;
	else if(extensionName == "xml")
		type = MIME_XML_TEXT;
	else if(extensionName == "json")
		type = MIME_JSON_TEXT;
	else if(extensionName == "js")
		type = MIME_JAVASCRIPT;
	else if(extensionName == "css")
		type = MIME_CSS_TEXT;

	return type;
}

HttpServer * HttpServer::Create(int port){
	HttpServer * ret = new HttpServer();
	ret->context = new HttpServerContext();
	ret->context->Start(port,30);

	return ret;
}

void HttpServer::SetCallback(HttpCallbackHandler * handler){
	context->callbackHandler = handler;
}

HttpServer::HttpServer(){
	context = NULL;
}

HttpServer::~HttpServer(){
	SafeDelete(context);
}

int HttpServer::GetPort(){
	if(context)
		return context->httpdPort;
	return 0;
}

_NETWORK_END

