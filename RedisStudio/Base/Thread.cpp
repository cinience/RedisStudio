#include "StdAfx.h"
#include "Thread.h"
#include <exception>
#include <process.h>

namespace Base {


Thread::CurrentThreadHolder Thread::_currentThreadHolder;


Thread::Thread():
    _pRunnableTarget(0),
    _thread(0),
    _threadId(0),
    _prio(PRIO_NORMAL_IMPL),
    _stackSize(0) // os default
{
}

            
Thread::~Thread()
{
    if (_thread) CloseHandle(_thread);
}


void Thread::setPriority(int prio)
{
    if (prio != _prio)
    {
        _prio = prio;
        if (_thread)
        {
            if (SetThreadPriority(_thread, _prio) == 0)
                throw std::exception("cannot set thread priority");
        }
    }
}


void Thread::setOSPriority(int prio, int /* policy */)
{
    setPriority(prio);
}


void Thread::start(Runnable& target)
{
    if (isRunning())
        throw std::exception("thread already running");

    _pRunnableTarget = &target;

    create(runnableEntry, this);
}


void Thread::start(Callable target, void* pData)
{
    if (isRunning())
        throw std::exception("thread already running");

    threadCleanup();
    _callbackTarget.callback = target;
    _callbackTarget.pData = pData;

    create(callableEntry, this);
}


void Thread::create(Entry ent, void* pData)
{
#if defined(_DLL)
    _thread = CreateThread(NULL, _stackSize, ent, pData, 0, &_threadId);
#else
    unsigned threadId;
    _thread = (HANDLE) _beginthreadex(NULL, _stackSize, ent, this, 0, &threadId);
    _threadId = static_cast<DWORD>(threadId);
#endif
    if (!_thread)
        throw std::exception("cannot create thread");
    if (_prio != PRIO_NORMAL_IMPL && !SetThreadPriority(_thread, _prio))
        throw std::exception("cannot set thread priority");
}


void Thread::join()
{
    if (!_thread) return;

    switch (WaitForSingleObject(_thread, INFINITE))
    {
    case WAIT_OBJECT_0:
        threadCleanup();
        return;
    default:
        throw std::exception("cannot join thread");
    }
}


bool Thread::join(long milliseconds)
{
    if (!_thread) return true;

    switch (WaitForSingleObject(_thread, milliseconds + 1))
    {
    case WAIT_TIMEOUT:
        return false;
    case WAIT_OBJECT_0:
        threadCleanup();
        return true;
    default:
        throw std::exception("cannot join thread");
    }
}


bool Thread::isRunning() const
{
    if (_thread)
    {
        DWORD ec = 0;
        return GetExitCodeThread(_thread, &ec) && ec == STILL_ACTIVE;
    }
    return false;
}


void Thread::threadCleanup()
{
    if (!_thread) return;
    if (CloseHandle(_thread)) _thread = 0;
}


Thread* Thread::current()
{
    return _currentThreadHolder.get();
}


Thread::TIDImpl Thread::currentTid()
{
    return GetCurrentThreadId();
}


#if defined(_DLL)
DWORD WINAPI Thread::runnableEntry(LPVOID pThread)
#else
unsigned __stdcall Thread::runnableEntry(void* pThread)
#endif
{
    _currentThreadHolder.set(reinterpret_cast<Thread*>(pThread));
    try
    {
        reinterpret_cast<Thread*>(pThread)->_pRunnableTarget->run();
    }
    catch (std::exception& exc)
    {
        (void)exc; 
    }
    catch (...)
    {
        
    }
    return 0;
}


#if defined(_DLL)
DWORD WINAPI Thread::callableEntry(LPVOID pThread)
#else
unsigned __stdcall Thread::callableEntry(void* pThread)
#endif
{
    _currentThreadHolder.set(reinterpret_cast<Thread*>(pThread));
    try
    {
        Thread* pTI = reinterpret_cast<Thread*>(pThread);
        pTI->_callbackTarget.callback(pTI->_callbackTarget.pData);
    }
    catch (std::exception& exc)
    {
        (void)exc;   
    }
    catch (...)
    {
        
    }
    return 0;
}


} // namespace Poco
