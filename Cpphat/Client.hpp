#ifndef CPPHAT_CLIENT_H_
#define CPPHAT_CLIENT_H_

#include <iostream>
#include "Session.hpp"

class Client {
	Session* m_session = NULL;
	bool m_tryConnect = false;
	bool m_connected = false;
	bool m_closed = true;

	static std::ostream* os;

public:
	static Client Instance;

	static void SetOutputStream(std::ostream& _os);
	static std::ostream& GetOutputStream();

	~Client();
	int Init();
	void Tick();
	void Close();

	bool IsConnected() { return m_connected; };
	bool IsClosed() { return m_closed; };

	void SendPacket(Packet& packet);
	void SendMessage(std::string message);
};

class ClientSessionHandler : public SessionHandler {
	Client& m_client;
public:
	ClientSessionHandler(Session& _session, Client& _client);

	bool Handle(UnknownPacket& packet);
	bool Handle(ChatPacket& packet);
	bool Handle(AddUserPacket& packet);
	bool Handle(RemoveUserPacket& packet);
};

#endif