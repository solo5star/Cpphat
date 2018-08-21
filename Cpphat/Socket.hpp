#ifndef CPPHAT_SOCKET_H_
#define CPPHAT_SOCKET_H_

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <winsock2.h>

#pragma comment (lib, "ws2_32.lib")

#define wsasocket socket
#define wsaconnect connect
#define WSASOCKET SOCKET
#undef GetMessage
#undef SendMessage
#undef ERROR

#define DEFAULT_BUFFER_SIZE 512
#define DEFAULT_IP "127.0.0.1"
#define DEFAULT_PORT 1391

// WinSock2 socket function ref : https://msdn.microsoft.com/en-us/library/windows/desktop/ms740506(v=vs.85).aspx

class InternetAddress {
	std::string m_ip;
	unsigned short m_port;
public:
	InternetAddress(std::string _ip = DEFAULT_IP, const unsigned short _port = DEFAULT_PORT);
	InternetAddress& operator= (const InternetAddress _internetAddress);

	std::string GetIp() const;
	unsigned short GetPort() const;

	friend std::ostream& operator<<(std::ostream& out, InternetAddress _internetAddress);
};

class WinSocket {
	WSASOCKET m_wsasocket;
	InternetAddress m_internetAddress;

	int m_lastError;
	int m_bufferSize;
protected:
public:
	WinSocket();

	int SetNonBlockingMode(bool val);

	int GetLastError();
	InternetAddress& GetInternetAddress();
	void SetInternetAddress(const InternetAddress _internetAddress);

	int Init();

	// Server
	int Bind();
	int Listen();
	int Accept(WinSocket& client);

	// Client
	int Connect();
	int Receive(unsigned char * buffer, int size);
	int Send(unsigned char * buffer, int size);
	int Shutdown();

	int Close();
};

class Socket : public WinSocket {

};

class ServerSocket : public WinSocket {

};

class SocketError {
	int m_errorCode;
public:
	SocketError(const int _errorCode);

	friend std::ostream& operator<<(std::ostream& os, const SocketError error);
};

std::ostream& operator<<(std::ostream& os, const SocketError error);

int SocketStartup();
void SocketCleanup();

#endif