#include "Client.hpp"

std::ostream* Client::os = &std::cout;

Client Client::Instance = Client();

void Client::SetOutputStream(std::ostream& _os) {
	os = &_os;
}

std::ostream& Client::GetOutputStream() {
	return *os;
}

Client::~Client() {
	Close();
}

int Client::Init() {
	m_session = new Session(-1);
	m_tryConnect = false;
	m_connected = false;
	m_closed = false;

	m_session->SetHandler(new ClientSessionHandler(*m_session, *this));

	Logger::Info() << "Starting client ..." << Logger::EOL;

	Socket& socket = m_session->GetSocket();
	if (socket.Init()) {
		Logger::Error() << SocketError(socket.GetLastError()) << Logger::EOL;
		return 1;
	}
	Logger::Info() << "Client initialized" << Logger::EOL;

	Logger::Info() << "Try connecting..." << Logger::EOL;
	m_tryConnect = true;

	return 0;
}

void Client::Tick() {
	Socket& socket = m_session->GetSocket();

	// Connect procedure
	if (m_tryConnect) {
		socket.SetInternetAddress(InternetAddress(Configure::Ini.GetString("client-ip", "127.0.0.1"), Configure::Ini.GetInt("client-port", 1391)));
		Logger::Info() << "Connecting to " << socket.GetInternetAddress() << Logger::EOL;

		if (socket.Connect()) {
			int errorCode = socket.GetLastError();

			Logger::Error() << SocketError(socket.GetLastError()) << Logger::EOL;
			m_tryConnect = false;
		}
		// Successfully connected
		else {
			Logger::Info() << "Connection established" << Logger::EOL;
			socket.SetNonBlockingMode(true);
			m_tryConnect = false;
			m_connected = true;
		}
	}

	if (m_connected) {
		if (m_session->IsClosed()) {
			Close();
			delete m_session;
		}
		else {
			m_session->ReceivePacket();
		}
	}
}

void Client::Close() {
	if (m_closed) {
		return;
	}
	m_session->Close();
	delete m_session;

	m_tryConnect = false;
	m_connected = false;

	GetOutputStream() << "Connection closed" << Logger::EOL;
	Logger::Info() << "Connection closed" << Logger::EOL;

	m_closed = true;
}

void Client::SendPacket(Packet& packet) {
	if (!m_connected) {
		Logger::Debug() << "Tried to send packet without connection" << Logger::EOL;
		return;
	}
	m_session->SendPacket(packet);
}

void Client::SendMessage(std::string message) {
	ChatPacket packet;
	packet.UserId = -1;
	packet.Message = message;

	SendPacket(packet);
}

ClientSessionHandler::ClientSessionHandler(Session& _session, Client& _client)
	: SessionHandler(_session)
	, m_client(_client)
{ }

bool ClientSessionHandler::Handle(UnknownPacket& packet) {
	return false;
}

bool ClientSessionHandler::Handle(ChatPacket& packet) {
	Client::GetOutputStream() << "[" << packet.UserId << "] " << packet.Message << Logger::EOL;
	return true;
}

bool ClientSessionHandler::Handle(AddUserPacket& packet) {
	Client::GetOutputStream() << "User " << packet.UserId << " joined the chat" << Logger::EOL;
	return true;
}

bool ClientSessionHandler::Handle(RemoveUserPacket& packet) {
	Client::GetOutputStream() << "User " << packet.UserId << " quit the chat" << Logger::EOL;
	return true;
}