#ifndef __SCRIPT_MANAGER_H__
#define __SCRIPT_MANAGER_H__

#include "lua.hpp"
#include "luatinker/lua_tinker.h"
#include "BaseTypes.h"
#include "misc/Singleton.h"
#include <string>
#include <map>
using namespace std;

namespace easygame {

class ScriptManager;

enum class ScriptType
{
	NewState,		// 新的虚拟机
	ThreadState,	// 协程
};

class ScriptObject
{
	friend class ScriptManager;

protected:
	ScriptObject(lua_State* mainState);
	virtual ~ScriptObject();

public:
	lua_State* getState() { return mLuaState; }

	// 设置lua变量
	template<typename T>
	void set(const char* name, T object)
	{
		lua_tinker::set<T>(mLuaState, name, object);
	}

	// 获得lua变量
	template<typename T>
	T get(const char* name)
	{
		return lua_tinker::get<T>(mLuaState, name);
	}

	// 定义C++全局函数到lua
	template<typename F> 
	void def(const char* name, F func)
	{ 
		lua_tinker::def(mLuaState, name, func);
	}

	bool dofile(const char* filename);
	bool dobuffer(const char* buff, size_t len);
	bool dostring(const char* script);

	// 运行一次GC清理
	void runGC();

	// 获得当前脚本占用内存大小(单位K字节)
	int getMemory();

	void close();

protected:

private:
	string mFileName;		// 文件名
	lua_State* mLuaState;	// lua虚拟机
	ScriptType mScriptType;
};


//////////////////////////////////////////////////////////////////////////

class ScriptManager : public Singleton<ScriptManager>
{
public:
	ScriptManager();
	virtual ~ScriptManager();

	// 设置脚本的根目录
	void setSciptRootPath(const char* path);
	const char* getSciptRootPath();

	ScriptObject* createScript(ScriptType type = ScriptType::NewState);
	void destroyScript(ScriptObject*& pScript);

	//ScriptObject* getScript(size_t index);

	void reladAllScript();

	// 获得所有脚本占用内存大小(单位K字节)
	int getTotalMemory();

	void printInfo();

protected:

private:
	string mScriptRootPath;
	lua_State* mLuaState;	// 主虚拟机
	map<ScriptObject*,ScriptObject*> mScriptList;
};



}	// namespace

#endif