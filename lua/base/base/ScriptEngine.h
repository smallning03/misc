#ifndef ScriptState_h__
#define ScriptState_h__

#include <string>
#include <map>
#include "lua.hpp"

typedef int LUA_FUNCTION;

class VarList;
class ScriptEngine
{
private:
	ScriptEngine();
public:
	static ScriptEngine *instance();
	virtual ~ScriptEngine();

	bool start();
	void stop();

	inline lua_State *state() { return m_luaState; }

	void registerEvent(const char *name, LUA_FUNCTION fn);
	void unregisterEvent(const char *name);
	void callEvent(const char *name, const VarList &args, VarList &result);

	LUA_FUNCTION getHandler(const char *name);

private:
	lua_State *m_luaState;
	typedef std::map<std::string, LUA_FUNCTION> FUNCTION_MAP;
	typedef std::pair<std::string, LUA_FUNCTION> FUNCTION_MAP_PAIR;
	FUNCTION_MAP m_functionMap;
};

#endif // ScriptState_h__