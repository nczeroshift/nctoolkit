
/**
 * NCtoolKit © 2007-2018 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckUriCodec.h"
#include "nckUtils.h"

// URI ecode/decode by Jin Qing, found in codeguru.com
// http://en.wikipedia.org/wiki/Percent-encoding#Types_of_URI_characters

static long HEX2DEC[256];
static bool SAFE[256];
static bool initFlag = false;

_NETWORK_BEGIN

void initConsts() {
    if (initFlag)
        return;

    for (int i = 0; i < 255; i++)
        HEX2DEC[i] = -1;

    HEX2DEC['0'] = 0;
    HEX2DEC['1'] = 1;
    HEX2DEC['2'] = 2;
    HEX2DEC['3'] = 3;
    HEX2DEC['4'] = 4;
    HEX2DEC['5'] = 5;
    HEX2DEC['6'] = 6;
    HEX2DEC['7'] = 7;
    HEX2DEC['8'] = 8;
    HEX2DEC['9'] = 9;

    HEX2DEC['A'] = 10;
    HEX2DEC['B'] = 11;
    HEX2DEC['C'] = 12;
    HEX2DEC['D'] = 13;
    HEX2DEC['E'] = 14;
    HEX2DEC['F'] = 15;

    HEX2DEC['a'] = 10;
    HEX2DEC['b'] = 11;
    HEX2DEC['c'] = 12;
    HEX2DEC['d'] = 13;
    HEX2DEC['e'] = 14;
    HEX2DEC['f'] = 15;

    for (int i = 0; i < 255; i++) {
        if ((i >= 'a' && i <= 'z') ||
            (i >= 'A' && i <= 'Z') ||
            (i >= '0' && i <= '9') || i == '.')
            SAFE[i] = true;
        else
            SAFE[i] = false;
    }

    initFlag = true;
}

std::string UriEncode(const std::string & sSrc)
{
    initConsts();

    const char DEC2HEX[16 + 1] = "0123456789ABCDEF";
    const unsigned char * pSrc = (const unsigned char *)sSrc.c_str();
    const int SRC_LEN = sSrc.length();
    unsigned char * const pStart = new unsigned char[SRC_LEN * 3];
    unsigned char * pEnd = pStart;
    const unsigned char * const SRC_END = pSrc + SRC_LEN;

    for (; pSrc < SRC_END; ++pSrc)
    {
        if (SAFE[*pSrc])
            *pEnd++ = *pSrc;
        else
        {
            // escape this char
            *pEnd++ = '%';
            *pEnd++ = DEC2HEX[*pSrc >> 4];
            *pEnd++ = DEC2HEX[*pSrc & 0x0F];
        }
    }

    std::string sResult((char *)pStart, (char *)pEnd);
    delete[] pStart;
    return sResult;
}

std::string UriDecode(const std::string & sSrc)
{
    initConsts();

    // Note from RFC1630: "Sequences which start with a percent
    // sign but are not followed by two hexadecimal characters
    // (0-9, A-F) are reserved for future extension"

    const unsigned char * pSrc = (const unsigned char *)sSrc.c_str();
    const int SRC_LEN = sSrc.length();
    const unsigned char * const SRC_END = pSrc + SRC_LEN;
    // last decodable '%' 
    const unsigned char * const SRC_LAST_DEC = SRC_END - 2;

    char * const pStart = new char[SRC_LEN];
    char * pEnd = pStart;

    while (pSrc < SRC_LAST_DEC)
    {
        if (*pSrc == '%')
        {
            char dec1, dec2;
            if (-1 != (dec1 = HEX2DEC[*(pSrc + 1)])
                && -1 != (dec2 = HEX2DEC[*(pSrc + 2)]))
            {
                *pEnd++ = (dec1 << 4) + dec2;
                pSrc += 3;
                continue;
            }
        }

        *pEnd++ = *pSrc++;
    }

    // the last 2- chars
    while (pSrc < SRC_END)
        *pEnd++ = *pSrc++;

    std::string sResult(pStart, pEnd);
    delete[] pStart;
    return sResult;
}

int UriParseParameters(char * text, int size, std::vector<std::pair<std::string,std::string>> * params) {
    int lastPos = 0;
    for (int i = 0; i < size; i++) {
        if (text[i] == '?' || text[i] == '&' || i == size - 1) {
            int limit = i;
            if (text[i] != '?' && limit - lastPos > 0) {
                int sz = (limit - lastPos) + ((i == size - 1) ? 1 : 0);
                char * buff = new char[sz+1];
                buff[sz] = '\0';
                memcpy(buff, text + lastPos, sz);
                std::string param(buff);
                int idx = param.find_first_of("=");
                if (idx > 0) {
                    std::string key = param.substr(0, idx);
                    std::string value = param.substr(idx + 1, param.size() - 1);
                    params->push_back(std::pair<std::string, std::string>(UriDecode(key), UriDecode(value)));
                }
                else {
                    params->push_back(std::pair<std::string,std::string>(UriDecode(param), ""));
                }
                SafeArrayDelete(buff);
            }

            lastPos = i + 1;
        }
    }

    return params->size();
}

_NETWORK_END