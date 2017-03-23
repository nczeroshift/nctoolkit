
/**
 * NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

// Fix windows anoying defines 
#if defined _WIN32 || defined _WIN64
#include <windows.h>
#include <winbase.h>
#undef CreateMutex
HANDLE WinCreateThread(LPTHREAD_START_ROUTINE routine,LPVOID args){
	return CreateThread(0,0,(LPTHREAD_START_ROUTINE)routine,(LPVOID)args,CREATE_SUSPENDED,0);
}
HANDLE WinCreateMutex(){
	return CreateMutexA(NULL,FALSE,NULL);
}

#else
#include <pthread.h>
#include <unistd.h>

#if defined(__APPLE__)
#include "macosx/nckWindow_macosx.h"
#endif

#endif

#include "nckThread.h"
#include "nckUtils.h"
#include <stdint.h>
#include <stdio.h>

_CORE_BEGIN

#if defined _WIN32 || defined _WIN64

void Thread::Wait(unsigned int time){
    // http://stackoverflow.com/questions/5801813/c-usleep-is-obsolete-workarounds-for-windows-mingw
    /*int64_t usec = time*1000;
    HANDLE timer;
    LARGE_INTEGER ft;
    ft.QuadPart = -(10*usec);
    timer = CreateWaitableTimer(NULL, TRUE, NULL);
    SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0);
    WaitForSingleObject(timer, INFINITE);
    CloseHandle(timer);*/
	// Windows may take some time to wake up after sleep, at least it sleep 14ms always.
	Sleep(time);
}

class Thread_Win32: public Thread{
public:
	Thread_Win32(Thread_Function func, void *args){
        m_Args = args;
        m_Func = func;
		m_Started = false;
        m_Handle = NULL;
	}

	virtual ~Thread_Win32(){
		if(m_Handle){
            CloseHandle(m_Handle);
            TerminateThread(m_Handle, 1);
		}
	}

	void Start(){
		if(!m_Started){
            m_Handle = WinCreateThread((LPTHREAD_START_ROUTINE)m_Func, m_Args);
            ResumeThread(m_Handle);
			m_Started = true;
		}
	}

	void Join(){
		WaitForSingleObject(m_Handle, INFINITE);
        
        CloseHandle(m_Handle);
        TerminateThread(m_Handle, 1);
        
        m_Handle = NULL;
		m_Started = false;
	}

    Thread_Function m_Func;
    void * m_Args;
	bool m_Started;
	HANDLE m_Handle;
};

class Mutex_Win32 : public Mutex{
public:
	Mutex_Win32(){
		m_Handle = WinCreateMutex();
	}

	~Mutex_Win32(){
		 CloseHandle(m_Handle);
	}

	void Lock(){
		WaitForSingleObject(m_Handle,INFINITE);
	}

	void Unlock(){
		ReleaseMutex(m_Handle);
	}

	HANDLE m_Handle;
};

#elif defined (__linux__) || defined(__APPLE__)

void Thread::Wait(unsigned int time){
	usleep(time*1000);
}

class Thread_Pthread: public Thread{
public:
	Thread_Pthread(Thread_Function func, void *args){
		m_Function = func;
		m_Arguments = args;
		m_Thread = 0;
		m_Started = false;
	}

	virtual ~Thread_Pthread(){

	}

	
	void Start(){
		pthread_create (&m_Thread, NULL, m_Function, m_Arguments);
	}

	void Join(){
		pthread_join(m_Thread,NULL);
		m_Thread = 0;
	}

	void *m_Arguments;
	Thread_Function m_Function;
	pthread_t m_Thread;
	bool m_Started;
};


class Mutex_Pthread : public Mutex{
public:
	Mutex_Pthread(){
		pthread_mutex_init(&m_Mutex,NULL);
	}

	virtual ~Mutex_Pthread(){
		pthread_mutex_destroy(&m_Mutex);
	}

	void Lock(){
		pthread_mutex_lock(&m_Mutex);
	}

	void Unlock(){
		pthread_mutex_unlock(&m_Mutex);
	}

	pthread_mutex_t m_Mutex;
};

#endif // #elif defined __linux__



Thread * CreateThread(Thread_Function function, void * data)
{
	if(!function)
		return NULL;
#if defined (_WIN32) || defined (_WIN64)
	Thread_Win32 *t = new Thread_Win32(function,data);
	return t;
#elif defined (__linux__) || defined(__APPLE__)
	return new Thread_Pthread(function,data);
#else
	return NULL;
#endif
}


Mutex * CreateMutex()
{
#if defined (_WIN32) || defined (_WIN64)
	return new Mutex_Win32();
#elif defined __linux__ || defined(__APPLE__)
	return new Mutex_Pthread();
#else
	return NULL;
#endif
}

Threadable::Threadable(){
	m_IsRunning = false;
	m_Teardown = false;
	m_Thread = CreateThread(ThreadCallback,this);
}

Threadable::~Threadable(){
	SafeDelete(m_Thread);
}

void Threadable::Start(){
	m_IsRunning = true;
    m_Teardown = false;
	m_Thread->Start();
}

void Threadable::Teardown(){
	m_Teardown = true;
}

void Threadable::TearDownAndJoin(){
	Teardown();
	m_Thread->Join();
}

bool Threadable::IsRunning(){
	return m_IsRunning;
}

void * Threadable::ThreadCallback(void * t){
#if defined (__APPLE__)
    void * pool = AutoReleasePool_Create_MacOSX();
#endif
	Threadable * tc = (Threadable*)t;
	tc->Run();
	tc->m_IsRunning = false;
#if defined (__APPLE__)
    AutoReleasePool_Destroy_MacOSX(pool);
#endif
	return NULL;
}

bool Threadable::IsTearingDown(){
	return m_Teardown;
}

_CORE_END
