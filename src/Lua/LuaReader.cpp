// Includes
#include <Lua/LuaReader.h>
#include <Utilities/Defines.h>
#include <Utilities/Logger.h>

#include "LevelActors/BaseActor.h"
#include "RenderElements/MeshTypes/DefaultMesh.h"
#include "Levels/Level.h"

// Additional includes


int SLuaReader::Lua_CallFib(const std::string& _luaSource, const int& _val)
{
	// Load interpreter
	lua_State* interpreter = InitLuaInterpreter();

	// Read File ---
	const char* luaFileDir = _luaSource.c_str();
	luaL_dofile(interpreter, luaFileDir);

	// Get the lua function called:
	lua_getglobal(interpreter, "fib");

	// Push the argument to the lua function
	lua_pushnumber(interpreter, _val);

	// call the function with 1 argument, returning 1 result
	lua_call(interpreter, 1, 1);

	// Get the result from the function on the lua stack
	int result = (int)lua_tointeger(interpreter, -1);

	// Remove result to clean up
	lua_pop(interpreter, 1);

	// Clean up
	lua_close(interpreter);

	return result;
}

int SLuaReader::Lua_CallAdd(const std::string& _luaSource, const int& _num1, const int& _num2)
{
	// Load interpreter
	lua_State* interpreter = InitLuaInterpreter();

	// Read File ---
	const char* luaFileDir = _luaSource.c_str();
	luaL_dofile(interpreter, luaFileDir);

	// Get the lua function called:
	lua_getglobal(interpreter, "Add");

	// Push the argument to the lua function
	lua_pushnumber(interpreter, _num1);
	lua_pushnumber(interpreter, _num2);

	// call the function with 2 argument, returning 1 result
	lua_call(interpreter, 2, 1);

	// Get the result from the function on the lua stack
	int result = (int)lua_tointeger(interpreter, -1);

	// Remove result to clean up
	lua_pop(interpreter, 1);

	// Clean up
	lua_close(interpreter);

	return result;
}

int SLuaReader::Lua_CallCreateActorData(lua_State* _interpreter, const std::string& _luaSource, std::shared_ptr<Level> _level)
{
	ReadFile(_interpreter, _luaSource);

	LoadFunction(_interpreter, "CreateActor", 0, 4);

	DebugLuaStack(_interpreter);

	// Get the first return value, (name) ----------------------
	std::string name = lua_tostring(_interpreter, 1); // Gets the first return value at the top of the stack being the name, then pops it off

	// The function return parameters are not on the local stack and therefore do not need to be popped of the stack
	// only values i push to the stack through c++ are values i need to pop manually, everything else is managed by the interpreter
	// lua_pop(_interpreter,1);

	// Get the second return value, (x,y,z positions) ----------------------
	luaL_checktype(_interpreter, 2, LUA_TTABLE);	// Auto throws error if type miss match

	// X
	lua_pushstring(_interpreter, "x");// pushes the key "x" (table key defined in lua code) to the top of the interpreter stack
	lua_gettable(_interpreter, 2); // checks the table located at index 2 with the key at index 1 and adds the value to the top of the stack, consumes the table key
	float posX = static_cast<float>(luaL_checknumber(_interpreter, -1)); // check number at the top of the stack, making sure its a number
	lua_pop(_interpreter, 1); // pops 1 element from the top of the stack, (not the first element) the number created by the table

	// Y
	lua_pushstring(_interpreter, "y");
	lua_gettable(_interpreter, 2); 
	float posY = static_cast<float>(luaL_checknumber(_interpreter, -1));
	lua_pop(_interpreter, 1); 

	// Z
	lua_pushstring(_interpreter, "z");
	lua_gettable(_interpreter, 2);
	float posZ = static_cast<float>(luaL_checknumber(_interpreter, -1));
	lua_pop(_interpreter, 1);
	glm::vec3 position = glm::vec3(posX, posY, posZ);


	// Get the third return value, (x,y,z scale) ----------------------
	luaL_checktype(_interpreter, 2, LUA_TTABLE);	// Auto throws error if type miss match

	// X
	lua_pushstring(_interpreter, "x");// pushes the key "x" (table key defined in lua code) to the top of the interpreter stack
	lua_gettable(_interpreter, 3); // checks the table located at index 2 with the key at index 1 and adds the value to the top of the stack, consumes the table key
	float scaleX = static_cast<float>(luaL_checknumber(_interpreter, -1)); // check number at the top of the stack, making sure its a number
	lua_pop(_interpreter, 1); // pops 1 element from the top of the stack, (not the first element) the number created by the table

	// Y
	lua_pushstring(_interpreter, "y");
	lua_gettable(_interpreter, 3);
	float scaleY = static_cast<float>(luaL_checknumber(_interpreter, -1));
	lua_pop(_interpreter, 1);

	// Z
	lua_pushstring(_interpreter, "z");
	lua_gettable(_interpreter, 3);
	float scaleZ = static_cast<float>(luaL_checknumber(_interpreter, -1));
	lua_pop(_interpreter, 1);
	glm::vec3 scale = glm::vec3(scaleX, scaleY, scaleZ);

	// Get the fourth return value, (collisionType) ----------------------

	int collisionTypeNum = static_cast<int>(luaL_checknumber(_interpreter, 4));
	lua_pop(_interpreter, 1);
	CollisionBase collisionBase = CollisionBase::Compare;
	switch (collisionTypeNum)
	{
		case 1:
			collisionBase = CollisionBase::AABB;
			break;
		default:
			LOG_WARNING("Lua::CollisionTypeMissmatch");
		break;
	}

	lua_pop(_interpreter, 3);
	DebugLuaStack(_interpreter);

	std::shared_ptr<BaseActor> Cube1 = std::make_shared<BaseActor>(name, Mesh::CreateCube(nullptr), collisionBase, position, scale);
	_level->AddActorToSceneGraph(Cube1);
	Cube1->SetLuaControl(false);

	return 0;
}

