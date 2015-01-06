#include "StdAfx.h"
#include "Http.h"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")

#include "String.h"
#include <sstream>

#define BUFFERSIZE 1024

namespace Base {

Http::Http(const std::string& hostname, int port)
{
    struct hostent *remoteHost = gethostbyname(hostname.c_str());
    struct in_addr addr;
    _host = hostname;
    _port = port;
    if (remoteHost != NULL) {
        if (remoteHost->h_addrtype == AF_INET)
        {
            if (remoteHost->h_addr_list[0] != 0) {
                addr.s_addr = *(u_long *) remoteHost->h_addr_list[0];
                _ip = inet_ntoa(addr);
            }
        }
    }
}

Http::~Http()
{
    
}

bool Http::ping() 
{
    if (_ip.empty()) return false;
    return true;
}
    
Http::Response Http::post(const std::string& path, const std::string& data) 
{
    string request;
    string response;
    int resp_leng;
    Response res;
    res.status = -1;

    char buffer[1024] = {0};
    struct sockaddr_in serveraddr;
    int sock;
    
    std::stringstream ss;
    ss << data.length();

    std::stringstream request2;

    request2 << "POST " << path << " HTTP/1.1"<<endl;
    request2 << "User-Agent: RedisStudio" << endl;
    //request2 << "" << endl;
    request2 << "Host: " << _host << endl;
    request2 << "Content-Length: " << data.length() << endl;
    request2 << "Content-Type: application/json;charset=utf-8" << endl;
    request2 << endl;
    request2 << data;
    request = request2.str();
    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        res.status = -1;
        res.data = "open socket failed.";
        return res;
    }
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family      = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr(_ip.c_str());
    serveraddr.sin_port        = htons((unsigned short) _port);
    if (connect(sock, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0) {
        res.status = -1;
        res.data = "connect() failed";
        return res;
    }
    //send request
    if (send(sock, request.c_str(), request.length(), 0) != request.length()) {
        res.status = -1;
        res.data ="send() sent a different number of bytes than expected";
        closesocket(sock);
        return res;
    }
    
    //get response
    response = "";
    resp_leng= BUFFERSIZE;
    while (resp_leng == BUFFERSIZE)
    {
        resp_leng= recv(sock, (char*)&buffer, BUFFERSIZE, 0);
        if (resp_leng>0)
            response+= string(buffer).substr(0,resp_leng);
        //note: download lag is not handled in this code
    }
    //disconnect
    closesocket(sock);

    String::TSeqStr seq;
    String::Split(response, "\n", seq);
    if (seq.size() == 0) {
        res.status = -1;
        res.data = "response data is wrong";
        return res;
    }
    String::TSeqStr hdrseq;
    String::Split(seq[0], " ", hdrseq);
    if (hdrseq.size() > 1) {
        res.status = atoi(hdrseq[1].c_str());
    }
    if (res.status == 200) {
        char *p = strstr(buffer, "\r\n\r\n") + 4;
        res.data = p;
    }
    return res;
}

} // namespace Base
