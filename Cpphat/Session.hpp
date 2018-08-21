#ifndef CPPHAT_SESSION_H_
#define CPPHAT_SESSION_H_

#include "Socket.hpp"
#include "BinaryStream.hpp"
#include "Logger.hpp"

class SessionHandler;

class Packet : public BinaryStream {
protected:
	bool m_isEncoded = false;
public:
	enum : unsigned short {
		UNKNOWN = 0,
		CHAT = 1,
		ADD_USER = 2,
		REMOVE_USER = 3
	};

	void Encode();

	void AllocatePayload();

	void EncodeHeader();

	virtual unsigned short GetPacketType() = 0;
	virtual unsigned int GetDataLength() = 0;
	virtual void EncodeData() = 0;
	virtual void DecodeData() = 0;

	virtual bool Handle(SessionHandler& handler) = 0;
};

class UnknownPacket : public Packet {
	unsigned short GetPacketType();
	unsigned int GetDataLength();
	void EncodeData();
	void DecodeData();
	bool Handle(SessionHandler& handler);
};

class ChatPacket : public Packet {
public:
	int UserId;
	std::string Message;

	unsigned short GetPacketType();
	unsigned int GetDataLength();
	void EncodeData();
	void DecodeData();
	bool Handle(SessionHandler& session);
};

class AddUserPacket : public Packet {
public:
	int UserId;

	unsigned short GetPacketType();
	unsigned int GetDataLength();
	void EncodeData();
	void DecodeData();
	bool Handle(SessionHandler& handler);
};

class RemoveUserPacket : public Packet {
public:
	int UserId;

	unsigned short GetPacketType();
	unsigned int GetDataLength();
	void EncodeData();
	void DecodeData();
	bool Handle(SessionHandler& handler);
};

class Session {
	const int m_id;
	Socket m_socket;
	BinaryStream m_binaryStream;
	SessionHandler* m_handler;
	bool m_closed;

public:
	Session(const int _id);

	~Session();

	int GetId();
	Socket& GetSocket();

	void SetHandler(SessionHandler* _handler);

	void SendPacket(Packet& packet);
	void ReceivePacket();

	bool IsClosed();
	void Close();
};

class SessionHandler {
protected:
	Session & m_session;

	SessionHandler(Session& _session);

public:
	virtual bool Handle(UnknownPacket& packet) = 0;
	virtual bool Handle(ChatPacket& packet) = 0;
	virtual bool Handle(AddUserPacket& packet) = 0;
	virtual bool Handle(RemoveUserPacket& packet) = 0;
};

#endif