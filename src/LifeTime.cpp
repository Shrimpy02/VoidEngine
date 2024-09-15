#include "LifeTime.h"

void ILifeTime::InitializeLifeTime()
{
	mLifeTimeEnabled = true;
	mCurrentLifeTime = time(nullptr);
}

void ILifeTime::ReInitalizeLifeTime()
{
	mCurrentLifeTime = time(nullptr);
}
