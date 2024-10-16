#pragma once

// Includes
#include <iostream>
#include <sstream>
#include <cstdarg>

// defines debug mode witch enables debug logs to be printed.
// Comment out for final build. 
#define DEBUG_MODE

namespace Logger
{
	// enum for log level
	enum class LogLevel
	{
		_DEFAULT_,
		_DEBUG_,
		_INFO_,
		_WARNING_,
		_ERROR_
	};

	constexpr const char* levelToStr(LogLevel level)
	{
		// Gets const char of enum by unput case
		switch (level)
		{
		case LogLevel::_DEBUG_:
			return "[DEBUG] ";
		case LogLevel::_INFO_:
			return "[INFO] ";
		case LogLevel::_WARNING_:
			return "[WARNING] ";
		case LogLevel::_ERROR_:
			return "[ERROR] ";
		case LogLevel::_DEFAULT_:
			return "";
		}
		return "UNKNOWN";
	}

	template <LogLevel level>
	void log(const char* format, ...)
	{
		std::ostringstream oss;

		// Set color based on log level
		const char* color = "";
		switch (level) {
		case LogLevel::_DEBUG_:
			color = "\033[33m"; // Yellow
			break;
		case LogLevel::_INFO_:
			color = "\033[32m"; // Green
			break;
		case LogLevel::_WARNING_:
			color = "\033[33m"; // Yellow
			break;
		case LogLevel::_ERROR_:
			color = "\033[31m"; // Red
			break;
		}

		// Gets level and prints it first
		oss << levelToStr(level);

		// buffer shinnanigens
		va_list args;
		va_start(args, format);
		char buffer[256];
		vsnprintf(buffer, sizeof(buffer), format, args);
		va_end(args);

		// Sets text to white when finished and prints the message.
		oss << color << buffer << "\033[37m" << std::endl;
		std::cout << oss.str();
	}

}

// macro definitions of each log case.
#ifdef DEBUG_MODE
#define LOG_DEBUG(...) Logger::log<Logger::LogLevel::_DEBUG_>(__VA_ARGS__)
#else
#define LOG_DEBUG(...) do {} while (0)
#endif

#define LOG(...) Logger::log<Logger::LogLevel::_DEFAULT_>(__VA_ARGS__)
#define LOG_INFO(...) Logger::log<Logger::LogLevel::_INFO_>(__VA_ARGS__)
#define LOG_WARNING(...) Logger::log<Logger::LogLevel::_WARNING_>(__VA_ARGS__)
#define LOG_ERROR(...) Logger::log<Logger::LogLevel::_ERROR_>(__VA_ARGS__)