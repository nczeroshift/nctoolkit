
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_HTTPSERVER_H
#define NCK_HTTPSERVER_H

#include "nckNetworkConfig.h"
#include "nckImage.h"

#include <map>

_NETWORK_BEGIN

// Supported Multipurpose Internet Mail Extensions
enum MIMEType{
	MIME_HTML_TEXT,
	MIME_JPEG_IMAGE,
	MIME_PNG_IMAGE,
	MIME_BMP_IMAGE,
	MIME_PLAIN_TEXT,
	MIME_XML_TEXT,
	MIME_JSON_TEXT,
	MIME_JAVASCRIPT,
	MIME_CSS_TEXT,
};

class HttpCallbackHandler{
public:
	virtual ~HttpCallbackHandler(){};

    virtual bool GetRequest(const std::string & srcAddr,
			const std::string & path,
			const std::string & version,
            const std::map<std::string, std::string> & headers){return true;};

	virtual void PostRequest(const std::string & srcAddr,
			const std::string & path,
			const std::string & version,
			const std::map<std::string, std::string> & headers,
            const std::map<std::string,std::string> & params){};

	virtual void UnsupportedRequest(const std::string & srcAddr,
			const std::string & path,
			const std::string & version,
			const std::map<std::string, std::string> & headers){};

	virtual bool SendFileEvent(const std::string & srcAddr,
			const std::string & path,
            MIMEType type){return true;};

	virtual std::string GetJSON(const std::string & srcAddr,
			std::map<std::string,std::string> params){return "";};
    
	virtual size_t GetImage(const std::string & srcAddr, Core::ImageType type,
			std::map<std::string,std::string> params, unsigned char ** data ){return 0;};
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