int SLuaReader::Lua_CallCreateActorObject(lua_State* _interpreter, const std::string& _luaSource, std::shared_ptr<Level> _level)
{
	ReadFile(_interpreter, _luaSource);

	LoadFunction(_interpreter,"newObject" , 0, 1);

	//DebugLuaStack(_interpreter);

	if(lua_istable(_interpreter,-1))
	{
		std::string name = GetTableStringField(_interpreter, 1, "name");
		int collisionType = static_cast<int>(GetTableFloatField(_interpreter,1,"collisionType"));
		CollisionBase collisionBase = CollisionBase::Compare;
		if (collisionType == 1)
			collisionBase = CollisionBase::AABB;
		glm::vec3 position = GetTableVec3Field(_interpreter, 1, "position");
		glm::vec3 scale = GetTableVec3Field(_interpreter, 1, "scale");

		std::shared_ptr<BaseActor> Cube1 = std::make_shared<BaseActor>(name, Mesh::CreateCube(nullptr), collisionBase, position, scale);
		_level->AddActorToSceneGraph(Cube1);
		Cube1->SetLuaControl(true);

	} else
		LOG_ERROR("Lua stack index not table for object->Actor converting");

	lua_pop(_interpreter, 1);

	return 0;
}

int SLuaReader::Lua_CallCreateActorObjectArgs(lua_State* _interpreter, const std::string& _luaSource, std::shared_ptr<Level> _level, 
	std::string _name, int _collisionType, glm::vec3 _position, glm::vec3 _scale)
{
	ReadFile(_interpreter, _luaSource);

	//DebugLuaStack(_interpreter);

	LoadFunctionWithArgs(_interpreter, 4, 1,_name,_collisionType,_position,_scale);

	if (lua_istable(_interpreter, -1))
	{
		std::string name = GetTableStringField(_interpreter, 1, "name");
		int collisionType = static_cast<int>(GetTableFloatField(_interpreter, 1, "collisionType"));
		CollisionBase collisionBase = CollisionBase::Compare;
		if (collisionType == 1)
			collisionBase = CollisionBase::AABB;
		glm::vec3 position = GetTableVec3Field(_interpreter, 1, "position");
		glm::vec3 scale = GetTableVec3Field(_interpreter, 1, "scale");

		std::shared_ptr<BaseActor> Cube1 = std::make_shared<BaseActor>(name, Mesh::CreateCube(nullptr), collisionBase, position, scale);
		_level->AddActorToSceneGraph(Cube1);
		Cube1->SetLuaControl(false);

	}
	else
		LOG_ERROR("Lua stack index not table for object->Actor converting");

	lua_pop(_interpreter, 1);

	return 0;
}

glm::vec3 SLuaReader::Lua_SetActorPosition(const std::string& _luaSource, lua_State* _interpreter)
{
	ReadFile(_interpreter, _luaSource);

	LoadFunction(_interpreter, "GetPosition", 0, 1);

	//DebugLuaStack(_interpreter);

	glm::vec3 pos;

	if (lua_istable(_interpreter, -1))
	{
		pos.x = GetTableFloatField(_interpreter, 1, "x");
		pos.y = GetTableFloatField(_interpreter, 1, "y");
		pos.z = GetTableFloatField(_interpreter, 1, "z");

	}
	else
		LOG_ERROR("Lua stack index not table position updateing");

	lua_pop(_interpreter, 1);

	return pos;
}

