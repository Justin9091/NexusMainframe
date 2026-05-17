#include "server/WsServer.hpp"
#include <cstring>
#include <iostream>
#include <sstream>
#include <vector>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#define WS_CLOSE(s)  closesocket(s)
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#define WS_CLOSE(s)  close(s)
#endif

// ── SHA-1 ─────────────────────────────────────────────────────────────────────

static void sha1(const void* data, size_t len, uint8_t out[20]) {
    uint32_t h0=0x67452301, h1=0xEFCDAB89, h2=0x98BADCFE,
             h3=0x10325476, h4=0xC3D2E1F0;

    auto rotl = [](uint32_t v, int n){ return (v<<n)|(v>>(32-n)); };

    std::vector<uint8_t> m(static_cast<const uint8_t*>(data),
                            static_cast<const uint8_t*>(data)+len);
    m.push_back(0x80);
    while (m.size() % 64 != 56) m.push_back(0);
    uint64_t bits = (uint64_t)len * 8;
    for (int i = 7; i >= 0; --i) m.push_back((uint8_t)(bits >> (i*8)));

    for (size_t i = 0; i < m.size(); i += 64) {
        uint32_t w[80];
        for (int j = 0; j < 16; ++j)
            w[j] = ((uint32_t)m[i+j*4]<<24)|((uint32_t)m[i+j*4+1]<<16)
                  |((uint32_t)m[i+j*4+2]<<8)|m[i+j*4+3];
        for (int j = 16; j < 80; ++j)
            w[j] = rotl(w[j-3]^w[j-8]^w[j-14]^w[j-16], 1);

        uint32_t a=h0,b=h1,c=h2,d=h3,e=h4;
        for (int j = 0; j < 80; ++j) {
            uint32_t f,k;
            if      (j<20){f=(b&c)|(~b&d);k=0x5A827999;}
            else if (j<40){f=b^c^d;       k=0x6ED9EBA1;}
            else if (j<60){f=(b&c)|(b&d)|(c&d);k=0x8F1BBCDC;}
            else          {f=b^c^d;       k=0xCA62C1D6;}
            uint32_t t=rotl(a,5)+f+e+k+w[j]; e=d;d=c;c=rotl(b,30);b=a;a=t;
        }
        h0+=a;h1+=b;h2+=c;h3+=d;h4+=e;
    }

    uint32_t hh[5]={h0,h1,h2,h3,h4};
    for (int i=0;i<5;++i)
        for (int j=3;j>=0;--j)
            out[i*4+(3-j)]=(uint8_t)(hh[i]>>(j*8));
}

// ── Base64 ────────────────────────────────────────────────────────────────────

static std::string base64Encode(const uint8_t* data, size_t len) {
    static const char* T="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string out;
    for (size_t i=0;i<len;i+=3) {
        uint32_t v=(uint32_t)data[i]<<16;
        if(i+1<len) v|=(uint32_t)data[i+1]<<8;
        if(i+2<len) v|=data[i+2];
        out+=T[(v>>18)&63]; out+=T[(v>>12)&63];
        out+=(i+1<len)?T[(v>>6)&63]:'=';
        out+=(i+2<len)?T[v&63]:'=';
    }
    return out;
}

// ── Helpers ───────────────────────────────────────────────────────────────────

static bool recvAll(ws_socket_t s, void* buf, size_t n) {
    auto* p=static_cast<char*>(buf);
    size_t got=0;
    while(got<n){
        int r=recv(s,p+got,(int)(n-got),0);
        if(r<=0) return false;
        got+=r;
    }
    return true;
}

// ── WsServer ──────────────────────────────────────────────────────────────────

WsServer::~WsServer() { stop(); }

