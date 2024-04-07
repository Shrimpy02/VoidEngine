#pragma once
// Includes
#include <string>

// Custom macros for ease of code:

// Gets the source directory and adds it to the input path from src file, then returns full path.
#define SOURCE_DIRECTORY(relativePath) std::string("../../../" + std::string(relativePath))