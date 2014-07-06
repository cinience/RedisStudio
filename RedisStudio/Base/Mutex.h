#ifndef Base_ScopedMutex_INCLUDED
#define Base_ScopedMutex_INCLUDED

#include "ScopedLock.h"

namespace Base {

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

} // namespace Base

#endif