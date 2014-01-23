#ifndef Base_Event_INCLUDED
#define Base_Event_INCLUDED

#include <Windows.h>
#include <exception>

namespace Base {
class Event
{
public:
	Event(bool autoReset);		
	~Event();
	void set();
	void wait();
	bool wait(long milliseconds);
	void reset();
	
private:
	HANDLE _event;
};


//
// inlines
//
inline void Event::set()
{
	if (!SetEvent(_event))
	{
		throw std::exception("cannot signal event");
	}
}


inline void Event::reset()
{
	if (!ResetEvent(_event))
	{
		throw std::exception("cannot reset event");
	}
}

} //namespace Base

#endif