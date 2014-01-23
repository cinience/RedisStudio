#include "StdAfx.h"
#include "Event.h"

namespace Base {

Event::Event(bool autoReset)
{
	_event = CreateEventW(NULL, autoReset ? FALSE : TRUE, FALSE, NULL);
	if (!_event)
		throw std::exception("cannot create event");
}


Event::~Event()
{
	CloseHandle(_event);
}


void Event::wait()
{
	switch (WaitForSingleObject(_event, INFINITE))
	{
	case WAIT_OBJECT_0:
		return;
	default:
		throw std::exception("wait for event failed");
	}
}


bool Event::wait(long milliseconds)
{
	switch (WaitForSingleObject(_event, milliseconds + 1))
	{
	case WAIT_TIMEOUT:
		return false;
	case WAIT_OBJECT_0:
		return true;
	default:
		throw std::exception("wait for event failed");		
	}
}

} //namespace Base