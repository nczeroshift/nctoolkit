
/**
 * NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_THREAD_H
#define NCK_THREAD_H

#include "nckCoreConfig.h"
#include <stdlib.h>

_CORE_BEGIN

/**
* Abstract thread manager class.
*/
class Thread{
public:
	/// Destructor.
	virtual ~Thread(){};

	/// Start/Resume thread.
	virtual void Start()=0;

	/// Wait for thread to terminate.
	virtual void Join()=0;

	/// Halt execution for short period (ms)
	static void Wait(unsigned int time);
};

/**
 * Abstract class for threading support on any class.
 */
class Threadable
{
public:
	Threadable();
	virtual ~Threadable();

	/// Start thread.
	void Start();

	/// Tell thread to end.
	void Teardown();

	/// Tell thread to end and join.
	void TearDownAndJoin();

	/// Check if the thread is running.
	bool IsRunning();

protected:
	/// Method to be implemented by the child class.
	virtual void Run()=0;

	/// Internal thread callback.
	static void  * ThreadCallback(void * t);

	/// Check if the thread should tear down.
	bool IsTearingDown();
private:
	bool m_Teardown;
	bool m_IsRunning;
	Thread * m_Thread;
};

/**
* Abstract Mutex class. 
*/
class Mutex{
public:
	/// Destructor.
	virtual ~Mutex(){};

	/// Lock mutex.
	virtual void Lock() = 0;

	/// Unlock mutex.
	virtual void Unlock() = 0;
};

/// Thread function type cast.
typedef void * (*Thread_Function)(void*);

/// Create thread from function pointer and data parameter.
Thread * CreateThread(Thread_Function function, void * data = NULL);

/// Create mutex.
Mutex * CreateMutex();

_CORE_END

#endif // #ifndef NCK_THREAD_H
