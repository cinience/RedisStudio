#ifndef Base_Runnable_INCLUDED
#define Base_Runnable_INCLUDED

namespace Base {

class Runnable
{
public:    
    Runnable();
    virtual ~Runnable();
    
    virtual void run() = 0;
        /// Do whatever the thread needs to do. Must
        /// be overridden by subclasses.
};


} // namespace Base


#endif // Base_Runnable_INCLUDED
