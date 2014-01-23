#ifndef ScopedRedisReply_INCLUDED
#define ScopedRedisReply_INCLUDED

#include "hiredis/hiredis.h"

class ScopedRedisReply
{
public:
    ScopedRedisReply(redisReply* reply) : m_pReply(reply){}

    ~ScopedRedisReply()
    {
        if (m_pReply) freeReplyObject(m_pReply);
    }

    bool IsNull()
    {
        if (m_pReply) return false;
        return true;
    }

    redisReply* operator -> ()
    {
        if (m_pReply)
            return m_pReply;
        else 
            throw std::exception("NullPointer");
    }

private:
    redisReply* m_pReply;
};




#endif
