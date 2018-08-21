#include "Socket.hpp"

InternetAddress::InternetAddress(std::string _ip, const unsigned short _port)
	: m_ip(_ip)
	, m_port(_port)
{ }

InternetAddress& InternetAddress::operator= (const InternetAddress _internetAddress) {
	m_ip = _internetAddress.m_ip;
	m_port = _internetAddress.m_port;
	return *this;
}

std::string InternetAddress::GetIp() const {
	return m_ip;
}

unsigned short InternetAddress::GetPort() const {
	return m_port;
}

std::ostream& operator<<(std::ostream& out, InternetAddress _internetAddress) {
	out << _internetAddress.m_ip.c_str() << ':' << _internetAddress.m_port;
	return out;
}

WinSocket::WinSocket() {
	memset(&m_wsasocket, 0, sizeof(m_wsasocket));
	m_lastError = 0;
	m_bufferSize = DEFAULT_BUFFER_SIZE;
}

int WinSocket::GetLastError() {
	return m_lastError;
}

InternetAddress& WinSocket::GetInternetAddress() {
	return m_internetAddress;
}

void WinSocket::SetInternetAddress(const InternetAddress _internetAddress) {
	m_internetAddress = _internetAddress;
}

int WinSocket::Init() {
	// AF_INET : The Internet Protocol version 4 (IPv4) address family
	// SOCK_STREAM : sequenced, reliable, Two-way, connection-based byte streams. TCP
	// IPPROTO_TCP : TCP
	m_wsasocket = wsasocket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_wsasocket == INVALID_SOCKET) {
		m_lastError = WSAGetLastError();
		return 1;
	}

	return 0;
}

int WinSocket::SetNonBlockingMode(bool val) {
	// Setup non-blocking mode
	// FIONBIO enables or disables the blocking mode
	// If iMode = 0, blocking is enabled
	// if iMode != 0, non-blocking mode is enabled
	u_long iMode = val;
	int result_code = ioctlsocket(m_wsasocket, FIONBIO, &iMode);
	if (result_code != 0) {
		m_lastError = WSAGetLastError();
		return 1;
	}
	return 0;
}

int WinSocket::Connect() {
	SOCKADDR_IN addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.S_un.S_addr = inet_addr(m_internetAddress.GetIp().c_str());
	addr.sin_port = htons(m_internetAddress.GetPort());

	int result_code = wsaconnect(m_wsasocket, (SOCKADDR*)&addr, sizeof(addr));
	if (result_code == INVALID_SOCKET) {
		m_lastError = WSAGetLastError();
		return 1;
	}

	// NO ERROR
	return 0;
}

int WinSocket::Bind() {
	SOCKADDR_IN addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	addr.sin_addr.S_un.S_addr = inet_addr(m_internetAddress.GetIp().c_str());
	addr.sin_port = htons(m_internetAddress.GetPort());

	int result_code = bind(m_wsasocket, (SOCKADDR*)&addr, sizeof(addr));
	if (result_code == INVALID_SOCKET) {
		m_lastError = WSAGetLastError();
		return 1;
	}
	return 0;
}

int WinSocket::Listen() {
	if (listen(m_wsasocket, SOMAXCONN) == SOCKET_ERROR) {
		m_lastError = WSAGetLastError();
		return 1;
	}
	return 0;
}

int WinSocket::Accept(WinSocket& client) {
	client.m_wsasocket = accept(m_wsasocket, NULL, NULL);
	if (client.m_wsasocket == INVALID_SOCKET) {
		m_lastError = WSAGetLastError();
		return 1;
	}
	return 0;
}

int WinSocket::Receive(unsigned char * buffer, int size) {
	// result < 0 : SOCKET_ERROR
	// result == 0 : Connection closed
	// result > 0 : Bytes received
	int result = recv(m_wsasocket, (char *)buffer, size, 0);
	if (result == SOCKET_ERROR) {
		m_lastError = WSAGetLastError();
		return result;
	}
	return result;
}

int WinSocket::Send(unsigned char * buffer, int size) {
	// result < 0 : SOCKET_ERROR
	// result > 0 : Bytes sent
	int result = send(m_wsasocket, (char *)buffer, size, 0);
	if (result == SOCKET_ERROR) {
		m_lastError = WSAGetLastError();
		return result;
	}
	return result;
}

int WinSocket::Shutdown() {
	// shutdown the connection since no more data will be sent
	if (shutdown(m_wsasocket, SD_SEND) == SOCKET_ERROR) {
		m_lastError = WSAGetLastError();
		closesocket(m_wsasocket);
		return 1;
	}
	return 0;
}

int WinSocket::Close() {
	if (closesocket(m_wsasocket) != 0) {
		m_lastError = WSAGetLastError();
		return 1;
	}
	return 0;
}

SocketError::SocketError(int _errorCode)
	: m_errorCode(_errorCode)
{ }

std::ostream& operator<<(std::ostream& os, const SocketError error) {
	switch (error.m_errorCode) {
		// Reference : https://msdn.microsoft.com/en-us/library/windows/desktop/ms740668(v=vs.85).aspx
	case WSAEINTR: os << "Interrupted function call"; break;
	case WSAEBADF: os << "File handle is not valid"; break;
	case WSAEACCES: os << "Permission denied"; break;
	case WSAEFAULT: os << "Bad address"; break;
	case WSAEINVAL: os << "Invalid argument"; break;
	case WSAEMFILE: os << "Too many open files"; break;
	case WSAEWOULDBLOCK: os << "Resource temporarily unavailable"; break;
	case WSAEINPROGRESS: os << "Operation now in progress"; break;
	case WSAEALREADY: os << "Operation already in progress"; break;
	case WSAENOTSOCK: os << "Socket operation on nonsocket"; break;
	case WSAEDESTADDRREQ: os << "Destination address required"; break;
	case WSAEMSGSIZE: os << "Message too long"; break;
	case WSAEPROTOTYPE: os << "Protocol wrong type for socket"; break;
	case WSAENOPROTOOPT: os << "Bad protocol option"; break;
	case WSAEPROTONOSUPPORT: os << "Protocol not supported"; break;
	case WSAESOCKTNOSUPPORT: os << "Socket type not supported"; break;
	case WSAEOPNOTSUPP: os << "Operation not supported"; break;
	case WSAEPFNOSUPPORT: os << "Protocol family not supported"; break;
	case WSAEAFNOSUPPORT: os << "Address family not supported by protocol family"; break;
	case WSAEADDRINUSE: os << "Address already in use"; break;
	case WSAEADDRNOTAVAIL: os << "Cannot assign requested address"; break;
	case WSAENETDOWN: os << "Network is down"; break;
	case WSAENETUNREACH: os << "Network is unreachable"; break;
	case WSAENETRESET: os << "Network dropped connection on reset"; break;
	case WSAECONNABORTED: os << "Software caused connection abort"; break;
	case WSAECONNRESET: os << "Connection reset by peer"; break;
	case WSAENOBUFS: os << "No buffer space available"; break;
		// TODO: ADD MORE
	case WSAECONNREFUSED: os << "Connection refused"; break;
		// TODO: ADD MORE
	case WSANOTINITIALISED: os << "Successful WSAStartup not yet performed"; break;
		// TODO: ADD MORE
	default: os << "Unknown error";
	}
	os << " (" << error.m_errorCode << ")";
	return os;
}

// Initialize winsock
int SocketStartup() {
	WSAData wsa_data;
	return WSAStartup(MAKEWORD(2, 2), &wsa_data);
}

void SocketCleanup() {
	WSACleanup();
}