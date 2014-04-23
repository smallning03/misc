#ifndef BitStream_h__
#define BitStream_h__

#include <stdint.h>

class BitStream
{
public:
	BitStream();
	BitStream(unsigned char *data, size_t len);
	virtual ~BitStream();

	inline uint8_t *buffer() { return m_buffer; }
	inline size_t size() { return m_write_pos; }
	inline void clear() { m_read_pos = 0; m_write_pos = 0; }
	void cut(size_t bytes);

	int8_t readInt8();
	int16_t readInt16();
	int32_t readInt32();
	int64_t readInt64();
	float readFloat();
	double readDouble();
	uint8_t *readData(uint8_t *data, size_t len);	
	
	void writeInt8(int8_t val);
	void writeInt16(int16_t val);
	void writeInt32(int32_t val);
	void writeInt64(int64_t val);
	void writeFloat(float val);
	void writeDouble(double val);
	void writeData(uint8_t *data, size_t len);

private:
	void reserve(size_t bytes);

private:
	uint8_t *m_buffer;
	size_t m_buffer_size;
	size_t m_read_pos;
	size_t m_write_pos;
};

#endif // BitStream_h__