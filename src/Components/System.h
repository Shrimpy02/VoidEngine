#pragma once
// Includes
#include <Core/Tag.h>

// Additional Includes
#include <memory> 

// Forward Declarations
class Actor;

/**
 * @class System
 * @brief 
 */
class System
{
public:
	// ---------- Global Variables --------------

private:
	// ---------- Local Variables --------------


public:
	// ---------- Global functions --------------
	System(){};

	// Remove copy and move functionality
	System(const System&) = delete;
	System& operator=(const System&) = delete;
	System(System&&) = delete;
	System& operator=(System&&) = delete;
	// De-constructor
	virtual ~System() { }

private:
	// ---------- Local functions --------------



public:
	// ---------- Getters and setters --------------

};