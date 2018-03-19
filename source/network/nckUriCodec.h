
/**
 * NCtoolKit © 2007-2018 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_URI_CODEC_H
#define NCK_URI_CODEC_H

#include "nckNetworkConfig.h"
#include <string>
#include <list>
#include <vector>

_NETWORK_BEGIN

std::string UriEncode(const std::string & sSrc);
std::string UriDecode(const std::string & sSrc);

int UriParseParameters(char * text, int size, std::vector<std::pair<std::string,std::string>> * params);

_NETWORK_END

#endif // #ifndef NCK_URI_CODEC_H
