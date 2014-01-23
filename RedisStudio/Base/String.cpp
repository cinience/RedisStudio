#include "stdafx.h"
#include "String.h"

namespace Base {

bool String::IsSpace( const char& ch )
{
	return ch==' ';
}

void String::Split(const std::string& str, const std::string& separators, TSeqStr& tokens, int options)
{
	std::string::const_iterator it1 = str.begin();
	std::string::const_iterator it2;
	std::string::const_iterator it3;
	std::string::const_iterator end = str.end();

	while (it1 != end)
	{
		if (options & TOK_TRIM)
		{
			while (it1 != end && IsSpace(*it1)) ++it1;
		}
		it2 = it1;
		while (it2 != end && separators.find(*it2) == std::string::npos) ++it2;
		it3 = it2;
		if (it3 != it1 && (options & TOK_TRIM))
		{
			--it3;
			while (it3 != it1 && IsSpace(*it3)) --it3;
			if (!IsSpace(*it3)) ++it3;
		}
		if (options & TOK_IGNORE_EMPTY)
		{
			if (it3 != it1)
				tokens.push_back(std::string(it1, it3));
		}
		else
		{
			tokens.push_back(std::string(it1, it3));
		}
		it1 = it2;
		if (it1 != end) ++it1;
	}
}

std::string String::Chrtos( char byte)
{
    std::string str;
    size_t len = 0;
    char buf[24] = {0};
    size_t size = 23;
    switch(byte) {
    case '\\':
    case '"':
        len = _snprintf(buf,size,"\\%c",byte);
        break;
    case '\n': len = _snprintf(buf,size,"\\n"); break;
    case '\r': len = _snprintf(buf,size,"\\r"); break;
    case '\t': len = _snprintf(buf,size,"\\t"); break;
    case '\a': len = _snprintf(buf,size,"\\a"); break;
    case '\b': len = _snprintf(buf,size,"\\b"); break;
    default:
        if (byte >= 33 && byte <=93)
            len = _snprintf(buf,size,"%c",byte);
        else
            len = _snprintf(buf,size,"\\x%02x",(unsigned char)byte);
        break;
    }
    str.assign(buf, len);
    return str;      
}

} // namespace Base