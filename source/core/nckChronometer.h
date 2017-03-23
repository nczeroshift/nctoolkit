
/**
 * NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_CHRONOMETER_H
#define NCK_CHRONOMETER_H

#include "nckCoreConfig.h"

_CORE_BEGIN

/**
* Chronometer class used to measure elapsed time between two
* events.
*/
class Chronometer
{
public:
	virtual ~Chronometer(){};

	/// Reset stop watch.
	virtual void Clear()=0;

	/// Start stop watch.
	virtual void Start()= 0;

	/// Stop timer.
	virtual void Stop()= 0;

	/// Get elapsed time between start and stop events.
	/// @return Elapsed time in seconds.
	virtual double GetElapsedTime() = 0;

    virtual bool IsRunning() = 0;
};

/**
* Creates a Chronometer/Chronometer.
* @param high_res Specifies the timer resolution, true for 
* high resolution (miliseconds), false for low precision (seconds).
* @return Reference to the new Chronometer.
*/
Chronometer *CreateChronometer(bool high_res = true);

_CORE_END

#endif // #ifndef NCK_CHRONOMETER_H
