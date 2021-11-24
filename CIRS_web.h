#pragma once

#if END == CLIENT
#if 0
string _host_name="223.83.131.30";
#else
string _host_name="127.0.0.1";
#endif
#endif
int port = 2006;

namespace Web {

    static WSADATA wsaData;

    inline void use_net() {
        WSAStartup(MAKEWORD(2, 2), &wsaData);
    }

    inline void unuse_net() {
        WSACleanup();
    }

    inline LPWSADATA getWSADATA() {
        return &wsaData;
    }

    enum SocketState {uninit,canuse,closed,error};

#if END == SERVER
    class ServerSocket;
#endif

    class Socket {
            SOCKET so;
            SocketState state=uninit;
            SOCKADDR_IN ServerAddr;
#if END == SERVER
            friend class ServerSocket;
#endif
            Socket() {}
        public:
            Socket(const char *ipaddr,int port) {
                so = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
                ServerAddr.sin_family = AF_INET;
                ServerAddr.sin_port = htons(port);
                ServerAddr.sin_addr.s_addr = inet_addr(ipaddr);
                int result = connect(so, (SOCKADDR *)&ServerAddr,
                                     sizeof(ServerAddr));
                if(result!=0) state=error;
                else state=canuse;
            }
            const SocketState *const getState() {
                return &state;
            }
            void send(string msg) {
                int len=msg.size();
                ::send(so,(char *)&len,(int)sizeof(int),0);
                ::send((SOCKET)so,(char *)msg.c_str(),len,0);
            }
            ///不推荐使用，有使程序崩溃的风险
            void send(char *buf,int size) {
                ::send((SOCKET)so,(char *)&size,(int)sizeof(int),0);
                ::send((SOCKET)so,buf,size,0);
            }
            string recv() {
                int len2;
                ::recv(so,(char *)&len2,sizeof(int),0);
                char *buf=new char[len2+1];
                ::recv(so,buf,len2,0);
                buf[len2]=0;
                string str=buf;
                delete[] buf;
                return str;
            }
            ///不推荐使用，有使程序崩溃的风险
            void recv(char *buf) {
                int size;
                ::recv(so,(char *)&size,sizeof(int),0);
                ::recv(so,buf,size,0);
            }
            void close() {
                if(state!=closed)
                    closesocket(so);
                state=closed;
            }
            ~Socket() {
                close();
            }
    };

#if END == SERVER
    class ServerSocket {
            SocketState state=uninit;
            Socket listener;
        public:
            ServerSocket(int port) {
                listener.so = ::socket(AF_INET, SOCK_STREAM,
                                       IPPROTO_TCP);
                listener.ServerAddr.sin_family = AF_INET;
                listener.ServerAddr.sin_port = htons(port);
                listener.ServerAddr.sin_addr.s_addr = htonl(
                        INADDR_ANY);
                ::bind(listener.so, (SOCKADDR *)&listener.ServerAddr,
                       sizeof(listener.ServerAddr));
                listen(listener.so, 2);
                state=canuse;
            }
            Socket accept() {
                Socket so;
                int len = sizeof(so.ServerAddr);
                so.so = ::accept(listener.so,
                                 (SOCKADDR *)&so.ServerAddr, &len);
                so.state=canuse;
                return so;
            }
            void close() {
                if(state!=closed)
                    closesocket(listener.so);
                state=closed;
            }
            const SocketState *const getState() {
                return &state;
            }
            ~ServerSocket() {
                close();
            }
    };
#endif
}

