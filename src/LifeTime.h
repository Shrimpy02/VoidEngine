#pragma once

// Includes 

// Additional includes
#include <ctime>

// Forward Declarations


/**
 * @class ILifeTime
 * @brief Interface manageing objects that has a lifetime.
 */
class ILifeTime
{
public:
	// ---------- Global Variables --------------


private:
	// ---------- Local Variables --------------
	time_t mCurrentLifeTime = 0;
	
public:
	// ---------- Global functions --------------

	// Deconstructs children and components, virtual passes deconstructing to inherited first.
	virtual ~ILifeTime() = default;

	void InitializeLifeTime();

private:
	// ---------- Local functions --------------

public:
	// ---------- Getters / setters / adders --------------

	// Getters ---------

	time_t GetLifeTime() { return mCurrentLifeTime; }

	// Setters ---------

};
