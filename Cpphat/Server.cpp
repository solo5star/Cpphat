#include "Server.hpp"

Server Server::Instance = Server();

Server::~Server() {
	Close();
}

int Server::Init() {
	m_socket = new ServerSocket();
	m_sessionCounter = 0;
	m_waitingSession = NULL;
	m_listening = false;
	m_closed = false;

	Logger::Info() << "Starting server..." << Logger::EOL;

	Logger::Info() << "Initialize socket..." << Logger::EOL;
	if (m_socket->Init()) {
		Logger::Error() << SocketError(m_socket->GetLastError()) << Logger::EOL;
		return 1;
	}

	m_socket->SetInternetAddress(InternetAddress(Configure::Ini.GetString("server-ip", "127.0.0.1"), Configure::Ini.GetInt("server-port", 1391)));
	Logger::Info() << "Bind on " << m_socket->GetInternetAddress() << Logger::EOL;
	if (m_socket->Bind()) {
		Logger::Error() << SocketError(m_socket->GetLastError());
		return 1;
	}

	Logger::Info() << "Listen ..." << Logger::EOL;
	if (m_socket->Listen()) {
		Logger::Error() << SocketError(m_socket->GetLastError());
		return 1;
	}
	m_socket->SetNonBlockingMode(true);
	m_listening = true;

	Logger::Info() << "Server initialized" << Logger::EOL;

	return 0;
}

void Server::Tick() {
	if (!m_listening) {
		return;
	}
	//Logger::Debug() << "Tried to tick" << Logger::EOL;
	// Accept procedure
	if (m_waitingSession == NULL) {
		m_waitingSession = new Session(m_sessionCounter++);
		m_waitingSession->SetHandler(new ServerSessionHandler(*m_waitingSession, *this));
	}
	if (m_socket->Accept(m_waitingSession->GetSocket())) {
		if (m_socket->GetLastError() != WSAEWOULDBLOCK)
			Logger::Error() << SocketError(m_socket->GetLastError());
	}
	// Accept success
	else {
		m_sessions[m_waitingSession->GetId()] = m_waitingSession;
		m_waitingSession->GetSocket().SetNonBlockingMode(true);
		Logger::Info() << "Session " << m_waitingSession->GetId() << " accepted" << Logger::EOL;

		// broadcast join
		AddUserPacket packet;
		packet.UserId = m_waitingSession->GetId();
		this->BroadcastPacket(packet);

		m_waitingSession = NULL;
	}

	// Recv procedure
	auto kv = m_sessions.begin();
	while(kv != m_sessions.end()) {
		// detect close
		if (kv->second->IsClosed()) {
			int id = kv->first;
			++kv;
			RemoveSession(id); // key
		}
		else {
			kv->second->ReceivePacket();
			++kv;
		}
	}
}

void Server::RemoveSession(int id) {
	if (m_sessions.find(id) == m_sessions.end()) {
		return;
	}
	Session* toRemove = m_sessions[id];
	m_sessions.erase(id);

	// alert he is disconnected
	RemoveUserPacket pk;
	pk.UserId = toRemove->GetId();
	BroadcastPacket(pk);

	// delete from heap
	delete toRemove;
	return;
}

bool Server::IsClosed() {
	return m_closed;
}

Session* Server::GetSession(int id) {
	return m_sessions[id];
}

void Server::Close() {
	if (m_closed) {
		return;
	}
	for (auto kv : m_sessions) {
		RemoveSession(kv.first); // key
	}
	if (m_waitingSession != NULL) {
		delete m_waitingSession;
		m_waitingSession = NULL;
	}
	delete m_socket;
	m_socket = NULL;

	m_sessionCounter = 0;
	m_listening = false;
	m_closed = true;
}

void Server::BroadcastPacket(Packet& packet) {
	for (const auto& kv : m_sessions) {
		kv.second->SendPacket(packet);
	}
}

ServerSessionHandler::ServerSessionHandler(Session& _session, Server& _server)
	: SessionHandler(_session)
	, m_server(_server)
{ }

bool ServerSessionHandler::Handle(UnknownPacket& packet) {
	return false;
}

bool ServerSessionHandler::Handle(ChatPacket& packet) {
	packet.UserId = m_session.GetId();
	m_server.BroadcastPacket(packet);
	return true;
}

bool ServerSessionHandler::Handle(AddUserPacket& packet) {
	return false;
}

bool ServerSessionHandler::Handle(RemoveUserPacket& packet) {
	return false;
}
