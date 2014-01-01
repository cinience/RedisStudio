#include "StdAfx.h"
#include "Mutex.h"

Mutex::Mutex()
{
	InitializeCriticalSectionAndSpinCount(&_cs, 4000);
}


Mutex::~Mutex()
{
	DeleteCriticalSection(&_cs);
}
