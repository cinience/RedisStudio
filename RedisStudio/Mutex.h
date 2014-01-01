#pragma once

#include "ScopedLock.h"

class  Mutex
{
public:
	typedef ScopedLock<Mutex> ScopedLock;
public:
	Mutex();
	~Mutex();
	void lock();
	bool tryLock();
	void unlock();

private:
	CRITICAL_SECTION _cs;
};

inline void Mutex::lock()
{
	try
	{
		EnterCriticalSection(&_cs);
	}
	catch (...)
	{
		throw std::exception("cannot lock mutex");
	}
}


inline bool Mutex::tryLock()
{
	try
	{
		return TryEnterCriticalSection(&_cs) != 0;
	}
	catch (...)
	{
	}
	throw std::exception("cannot lock mutex");
}


inline void Mutex::unlock()
{
	LeaveCriticalSection(&_cs);
}


