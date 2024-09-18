#include "LifeTime.h"

void ILifeTime::InitializeLifeTime()
{
	mLifeTimeEnabled = true;
	mCurrentLifeTime = time(nullptr);
}

void ILifeTime::ReInitializeLifeTime()
{
	mCurrentLifeTime = time(nullptr);
}
