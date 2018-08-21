#include "BinaryStream.hpp"

HexaDumper::HexaDumper(const unsigned char * const _buffer, const unsigned int _length)
	: m_buffer(_buffer)
	, m_length(_length)
{ }

void HexaDumper::Dump(std::ostream& os) const {
	static const char * const hexc = "0123456789ABCDEF";
	for (unsigned int i = 0; i < m_length; i++) {
		os << hexc[(m_buffer[i] & 0xf0) >> 4];
		os << hexc[m_buffer[i] & 0x0f];
		os << ' ';
	}
}

std::ostream& operator<<(std::ostream& os, HexaDumper& dmp) {
	dmp.Dump(os);
	return os;
}

std::ostream& operator<<(std::ostream& os, const HexaDumper& dmp) {
	dmp.Dump(os);
	return os;
}

BinaryStream::BinaryStream()
	: Buffer(NULL)
	, Length(0)
	, Offset(0)
{ }

BinaryStream::BinaryStream(unsigned int _length) {
	AllocateBuffer(_length);
}

BinaryStream::~BinaryStream() {
	if (Buffer != NULL) delete[] Buffer;
}

void BinaryStream::AllocateBuffer(unsigned int _length) {
	if (Buffer != NULL) {
		delete[] Buffer;
	}
	Length = _length;
	Buffer = new unsigned char[Length + 1];
	Buffer[Length] = '\0';
}

void BinaryStream::Reset() {
	Offset = 0;
}

unsigned char BinaryStream::operator[](unsigned int index) {
	return Buffer[index];
}

BinaryStream& BinaryStream::operator<<(unsigned char value) {
	Buffer[Offset++] = value;
	return *this;
}

BinaryStream& BinaryStream::operator>>(unsigned char& value) {
	value = Buffer[Offset++];
	return *this;
}

unsigned int BinaryStream::SizeOf(unsigned char value) {
	return sizeof(value);
}

BinaryStream& BinaryStream::operator<<(unsigned short value) {
	Buffer[Offset++] = value & 0xff;
	Buffer[Offset++] = (value >> 8) & 0xff;
	return *this;
}

BinaryStream& BinaryStream::operator>>(unsigned short& value) {
	value = 0;
	value |= Buffer[Offset++];
	value |= Buffer[Offset++] << 8;
	return *this;
}

unsigned int BinaryStream::SizeOf(unsigned short value) {
	return sizeof(value);
}

BinaryStream& BinaryStream::operator<<(int value) {
	Buffer[Offset++] = value & 0xff;
	Buffer[Offset++] = (value >> 8) & 0xff;
	Buffer[Offset++] = (value >> 16) & 0xff;
	Buffer[Offset++] = (value >> 24) & 0xff;
	return *this;
}

BinaryStream& BinaryStream::operator>>(int& value) {
	value = 0;
	value |= Buffer[Offset++];
	value |= Buffer[Offset++] << 8;
	value |= Buffer[Offset++] << 16;
	value |= Buffer[Offset++] << 24;
	return *this;
}

unsigned int BinaryStream::SizeOf(int value) {
	return sizeof(value);
}

BinaryStream& BinaryStream::operator<<(unsigned int value) {
	Buffer[Offset++] = value & 0xff;
	Buffer[Offset++] = (value >> 8) & 0xff;
	Buffer[Offset++] = (value >> 16) & 0xff;
	Buffer[Offset++] = (value >> 24) & 0xff;
	return *this;
}

BinaryStream& BinaryStream::operator>>(unsigned int& value) {
	value = 0;
	value |= Buffer[Offset++];
	value |= Buffer[Offset++] << 8;
	value |= Buffer[Offset++] << 16;
	value |= Buffer[Offset++] << 24;
	return *this;
}

unsigned int BinaryStream::SizeOf(unsigned int value) {
	return sizeof(value);
}

BinaryStream& BinaryStream::operator<<(std::string& value) {
	*this << value.length();
	for (size_t i = 0; i < value.length(); i++)
		*this << (unsigned char)value[i];

	return *this;
}

BinaryStream& BinaryStream::operator>>(std::string& value) {
	unsigned int len;
	*this >> len;
	for (unsigned int i = 0; i < len; i++) {
		unsigned char c;
		*this >> c;
		value += (unsigned char)c;
	}

	return *this;
}

unsigned int BinaryStream::SizeOf(std::string& value) {
	return sizeof(unsigned int) + sizeof(unsigned char) * value.length();
}

BinaryStream& BinaryStream::operator<<(BinaryStream& value) {
	for (size_t i = 0; i < value.Length; i++)
		*this << value[i];

	return *this;
}

HexaDumper BinaryStream::Dump() {
	return HexaDumper(Buffer, Length);
}