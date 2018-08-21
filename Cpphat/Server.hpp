#ifndef CPPHAT_SERVER_H_
#define CPPHAT_SERVER_H_

#include <string>
#include "Session.hpp"

class Server {
	ServerSocket* m_socket = NULL;

	// <Session-id, Session>
	int m_sessionCounter = 0;
	std::map<int, Session*> m_sessions;
	Session* m_waitingSession = NULL;
	bool m_listening = false;
	bool m_closed = true;

protected:
	void RemoveSession(int id);

public:
	static Server Instance;

	~Server();

	int Init();
	void Tick();
	void Close();

	bool IsClosed();

	Session* GetSession(int id);

	void BroadcastPacket(Packet& packet);
};

class ServerSessionHandler : public SessionHandler {
	Server& m_server;
public:
	ServerSessionHandler(Session& _session, Server& _server);

	bool Handle(UnknownPacket& packet);
	bool Handle(ChatPacket& packet);
	bool Handle(AddUserPacket& packet);
	bool Handle(RemoveUserPacket& packet);
};

#endif