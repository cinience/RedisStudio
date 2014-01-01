#pragma once

template <class M>
class ScopedLock
{
public:
	ScopedLock(M& mutex): _mutex(mutex)
	{
		_mutex.lock();
	}

	~ScopedLock()
	{
		_mutex.unlock();
	}

private:
	M& _mutex;

	ScopedLock();
	ScopedLock(const ScopedLock&);
	ScopedLock& operator = (const ScopedLock&);
};