void WsServer::start(int port) {
#ifdef _WIN32
    WSADATA wsa;
    WSAStartup(MAKEWORD(2,2),&wsa);
#endif
    _listenSock = socket(AF_INET,SOCK_STREAM,0);

    int opt=1;
    setsockopt(_listenSock,SOL_SOCKET,SO_REUSEADDR,(char*)&opt,sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=INADDR_ANY;
    addr.sin_port=htons((uint16_t)port);
    bind(_listenSock,(sockaddr*)&addr,sizeof(addr));
    listen(_listenSock,16);

    _running=true;
    _acceptThread=std::thread(&WsServer::acceptLoop,this);
    std::cout<<"[WsServer] Listening on port "<<port<<"\n";
}

void WsServer::stop() {
    if (!_running.exchange(false)) return;

    if (_listenSock != WS_INVALID_SOCKET) {
        WS_CLOSE(_listenSock);
        _listenSock = WS_INVALID_SOCKET;
    }
    {
        std::lock_guard lock(_mu);
        for (auto& [id,s]:_clients) WS_CLOSE(s);
        _clients.clear();
    }
    if (_acceptThread.joinable()) _acceptThread.join();
#ifdef _WIN32
    WSACleanup();
#endif
}

void WsServer::send(int clientId, const std::string& msg) {
    ws_socket_t sock;
    {
        std::lock_guard lock(_mu);
        auto it=_clients.find(clientId);
        if(it==_clients.end()) return;
        sock=it->second;
    }
    writeFrame(sock,msg);
}

void WsServer::broadcast(const std::string& msg) {
    std::vector<ws_socket_t> socks;
    {
        std::lock_guard lock(_mu);
        socks.reserve(_clients.size());
        for (auto& [id,s]:_clients) socks.push_back(s);
    }
    for (auto s:socks) writeFrame(s,msg);
}

void WsServer::acceptLoop() {
    while (_running) {
        sockaddr_in ca{};
        socklen_t len=sizeof(ca);
        ws_socket_t s=accept(_listenSock,(sockaddr*)&ca,&len);
        if (s==WS_INVALID_SOCKET) break;

        int id=_nextId++;
        {
            std::lock_guard lock(_mu);
            _clients[id]=s;
        }
        std::thread([this,id,s](){ clientLoop(id,s); }).detach();
    }
}

void WsServer::clientLoop(int id, ws_socket_t sock) {
    if (!handshake(sock)) {
        std::lock_guard lock(_mu);
        _clients.erase(id);
        WS_CLOSE(sock);
        return;
    }

    if (_onConnect) _onConnect(id);

    while (_running) {
        bool closed=false;
        std::string msg=readFrame(sock,closed);
        if (closed||!_running) break;
        if (!msg.empty()&&_onMessage) _onMessage(id,msg);
    }

    {
        std::lock_guard lock(_mu);
        _clients.erase(id);
        WS_CLOSE(sock);
    }
    if (_onClose&&_running) _onClose(id);
}

bool WsServer::handshake(ws_socket_t sock) {
    char buf[4096];
    int n=recv(sock,buf,sizeof(buf)-1,0);
    if(n<=0) return false;
    buf[n]='\0';

    std::string req(buf,n);
    auto pos=req.find("Sec-WebSocket-Key:");
    if(pos==std::string::npos) return false;
    pos+=18;
    while(pos<req.size()&&req[pos]==' ') pos++;
    auto end=req.find("\r\n",pos);
    if(end==std::string::npos) return false;
    std::string key=req.substr(pos,end-pos);

    std::string magic=key+"258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    uint8_t digest[20];
    sha1(magic.data(),magic.size(),digest);
    std::string accept=base64Encode(digest,20);

    std::string resp=
        "HTTP/1.1 101 Switching Protocols\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Sec-WebSocket-Accept: "+accept+"\r\n"
        "\r\n";
    return ::send(sock,resp.data(),(int)resp.size(),0)==(int)resp.size();
}

std::string WsServer::readFrame(ws_socket_t sock, bool& closed) {
    closed=false;
    uint8_t hdr[2];
    if(!recvAll(sock,hdr,2)){closed=true;return{};}

    uint8_t opcode=hdr[0]&0x0F;
    bool masked=(hdr[1]&0x80)!=0;
    uint64_t plen=hdr[1]&0x7F;

    if(plen==126){
        uint8_t e[2]; if(!recvAll(sock,e,2)){closed=true;return{};}
        plen=((uint64_t)e[0]<<8)|e[1];
    } else if(plen==127){
        uint8_t e[8]; if(!recvAll(sock,e,8)){closed=true;return{};}
        plen=0; for(int i=0;i<8;i++) plen=(plen<<8)|e[i];
    }

    uint8_t mask[4]={};
    if(masked&&!recvAll(sock,mask,4)){closed=true;return{};}

    std::vector<uint8_t> payload(plen);
    if(plen>0&&!recvAll(sock,payload.data(),(size_t)plen)){closed=true;return{};}
    if(masked) for(size_t i=0;i<payload.size();i++) payload[i]^=mask[i%4];

    if(opcode==0x8){closed=true;return{};}
    if(opcode==0x9){                           // ping → pong
        uint8_t pong[]={0x8A,0x00};
        ::send(sock,(char*)pong,2,0);
        return{};
    }
    return std::string(payload.begin(),payload.end());
}

bool WsServer::writeFrame(ws_socket_t sock, const std::string& payload) {
    std::vector<uint8_t> frame;
    frame.push_back(0x81);                     // FIN + text
    size_t len=payload.size();
    if(len<126){
        frame.push_back((uint8_t)len);
    } else if(len<65536){
        frame.push_back(126);
        frame.push_back((uint8_t)(len>>8));
        frame.push_back((uint8_t)len);
    } else {
        frame.push_back(127);
        for(int i=7;i>=0;--i) frame.push_back((uint8_t)(len>>(i*8)));
    }
    frame.insert(frame.end(),payload.begin(),payload.end());
    return ::send(sock,(char*)frame.data(),(int)frame.size(),0)==(int)frame.size();
}
