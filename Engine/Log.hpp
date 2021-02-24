#pragma once

#include <string>
#include <vector>

enum class LogSeverity {
	TRACE,
	INFO,
	WARNING,
	ERRO,
	FATAL
};

enum class LogSource {
	NEO_DOA,
	CLIENT
};

struct LogMessage {
	std::string _message;
	LogSeverity _severity;

	LogMessage(LogSeverity severity, const std::string& message);
};

struct Log {
	inline static std::vector<LogMessage> _messages;

	//Please use the #define's below, don't call these methods directly.
	static void Print(LogSource src, LogSeverity sev, const char* fmt, ...);
	static void SaveMessage(LogSource src, LogSeverity sev, const char* fmt, ...);

	static void Clear();
};

#ifdef _DEBUG
#define DOA_LOG_TRACE(fmt, ...)			Log::SaveMessage(LogSource::NEO_DOA, LogSeverity::TRACE, fmt, __VA_ARGS__);		Log::Print(LogSource::NEO_DOA, LogSeverity::TRACE, fmt, __VA_ARGS__)
#define DOA_LOG_INFO(fmt, ...)			Log::SaveMessage(LogSource::NEO_DOA, LogSeverity::INFO, fmt, __VA_ARGS__);		Log::Print(LogSource::NEO_DOA, LogSeverity::INFO, fmt, __VA_ARGS__)
#define DOA_LOG_WARNING(fmt, ...)		Log::SaveMessage(LogSource::NEO_DOA, LogSeverity::WARNING, fmt, __VA_ARGS__);	Log::Print(LogSource::NEO_DOA, LogSeverity::WARNING, fmt, __VA_ARGS__)
#define DOA_LOG_ERROR(fmt, ...)			Log::SaveMessage(LogSource::NEO_DOA, LogSeverity::ERRO, fmt, __VA_ARGS__);		Log::Print(LogSource::NEO_DOA, LogSeverity::ERRO, fmt, __VA_ARGS__)
#define DOA_LOG_FATAL(fmt, ...)			Log::SaveMessage(LogSource::NEO_DOA, LogSeverity::FATAL, fmt, __VA_ARGS__);		Log::Print(LogSource::NEO_DOA, LogSeverity::FATAL, fmt, __VA_ARGS__)

#define CLI_LOG_TRACE(fmt, ...)			Log::SaveMessage(LogSource::CLIENT, LogSeverity::TRACE, fmt, __VA_ARGS__);		Log::Print(LogSource::CLIENT, LogSeverity::TRACE, fmt, __VA_ARGS__)
#define CLI_LOG_INFO(fmt, ...)			Log::SaveMessage(LogSource::CLIENT, LogSeverity::INFO, fmt, __VA_ARGS__);		Log::Print(LogSource::CLIENT, LogSeverity::INFO, fmt, __VA_ARGS__)
#define CLI_LOG_WARNING(fmt, ...)		Log::SaveMessage(LogSource::CLIENT, LogSeverity::WARNING, fmt, __VA_ARGS__);	Log::Print(LogSource::CLIENT, LogSeverity::WARNING, fmt, __VA_ARGS__)
#define CLI_LOG_ERROR(fmt, ...)			Log::SaveMessage(LogSource::CLIENT, LogSeverity::ERRO, fmt, __VA_ARGS__);		Log::Print(LogSource::CLIENT, LogSeverity::ERRO, fmt, __VA_ARGS__)
#define CLI_LOG_FATAL(fmt, ...)			Log::SaveMessage(LogSource::CLIENT, LogSeverity::FATAL, fmt, __VA_ARGS__);		Log::Print(LogSource::CLIENT, LogSeverity::FATAL, fmt, __VA_ARGS__)
#else
#define DOA_LOG_TRACE(fmt, ...)			Log::SaveMessage(LogSource::NEO_DOA, LogSeverity::TRACE, fmt, __VA_ARGS__)
#define DOA_LOG_INFO(fmt, ...)			Log::SaveMessage(LogSource::NEO_DOA, LogSeverity::INFO, fmt, __VA_ARGS__)
#define DOA_LOG_WARNING(fmt, ...)		Log::SaveMessage(LogSource::NEO_DOA, LogSeverity::WARNING, fmt, __VA_ARGS__)
#define DOA_LOG_ERROR(fmt, ...)			Log::SaveMessage(LogSource::NEO_DOA, LogSeverity::ERRO, fmt, __VA_ARGS__)
#define DOA_LOG_FATAL(fmt, ...)			Log::SaveMessage(LogSource::NEO_DOA, LogSeverity::FATAL, fmt, __VA_ARGS__)

#define CLI_LOG_TRACE(fmt, ...)			Log::SaveMessage(LogSource::CLIENT, LogSeverity::TRACE, fmt, __VA_ARGS__)
#define CLI_LOG_INFO(fmt, ...)			Log::SaveMessage(LogSource::CLIENT, LogSeverity::INFO, fmt, __VA_ARGS__)
#define CLI_LOG_WARNING(fmt, ...)		Log::SaveMessage(LogSource::CLIENT, LogSeverity::WARNING, fmt, __VA_ARGS__)
#define CLI_LOG_ERROR(fmt, ...)			Log::SaveMessage(LogSource::CLIENT, LogSeverity::ERRO, fmt, __VA_ARGS__)
#define CLI_LOG_FATAL(fmt, ...)			Log::SaveMessage(LogSource::CLIENT, LogSeverity::FATAL, fmt, __VA_ARGS__)
#endif