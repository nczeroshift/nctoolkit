
/**
 * NCtoolKit © 2007-2018 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_HTTPOBJECTS_H
#define NCK_HTTPOBJECTS_H

#include "nckNetworkConfig.h"
#include "nckQueueBuffer.h"
#include <map>
#include <vector>

_NETWORK_BEGIN

// Supported Multipurpose Internet Mail Extensions
enum MIMEType {
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

// Http Request Object
class HttpRequest {
public:
    HttpRequest();
    ~HttpRequest();

    int GetStatusCode();
    void SetStatusCode(int code);

    void SetType(MIMEType type);
    MIMEType GetType();

    Core::QueueBuffer * GetBuffer();

    void SetAddress(const std::string & addr);
    std::string GetAddress();

    void SetPath(const std::string & path);
    std::string GetPath();

    void SetMethod(const std::string & method);
    std::string GetMethod();
    
    void SetVersion(const std::string & path);
    std::string GetVersion();

    bool HasParameter(const std::string & name);
    void SetParameter(const std::string & name, const std::string & value);
    std::string GetParameter(const std::string & name);
    std::vector<std::string> GetParameterKeys();

    bool HasHeader(const std::string & name);
    void SetHeader(const std::string & name, const std::string & value);
    std::string GetHeader(const std::string & name);

private:
    Core::QueueBuffer * m_Buffer;
    MIMEType m_Type;
    int m_StatusCode;
    std::map<std::string, std::string> m_Headers;
    std::map<std::string, std::string> m_Parameters;
    std::string m_Path;
    std::string m_Address;
    std::string m_Version;
    std::string m_Method;
};

/// HTTP Response object
class HttpResponse {
public:
    HttpResponse();
    ~HttpResponse();

    int GetStatusCode();
    void SetStatusCode(int code);

    void SetType(MIMEType type);
    MIMEType GetType();

    Core::QueueBuffer * GetBuffer();

    bool HasHeader(const std::string & name);
    void SetHeader(const std::string & name, const std::string & value);
    std::string GetHeader(const std::string & name);
private:
    Core::QueueBuffer * m_Buffer;
    MIMEType m_Type;
    int m_StatusCode;
    std::map<std::string, std::string> m_Headers;
    std::string m_Path;
    std::string m_Address;
    std::string m_Version;
};

_NETWORK_END

#endif // #ifndef NCK_HTTPSERVER_H
