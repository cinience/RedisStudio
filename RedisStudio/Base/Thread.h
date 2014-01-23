#ifndef Base_Thread_INCLUDED
#define Base_Thread_INCLUDED

#include <Windows.h>
#include "Runnable.h"

namespace Base {


class Thread
{
public:	
    typedef DWORD TIDImpl;
	typedef void (*Callable)(void*);

#if defined(_DLL)
	typedef DWORD (WINAPI *Entry)(LPVOID);
#else
	typedef unsigned (__stdcall *Entry)(void*);
#endif

	struct CallbackData
	{
		CallbackData(): callback(0), pData(0)
		{
		}

		Callable  callback;
		void*     pData; 
	};

	enum Priority
	{
		PRIO_LOWEST_IMPL  = THREAD_PRIORITY_LOWEST,
		PRIO_LOW_IMPL     = THREAD_PRIORITY_BELOW_NORMAL,
		PRIO_NORMAL_IMPL  = THREAD_PRIORITY_NORMAL,
		PRIO_HIGH_IMPL    = THREAD_PRIORITY_ABOVE_NORMAL,
		PRIO_HIGHEST_IMPL = THREAD_PRIORITY_HIGHEST
	};

	enum Policy
	{
		POLICY_DEFAULT_IMPL = 0
	};

	Thread();				
	~Thread();

	TIDImpl tid() const;
	void setPriority(int prio);
	int getPriority() const;
	void setOSPriority(int prio, int policy = 0);
	int getOSPriority() const;
	static int getMinOSPriority(int policy);
	static int getMaxOSPriority(int policy);
	void setStackSize(int size);
	int getStackSize() const;
	void start(Runnable& target);
	void start(Callable target, void* pData = 0);

	void join();
	bool join(long milliseconds);
	bool isRunning() const;
	static void sleep(long milliseconds);
	static void yieldImpl();
	static Thread* current();
	static TIDImpl currentTid();
    
protected:
#if defined(_DLL)
	static DWORD WINAPI runnableEntry(LPVOID pThread);
#else
	static unsigned __stdcall runnableEntry(void* pThread);
#endif

#if defined(_DLL)
	static DWORD WINAPI callableEntry(LPVOID pThread);
#else
	static unsigned __stdcall callableEntry(void* pThread);
#endif

	void create(Entry ent, void* pData);
	void threadCleanup();

private:
	class CurrentThreadHolder
	{
	public:
		CurrentThreadHolder(): _slot(TlsAlloc())
		{
			if (_slot == TLS_OUT_OF_INDEXES)
				throw std::exception("cannot allocate thread context key");
		}
		~CurrentThreadHolder()
		{
			TlsFree(_slot);
		}
		Thread* get() const
		{
			return reinterpret_cast<Thread*>(TlsGetValue(_slot));
		}
		void set(Thread* pThread)
		{
			TlsSetValue(_slot, pThread);
		}
	
	private:
		DWORD _slot;
	};

	Runnable*    _pRunnableTarget;
	CallbackData _callbackTarget;
	HANDLE       _thread;
	DWORD        _threadId;
	int          _prio;
	int          _stackSize;

	static CurrentThreadHolder _currentThreadHolder;
};


//
// inlines
//
inline int Thread::getPriority() const
{
	return _prio;
}


inline int Thread::getOSPriority() const
{
	return _prio;
}


inline int Thread::getMinOSPriority(int /* policy */)
{
	return PRIO_LOWEST_IMPL;
}


inline int Thread::getMaxOSPriority(int /* policy */)
{
	return PRIO_HIGHEST_IMPL;
}


inline void Thread::sleep(long milliseconds)
{
	Sleep(DWORD(milliseconds));
}


inline void Thread::yieldImpl()
{
	Sleep(0);
}


inline void Thread::setStackSize(int size)
{
	_stackSize = size;
}


inline int Thread::getStackSize() const
{
	return _stackSize;
}


inline Thread::TIDImpl Thread::tid() const
{
	return _threadId;
}


} // namespace Base


#endif // Base_Thread_INCLUDED
