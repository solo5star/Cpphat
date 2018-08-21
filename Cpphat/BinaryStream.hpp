#ifndef CPPHAT_BINARYSTREAM_H_
#define CPPHAT_BINARYSTREAM_H_

#include <iostream>
#include <string>

class HexaDumper {
	const unsigned char * const m_buffer;
	const unsigned int m_length;
public:
	HexaDumper(const unsigned char * const _buffer, const unsigned int _length);
	void Dump(std::ostream& os) const;
};

std::ostream& operator<<(std::ostream& os, HexaDumper& dmp);

std::ostream& operator<<(std::ostream& os, const HexaDumper& dmp);

class BinaryStream {
public:
	unsigned char * Buffer;
	unsigned int Length;
	unsigned int Offset;

	BinaryStream();
	BinaryStream(unsigned int _length);
	~BinaryStream();

	void AllocateBuffer(unsigned int _length);
	void Reset();
	unsigned char operator[](unsigned int index);

	BinaryStream& operator<<(unsigned char value);
	BinaryStream& operator>>(unsigned char& value);
	unsigned int SizeOf(unsigned char value);

	BinaryStream& operator<<(unsigned short value);
	BinaryStream& operator>>(unsigned short& value);
	unsigned int SizeOf(unsigned short value);

	BinaryStream& operator<<(int value);
	BinaryStream& operator>>(int& value);
	unsigned int SizeOf(int value);

	BinaryStream& operator<<(unsigned int value);
	BinaryStream& operator>>(unsigned int& value);
	unsigned int SizeOf(unsigned int value);

	BinaryStream& operator<<(std::string& value);
	BinaryStream& operator>>(std::string& value);
	unsigned int SizeOf(std::string& value);

	BinaryStream& operator<<(BinaryStream& value);
	HexaDumper Dump();
};

#endif