bool SLuaReader::ReadFile(lua_State* _interpreter, const std::string& _luaSource)
{
	// Convert string to const char 
	const char* fileSrc = _luaSource.c_str();

	// Check if do file reads correctly
	if (luaL_dofile(_interpreter, fileSrc) != LUA_OK)
	{
		const char* error = lua_tostring(_interpreter, -1); // get the error message from first stack element
		LOG_ERROR("Lua Error: %s", error); // print error
		lua_pop(_interpreter, 1); // Remove error message from stack
		return false;
	}
	return true;
}

bool SLuaReader::LoadFunction(lua_State* _interpreter, const std::string& _functionName, const int& _numArguments, const int& _numReturnValues, const int& _errorLocation)
{
	// Load function from source code
	lua_getglobal(_interpreter, _functionName.c_str());

	// Call function (use protected call rather than call for error handling, last arg is location on stack, def = 0)
	if(lua_pcall(_interpreter, _numArguments, _numReturnValues, _errorLocation) != LUA_OK)
	{
		const char* error = lua_tostring(_interpreter, -1); // get the error message from first stack element
		LOG_ERROR("Lua Call Error: %s", error); // print error
		lua_pop(_interpreter, 1); // Remove error message from stack
		return false;
	}
	return true;
}

bool SLuaReader::LoadFunctionWithArgs(lua_State* _interpreter, const int& _numArguments, const int& _numReturnValues,
	std::string _name, int _collisionType, glm::vec3 _position, glm::vec3 _scale, const int& _errorLocation)
{
	//// Get the `Actor:new` function
	lua_getglobal(_interpreter, "newObject");          // Pushes `Actor` table onto stack
	//lua_getfield(_interpreter, -1, "new");         // Pushes `Actor.new` function onto stack
	//lua_remove(_interpreter, -2);                  // Remove `Actor` table, keeping `new` function

	// Push arguments for `Actor:new` important for pcall to not call error, even through the lua function can use default values

	// Push name
	lua_pushstring(_interpreter, _name.c_str());

	// Push collision type
	lua_pushinteger(_interpreter, _collisionType);

	// Push position table
	lua_newtable(_interpreter);
	lua_pushstring(_interpreter, "x"); lua_pushnumber(_interpreter, _position.x); lua_settable(_interpreter, -3);
	lua_pushstring(_interpreter, "y"); lua_pushnumber(_interpreter, _position.y); lua_settable(_interpreter, -3);
	lua_pushstring(_interpreter, "z"); lua_pushnumber(_interpreter, _position.z); lua_settable(_interpreter, -3);

	// Push scale table
	lua_newtable(_interpreter);
	lua_pushstring(_interpreter, "x"); lua_pushnumber(_interpreter, _scale.x); lua_settable(_interpreter, -3);
	lua_pushstring(_interpreter, "y"); lua_pushnumber(_interpreter, _scale.y); lua_settable(_interpreter, -3);
	lua_pushstring(_interpreter, "z"); lua_pushnumber(_interpreter, _scale.z); lua_settable(_interpreter, -3);

	// Call function
	if (lua_pcall(_interpreter, _numArguments, _numReturnValues, _errorLocation) != LUA_OK)
	{
		const char* error = lua_tostring(_interpreter, -1); 
		LOG_ERROR("Lua Call Error: %s", error);
		lua_pop(_interpreter, 1);
		return false;
	}

	return true;
}

std::string SLuaReader::GetTableStringField(lua_State* _interpreter, int tableIndex, const char* key)
{	
	lua_pushstring(_interpreter, key);						// Push the key
	lua_gettable(_interpreter, tableIndex);					// Get the value associated with the key
	std::string value = lua_tostring(_interpreter, -1);		// Convert it to a string
	lua_pop(_interpreter, 1);					   			// Pop the value from the stack
	return value;
}

float SLuaReader::GetTableFloatField(lua_State* _interpreter, int tableIndex, const char* key)
{
	lua_pushstring(_interpreter, key);									// Push the key
	lua_gettable(_interpreter, tableIndex);								// Get the value associated with the key
	float value = static_cast<float>(lua_tonumber(_interpreter, -1));	// Convert it to a number
	lua_pop(_interpreter, 1);											// Pop the value from the stack
	return value;
}

