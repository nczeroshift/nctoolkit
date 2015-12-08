
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_DATE_H_
#define NCK_DATE_H_

#include "nckCoreConfig.h"
#include <time.h>

_CORE_BEGIN

class Date{
public:
	Date();
	Date(long timestamp);
	~Date();

	int GetYear();
	int GetMonth();
	int GetDay();
	int GetHours();
	int GetMinutes();
	int GetSeconds();

	long GetLong();
private:
	time_t m_Timestamp;
};

_CORE_END

#endif // #ifndef NCK_DATE_H_

