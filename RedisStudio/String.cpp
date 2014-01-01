#include "stdafx.h"
#include "String.h"

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
