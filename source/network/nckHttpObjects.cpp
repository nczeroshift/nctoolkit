
/**
 * NCtoolKit © 2007-2018 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckHttpObjects.h"
#include "nckUtils.h"

_NETWORK_BEGIN

HttpRequest::HttpRequest() {
    m_Buffer = NULL;
    m_StatusCode = 200;
    m_Type = MIME_PLAIN_TEXT;
}

HttpRequest::~HttpRequest() {
    SafeDelete(m_Buffer);
}

int HttpRequest::GetStatusCode() {
    return m_StatusCode;
}

void HttpRequest::SetStatusCode(int code) {
    m_StatusCode = code;
}

void HttpRequest::SetType(MIMEType type) {
    m_Type = type;
}

MIMEType HttpRequest::GetType() {
    return m_Type;
}

Core::QueueBuffer * HttpRequest::GetBuffer() {
    if(!m_Buffer) m_Buffer = new Core::QueueBuffer(0);
    return m_Buffer;
}

void HttpRequest::SetAddress(const std::string & addr) {
    m_Address = addr;
}

std::string HttpRequest::GetAddress() {
    return m_Address;
}

void HttpRequest::SetPath(const std::string & path) {
    m_Path = path;
}

std::string HttpRequest::GetPath() {
    return m_Path;
}

void HttpRequest::SetMethod(const std::string & method) {
    m_Method = method;
}

std::string HttpRequest::GetMethod() {
    return m_Method;
}

void HttpRequest::SetVersion(const std::string & path) {
    m_Version = path;
}

std::string HttpRequest::GetVersion() {
    return m_Version;
}

bool HttpRequest::HasParameter(const std::string & name) {
    if (m_Parameters.find(name) != m_Parameters.end())
        return true;
    return false;
}

void HttpRequest::SetParameter(const std::string & name, const std::string & value) {
    m_Parameters.insert(std::pair<std::string, std::string>(name, value));
}

std::string HttpRequest::GetParameter(const std::string & name) {
    if (m_Parameters.find(name) != m_Parameters.end())
        return m_Parameters.find(name)->second;
    return "";
}

std::vector<std::string> HttpRequest::GetParameterKeys() {
    std::vector<std::string> ret(m_Parameters.size());
    MapFor(std::string, std::string, m_Parameters, i)
        ret.push_back(i->first);
    return ret;
}

bool HttpRequest::HasHeader(const std::string & name) {
    if (m_Headers.find(name) != m_Headers.end())
        return true;
    return false;
}

void HttpRequest::SetHeader(const std::string & name, const std::string & value) {
    m_Headers.insert(std::pair<std::string, std::string>(name, value));
}

std::string HttpRequest::GetHeader(const std::string & name) {
    if (m_Headers.find(name) != m_Headers.end())
        return m_Headers.find(name)->second;
    return "";
}

HttpResponse::HttpResponse() {
    m_Buffer = NULL;
    m_StatusCode = 200;
    m_Type = MIME_PLAIN_TEXT;
}

HttpResponse::~HttpResponse() {
    SafeDelete(m_Buffer);
}

int HttpResponse::GetStatusCode() {
    return m_StatusCode;
}

void HttpResponse::SetStatusCode(int code) {
    m_StatusCode = code;
}

void HttpResponse::SetType(MIMEType type) {
    m_Type = type;
}

MIMEType HttpResponse::GetType() {
    return m_Type;
}

Core::QueueBuffer * HttpResponse::GetBuffer() {
    if(!m_Buffer) m_Buffer = new Core::QueueBuffer(0);
    return m_Buffer;
}

bool HttpResponse::HasHeader(const std::string & name) {
    if (m_Headers.find(name) != m_Headers.end())
        return true;
    return false;
}

void HttpResponse::SetHeader(const std::string & name, const std::string & value) {
    m_Headers.insert(std::pair<std::string, std::string>(name, value));
}

std::string HttpResponse::GetHeader(const std::string & name) {
    if (m_Headers.find(name) != m_Headers.end())
        return m_Headers.find(name)->second;
    return "";
}

_NETWORK_END