glm::vec3 SLuaReader::GetTableVec3Field(lua_State* _interpreter, int tableIndex, const char* key)
{
	lua_pushstring(_interpreter, key);           // Push the key
	lua_gettable(_interpreter, tableIndex);      // Get the value (assumed to be a table)
	glm::vec3 vec;
	if (lua_istable(_interpreter, -1)) {
		vec.x = GetTableFloatField(_interpreter, tableIndex+1, "x");
		vec.y = GetTableFloatField(_interpreter, tableIndex + 1, "y");
		vec.z = GetTableFloatField(_interpreter, tableIndex + 1, "z");
	}
	lua_pop(_interpreter, 1);                    // Pop the vec3 table from the stack
	return vec;
}

void SLuaReader::DebugIterateTableKeys(lua_State* _interpreter, int _tableIndex, int depth)
{
	//// Push nil onto the stack to start the iteration
	//lua_pushnil(_interpreter);

	//// Loop through the table, extracting keys
	//while (lua_next(_interpreter, _tableIndex) != 0) {
	//	// The key is at index -2, skip the value
	//	if (lua_type(_interpreter, -2) == LUA_TSTRING) {
	//		const char* key = lua_tostring(_interpreter, -2);
	//		LOG_DEBUG("Key: %s", key);
	//	}

	//	// Pop the value, leaving the key for the next iteration
	//	lua_pop(_interpreter, 1);
	//}


	// Push nil onto the stack to start the iteration
	lua_pushnil(_interpreter);

	// Adjust table index for relative access
	if (_tableIndex < 0) {
		_tableIndex -= 1; // Adjust for stack growth caused by lua_pushnil
	}

	// Create an indentation string based on depth
	std::string indentation(depth * 2, ' ');

	// Loop through the table
	while (lua_next(_interpreter, _tableIndex) != 0) {
		// The key is at index -2
		const char* key = (lua_type(_interpreter, -2) == LUA_TSTRING) ? lua_tostring(_interpreter, -2) : "<non-string key>";

		// Handle the value at index -1
		if (lua_type(_interpreter, -1) == LUA_TTABLE) {
			LOG_DEBUG("%sKey: %s, Value: <table>", indentation.c_str(), key);
			DebugIterateTableKeys(_interpreter, lua_gettop(_interpreter), depth + 1); // Recursively handle the table
		}
		else if (lua_type(_interpreter, -1) == LUA_TSTRING) {
			const char* value = lua_tostring(_interpreter, -1);
			LOG_DEBUG("%sKey: %s, Value: %s", indentation.c_str(), key, value);
		}
		else if (lua_type(_interpreter, -1) == LUA_TNUMBER) {
			float value = lua_tonumber(_interpreter, -1);
			LOG_DEBUG("%sKey: %s, Value: %f", indentation.c_str(), key, value);
		}
		else if (lua_type(_interpreter, -1) == LUA_TBOOLEAN) {
			bool value = lua_toboolean(_interpreter, -1);
			LOG_DEBUG("%sKey: %s, Value: %s", indentation.c_str(), key, value ? "true" : "false");
		}
		else {
			LOG_DEBUG("%sKey: %s, Value: <unknown>", indentation.c_str(), key);
		}

		// Pop the value, leaving the key for the next iteration
		lua_pop(_interpreter, 1);
	}
}

void SLuaReader::DebugLuaStack(lua_State* _interpreter)
{
	int stackSize = lua_gettop(_interpreter);
	for (int i = 1; i <= stackSize; i++) {
		if (lua_type(_interpreter, i) == LUA_TTABLE) {
			LOG_DEBUG("Stack[%d] is a table", i);

			// Iterate through the table and print the keys
			DebugIterateTableKeys(_interpreter, i);
		}
		else if (lua_type(_interpreter, i) == LUA_TSTRING) {
			LOG_DEBUG("Stack[%d] is a string: %s", i, lua_tostring(_interpreter, i));
		}
		else if (lua_type(_interpreter, i) == LUA_TNUMBER) {
			LOG_DEBUG("Stack[%d] is a number: %f", i, lua_tonumber(_interpreter, i));
		}
		else if (lua_type(_interpreter, i) == LUA_TFUNCTION) {
			LOG_DEBUG("Stack[%d] is a function");
		}
	}
}

lua_State* SLuaReader::InitLuaInterpreter()
{
	// initialize lua interpreter
	lua_State* luaInterpreter = luaL_newstate();

	// load lua base libraries (print,math,etc)
	luaL_openlibs(luaInterpreter);

	return luaInterpreter;
}

