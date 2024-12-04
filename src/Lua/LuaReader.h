#pragma once


// Includes
#include <string>
#include <memory>
#include <glm/glm.hpp>

// Additional Includes
extern "C" {
    #include <lua.h>
    #include <lauxlib.h>
    #include <lualib.h>
}

// Forward Declarations
struct Level;

/**
 * @class LuaReader
 * @brief Custom static class designed to handle object spawning.
 */
class SLuaReader
{
public:
    // ---------- Global functions --------------

    static lua_State* InitLuaInterpreter();

    static int Lua_CallFib(const std::string& _luaSource, const int& _val);
    static int Lua_CallAdd(const std::string& _luaSource, const int& _num1, const int& _num2);
    // Functionality uncompressed for remembering operation context
	static int Lua_CallCreateActorData(lua_State* _interpreter, const std::string& _luaSource, std::shared_ptr<Level> _level);
    static int Lua_CallCreateActorObject(lua_State* _interpreter, const std::string& _luaSource, std::shared_ptr<Level> _level);
    static int Lua_CallCreateActorObjectArgs(lua_State* _interpreter, const std::string& _luaSource, std::shared_ptr<Level> _level, std::string _name, int _collisionType, glm::vec3 _position, glm::vec3 _scale);

	static glm::vec3 Lua_SetActorPosition(const std::string& _luaSource, lua_State* _interpreter);

private:
    // ---------- Local functions --------------

    // Reads the lua file and loads it to the interpreter
    static bool ReadFile(lua_State* _interpreter, const std::string& _luaSource);

    // Finds the input function and calls it with appropriate arguments and return numbers
    static bool LoadFunction(lua_State* _interpreter, const std::string& _functionName, const int& _numArguments, const int& _numReturnValues, const int& _errorLocation = 0);
    static bool LoadFunctionWithArgs(lua_State* _interpreter, const int& _numArguments, const int& _numReturnValues, std::string _name, int _collisionType, glm::vec3 _position, glm::vec3 _scale, const int& _errorLocation = 0);

    // Gets the string value from a lua table
    static std::string GetTableStringField(lua_State* _interpreter, int tableIndex, const char* key);

	// Gets the float value from a lua table
    static float GetTableFloatField(lua_State* _interpreter, int tableIndex, const char* key);

    // Gets the first 3 values from a table assuming their key is x, y, z
    static glm::vec3 GetTableVec3Field(lua_State* _interpreter, int tableIndex, const char* key);

    static void DebugIterateTableKeys(lua_State* _interpreter, int _tableIndex, int depth = 0);
    static void DebugLuaStack(lua_State* _interpreter);

};

