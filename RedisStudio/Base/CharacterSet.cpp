#include "StdAfx.h"
#include "CharacterSet.h"

namespace Base {

std::string CharacterSet::UnicodeConvert( const std::wstring& strWide, UINT uCodePage )
{
    std::string strANSI;
    int iLen = ::WideCharToMultiByte(uCodePage, 0, strWide.c_str(), -1, NULL, 0, NULL, NULL);

    if (iLen > 1)
    { 
        strANSI.resize(iLen-1);
        ::WideCharToMultiByte(uCodePage, 0, strWide.c_str(), -1, &strANSI[0], iLen, NULL, NULL);
    }

    return strANSI;
}

std::string CharacterSet::UnicodeToUTF8( const std::wstring& strWide )
{
    return UnicodeConvert(strWide, CP_UTF8);
}

std::string CharacterSet::UnicodeToANSI( const std::wstring& strWide )
{
    return UnicodeConvert(strWide, CP_ACP);
}

std::wstring CharacterSet::ANSIToUnicode( const std::string& str )
{
    int uniocedeLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
    wchar_t* pUnicode = new wchar_t[uniocedeLen + 1];
    memset(pUnicode, 0 ,(uniocedeLen+1)*sizeof(wchar_t));
    ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, (LPWSTR)pUnicode, uniocedeLen);

    wstring rt = (wchar_t*) pUnicode;
    delete pUnicode;

    return rt;
}

std::wstring CharacterSet::UTF8ToUnicode( const std::string& str )
{
    int uniocedeLen = ::MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.size(), NULL, 0);
    wchar_t* pUnicode = new wchar_t[uniocedeLen + 1];
    memset(pUnicode, 0 ,(uniocedeLen+1)*sizeof(wchar_t));
    ::MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.size(), (LPWSTR)pUnicode, uniocedeLen);

    wstring rt = (wchar_t*) pUnicode;
    delete pUnicode;

    return rt;
}

} // namespace Base
