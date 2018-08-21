#include "Session.hpp"

unsigned short Packet::GetPacketType() {
	return 0;
}

void Packet::Encode() {
	if (m_isEncoded) {
		return;
	}
	AllocatePayload();
	EncodeHeader();
	EncodeData();

	m_isEncoded = true;
}

void Packet::AllocatePayload() {
	//	// bytes : data
	//	// 4 : int, packet length (with this)
	//	// 2 : unsigned short, packet type
	AllocateBuffer(SizeOf(Length) + SizeOf(GetPacketType()) + GetDataLength());
}

void Packet::EncodeHeader() {
	*this << Length;
	*this << GetPacketType();
}

unsigned short UnknownPacket::GetPacketType() {
	return Packet::UNKNOWN;
}

unsigned int UnknownPacket::GetDataLength() {
	return 0;
}

void UnknownPacket::EncodeData() {

}

void UnknownPacket::DecodeData() {

}

bool UnknownPacket::Handle(SessionHandler& handler) {
	return false;
};

unsigned short ChatPacket::GetPacketType() {
	return Packet::CHAT;
}

unsigned int ChatPacket::GetDataLength() {
	return SizeOf(UserId) + SizeOf(Message);
}

void ChatPacket::EncodeData() {
	*this << UserId;
	*this << Message;
}

void ChatPacket::DecodeData() {
	*this >> UserId;
	*this >> Message;
}

bool ChatPacket::Handle(SessionHandler& session) {
	return session.Handle(*this);
}

unsigned short AddUserPacket::GetPacketType() {
	return Packet::ADD_USER;
}

unsigned int AddUserPacket::GetDataLength() {
	return SizeOf(UserId);
}

void AddUserPacket::EncodeData() {
	*this << UserId;
}

void AddUserPacket::DecodeData() {
	*this >> UserId;
}

bool AddUserPacket::Handle(SessionHandler& handler) {
	return handler.Handle(*this);
}

unsigned short RemoveUserPacket::GetPacketType() {
	return Packet::REMOVE_USER;
}

unsigned int RemoveUserPacket::GetDataLength() {
	return SizeOf(UserId);
}

void RemoveUserPacket::EncodeData() {
	*this << UserId;
}

void RemoveUserPacket::DecodeData() {
	*this >> UserId;
}

bool RemoveUserPacket::Handle(SessionHandler& handler) {
	return handler.Handle(*this);
}

Session::Session(const int _id)
	: m_id(_id)
	, m_binaryStream(256)
	, m_handler(NULL)
	, m_closed(false)
{ }

Session::~Session() {
	if (m_handler != NULL) delete m_handler;
}

int Session::GetId() {
	return m_id;
}

Socket& Session::GetSocket() {
	return m_socket;
}

void Session::SetHandler(SessionHandler* _handler) {
	m_handler = _handler;
}

void Session::SendPacket(Packet& packet) {
	//packet.AllocatePayload();
	//packet.EncodeHeader();
	//packet.EncodeData();
	packet.Reset();
	packet.Encode();

	m_socket.Send(packet.Buffer, packet.Length);
	Logger::Debug() << "Send bytes to " << m_socket.GetInternetAddress()
		<< " : " << packet.Dump() << Logger::EOL;
}

void Session::ReceivePacket() {
	// Receive packet and set received size
	m_binaryStream.Reset();
	int len = m_socket.Receive(m_binaryStream.Buffer, m_binaryStream.Length);

	// Error
	if (len < 0) switch (m_socket.GetLastError()) {
	case WSAEWOULDBLOCK: break;
	case WSAECONNRESET:
		Logger::Error() << SocketError(m_socket.GetLastError()) << Logger::EOL;
		Close();
		break;
	default: Logger::Error() << SocketError(m_socket.GetLastError()) << Logger::EOL;
	}

	// Connection closed
	else if (len == 0) {
		Logger::Debug() << "Could not receive data, close session" << Logger::EOL;
		Close();
	}

	// Received packet
	else if (len > 0) {
		unsigned int pklen;
		unsigned short pktype;
		m_binaryStream >> pklen;
		m_binaryStream >> pktype;

		pklen = pklen - sizeof(unsigned int) - sizeof(unsigned short);

		Packet* pk;

		switch (pktype) {
		case Packet::CHAT: pk = new ChatPacket(); break;
		case Packet::ADD_USER: pk = new AddUserPacket(); break;
		case Packet::REMOVE_USER: pk = new RemoveUserPacket(); break;
		default: pk = new UnknownPacket(); break;
		}

		// Decode header of packet will skip 
		pk->AllocateBuffer(pklen);
		const unsigned char * src = m_binaryStream.Buffer + m_binaryStream.Offset;
		unsigned char * dest = pk->Buffer;
		for (unsigned int i = 0; i < pklen; i++) {
			dest[i] = src[i];
		}

		Logger::Debug() << "Receive bytes on session " << m_id << " : "
			<< pk->Dump() << Logger::EOL;

		pk->DecodeData();

		if (!pk->Handle(*this->m_handler)) {
			Logger::Warn() << "Handle packet failed, type=" << pktype << ", length=" << pklen << Logger::EOL;
		}
		delete pk;
	}
}

bool Session::IsClosed() {
	return m_closed;
}

void Session::Close() {
	if (m_closed) return;

	m_socket.Shutdown();
	m_socket.Close();
	m_closed = true;

	Logger::Info() << "Close session " << m_id << Logger::EOL;
}

SessionHandler::SessionHandler(Session& _session)
	: m_session(_session)
{ }