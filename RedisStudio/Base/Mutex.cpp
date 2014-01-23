#include "StdAfx.h"
#include "Mutex.h"

namespace Base {

Mutex::Mutex()
{
	InitializeCriticalSectionAndSpinCount(&_cs, 4000);
}


Mutex::~Mutex()
{
	DeleteCriticalSection(&_cs);
}

} // namespace Mutex