namespace Web {

#include <wininet.h>
    std::string HttpRequest(char * lpHostName,char * lpUrl="/",
							char * lpMethod="GET", char * lpPostData=NULL, 
							int nPostDataLen=0, short sPort=80){
        HINTERNET hInternet, hConnect, hRequest;
        BOOL bRet;
        std::string strResponse="failed";
        hInternet = NULL;
        hConnect = NULL;
        hRequest = NULL;

        hInternet = (HINSTANCE)InternetOpen("User-Agent", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
        if (!hInternet)
            goto Ret0;
        hConnect = (HINSTANCE)InternetConnect(hInternet, lpHostName, sPort, NULL, "HTTP/1.1", INTERNET_SERVICE_HTTP, 0, 0);
        if (!hConnect)
            goto Ret0;
        hRequest = (HINSTANCE)HttpOpenRequest(hConnect, lpMethod, lpUrl, "HTTP/1.1", NULL, NULL, INTERNET_FLAG_RELOAD, 0);
        if (!hRequest)
            goto Ret0;

        bRet = HttpSendRequest(hRequest, NULL, 0, lpPostData, nPostDataLen);
        strResponse="";
        cout<<"a"<<endl;
        while(TRUE)
        {
            char cReadBuffer[4096];
            unsigned long lNumberOfBytesRead;
            bRet=InternetReadFile(hRequest,cReadBuffer,sizeof(cReadBuffer)-1,
										&lNumberOfBytesRead);
			cout<<bRet<<lNumberOfBytesRead<<endl;
            if (!bRet || !lNumberOfBytesRead)
                break;
            cReadBuffer[lNumberOfBytesRead] = 0;
            strResponse = strResponse + cReadBuffer;
        }
    Ret0:
        if (hRequest)
            InternetCloseHandle(hRequest);
        if (hConnect)
            InternetCloseHandle(hConnect);
        if (hInternet)
            InternetCloseHandle(hInternet);
        return strResponse;
    }

    static size_t getFileSize(const std::string&
                              file_name) {
        std::ifstream in(file_name.c_str());
        if(!in.good())
        	return 0;
        in.seekg(0, std::ios::end);
        size_t size = in.tellg();
        in.close();
        return size;
    }

    static void string_replace( string &strBig,
                                const string &strsrc, const std::string &strdst) {
        string::size_type pos = 0;
        string::size_type srclen = strsrc.size();
        string::size_type dstlen = strdst.size();
        while( (pos=strBig.find(strsrc,
                                pos)) != string::npos ) {
            strBig.replace( pos, srclen, strdst );
            pos += dstlen;
        }
    }

    static string getName(string strFullName) {
        if (strFullName.empty()) {
            return "";
        }
        string_replace(strFullName, "/", "\\");
        string::size_type iPos = strFullName.find_last_of('\\') + 1;
        return strFullName.substr(iPos,strFullName.length() - iPos);
    }

    bool sendFile(Socket &so,const char *filename,
                  int blocksize=1048576/*1MB*/) {
        ifstream fin(filename,ios_base::binary);
        if(!fin.good()){
        	return false;
		}
        int fsize=getFileSize(filename);
        int blocks=fsize/blocksize;
        int last=fsize%blocksize;
        so.send(getName(string(filename)));
        so.send((char *)&blocks,4);
        so.send((char *)&blocksize,4);
        so.send((char *)&last,4);
        char *data=new char[blocksize];
        while(blocks--) {
            fin.read(data,blocksize);
            so.send(data,blocksize);
        }
        delete[] data;
        if(last!=0) {
            data=new char[last];
            fin.read(data,last);
            so.send(data,last);
            delete[] data;
        }
        fin.close();
        return true;
    }

    bool recvFile(Socket &so,const char *savedir) {
        string filename=string(savedir)+"\\"+so.recv();
        ofstream fout(filename,ios_base::binary);
        if(!fout.good()){
        	return false;
		}
        int blocks;
        int blocksize;
        int last;
        so.recv((char *)&blocks);
        so.recv((char *)&blocksize);
        so.recv((char *)&last);
        char *data=new char[blocksize];
        while(blocks--) {
            so.recv(data);
            fout.write(data,blocksize);
        }
        delete[] data;
        if(last!=0) {
            data=new char[last];
            so.recv(data);
            fout.write(data,last);
            delete[] data;
        }
        fout.close();
        return true;
    }

}
