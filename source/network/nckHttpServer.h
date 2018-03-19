
/**
 * NCtoolKit © 2007-2018 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_HTTPSERVER_H
#define NCK_HTTPSERVER_H

#include "nckHttpObjects.h"
#include "nckImage.h"

#include <map>

_NETWORK_BEGIN

class HttpCallbackHandler{
public:
	virtual ~HttpCallbackHandler(){};

    // Check if request can be processed, if not, send the response (by default 403).
    virtual bool AllowRequest(HttpRequest * request, HttpResponse * response) { return true; };

    // Handle request to custom endpoints (/x/ path).
    virtual bool HandleRequest(HttpRequest * request, HttpResponse * response) { return true; };
    
    // Callback when the request is not supported by this server.
    virtual void UnsupportedRequest(HttpRequest * request) {};
};

class HttpServerContext;

class HttpServer{
public:
	HttpServer();
	~HttpServer();
	static HttpServer * Create(int port);
	void SetCallback(HttpCallbackHandler * handler);
	int GetPort();
private:
	HttpServerContext * context;
};

_NETWORK_END

#endif // #ifndef NCK_HTTPSERVER_H
