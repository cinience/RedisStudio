#include "StdAfx.h"
#include "Util.h"
#include <winsock2.h>
#include <Iphlpapi.h>
#pragma comment(lib,"Iphlpapi.lib") /// for GetAdaptersInfo

namespace Base {

std::string Util::convertHumanTimeDuration(long long seconds)
{
    std::stringstream sb;
    if (seconds == 0) {
        return "0 secs";
    }        
    if (seconds >= 86400) {
        sb << seconds / 86400;
        sb << " days";
        seconds -= (seconds / 86400) * 86400;
    }
    if (seconds >= 3600) {
        if (sb.str().size() > 0) {
            sb << " ";
        }
        sb << seconds / 3600;
        sb << "hrs" ;
        seconds -= (seconds / 3600) * 3600;
    }
    if (seconds >= 60) {
        if (sb.str().size() > 0) {
            sb << " ";
        }
        sb << seconds / 60 << "mins";
        seconds -= (seconds / 60) * 60;
    }
    if (seconds > 0) {
        if (sb.str().size() > 0) {
            sb << " ";
        } 
        sb << seconds;
        sb << "secs";
    }

    return sb.str();
}

std::string Util::getPCID()
{
    BYTE szSystemInfo[4096] = {0}; 
    UINT uSystemInfoLen = 0; 
    UINT uErrorCode = 0;
    IP_ADAPTER_INFO iai;
    ULONG uSize = 0;
    std::string nicID ;
    DWORD dwResult = GetAdaptersInfo( &iai, &uSize );
    if( dwResult != ERROR_BUFFER_OVERFLOW )
    {
        return nicID;
    }
    IP_ADAPTER_INFO* piai = ( IP_ADAPTER_INFO* )HeapAlloc( GetProcessHeap(), 0, uSize );
    if( piai == NULL )
    {
        return nicID;
    }
    dwResult = GetAdaptersInfo( piai, &uSize );
    if(ERROR_SUCCESS == dwResult)
    {
        IP_ADAPTER_INFO* piai2 = piai;
        while( piai2 != NULL && ( uSystemInfoLen + piai2->AddressLength ) < 4096U )
        {
            CopyMemory( szSystemInfo + uSystemInfoLen, piai2->Address, piai2->AddressLength );
            uSystemInfoLen += piai2->AddressLength;
            piai2 = piai2->Next;                        
        }
    }
    HeapFree( GetProcessHeap(), 0, piai);
    char buf[124] = {0};
    for (UINT idx = 0; idx < uSystemInfoLen; ++idx)
    {
        memset(buf, 0 , sizeof(buf));
        sprintf(buf, "%02X",szSystemInfo[idx]);
        nicID += buf;
    }
    return nicID;
}

} // namespace Base
