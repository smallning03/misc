#ifndef Packet_h__
#define Packet_h__

#include "lua.hpp"
#include "VarList.h"

class Packet
{
public:
	Packet();
	virtual ~Packet();

	void addId(LUA_NUMBER val);
	void addInt8(LUA_NUMBER val);
	void addInt16(LUA_NUMBER val);
	void addInt32(LUA_NUMBER val);
	void addInt64(LUA_NUMBER val);
	void addFloat(LUA_NUMBER val);
	void addDouble(LUA_NUMBER val);
	void addString(const char *val);

	void send();

private:
	VarList m_args;
	
};

#endif // Packet_h__
