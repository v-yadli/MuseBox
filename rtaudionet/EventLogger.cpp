#include "EventLogger.h"


namespace RtAudioNet
{
	EventLogger::EventLogger(String^ name)
	{
		this->name = name;
	} // end EventLogger
	
	// Trace Level Logging
	void EventLogger::Trace(String^ message, ... array<Object^>^ params)
	{
		LoggingEventArgs^ args = gcnew LoggingEventArgs();
		args->logger = this->name;
		args->message = String::Format(message, params);
	
		TraceLoggingEvent(this, args);
	} // end Trace
	
	void EventLogger::TraceEx(String^ message, Exception^ ex)
	{
		LoggingEventArgs^ args = gcnew LoggingEventArgs();
		args->logger = this->name;
		args->message = message;
		args->ex = ex;
	
		TraceLoggingEvent(this, args);
	} // end TraceEx
	
	// Debug Level Logging
	void EventLogger::Debug(String^ message, ... array<Object^>^ params)
	{
		LoggingEventArgs^ args = gcnew LoggingEventArgs();
		args->logger = this->name;
		args->message = String::Format(message, params);
	
		DebugLoggingEvent(this, args);
	} // end Debug
	
	void EventLogger::DebugEx(String^ message, Exception^ ex)
	{
		LoggingEventArgs^ args = gcnew LoggingEventArgs();
		args->logger = this->name;
		args->message = message;
		args->ex = ex;
	
		DebugLoggingEvent(this, args);
	} // end DebugEx
	
	// Info Level Logging
	void EventLogger::Info(String^ message, ... array<Object^>^ params)
	{
		LoggingEventArgs^ args = gcnew LoggingEventArgs();
		args->logger = this->name;
		args->message = String::Format(message, params);
	
		InfoLoggingEvent(this, args);
	} // end Info
	
	void EventLogger::InfoEx(String^ message, Exception^ ex)
	{
		LoggingEventArgs^ args = gcnew LoggingEventArgs();
		args->logger = this->name;
		args->message = message;
		args->ex = ex;
	
		InfoLoggingEvent(this, args);
	} // end InfoEx
	
	// Warn Level Logging
	void EventLogger::Warn(String^ message, ... array<Object^>^ params)
	{
		LoggingEventArgs^ args = gcnew LoggingEventArgs();
		args->logger = this->name;
		args->message = String::Format(message, params);
	
		WarnLoggingEvent(this, args);
	} // end Warn
	
	void EventLogger::WarnEx(String^ message, Exception^ ex)
	{
		LoggingEventArgs^ args = gcnew LoggingEventArgs();
		args->logger = this->name;
		args->message = message;
		args->ex = ex;
	
		WarnLoggingEvent(this, args);
	} // end WarnEx
	
	// Error Level Logging
	void EventLogger::Error(String^ message, ... array<Object^>^ params)
	{
		LoggingEventArgs^ args = gcnew LoggingEventArgs();
		args->logger = this->name;
		args->message = String::Format(message, params);
	
		ErrorLoggingEvent(this, args);
	} // end Error
	
	void EventLogger::ErrorEx(String^ message, Exception^ ex)
	{
		LoggingEventArgs^ args = gcnew LoggingEventArgs();
		args->logger = this->name;
		args->message = message;
		args->ex = ex;
	
		ErrorLoggingEvent(this, args);
	} // end ErrorEx
	
	// Critical Level Logging
	void EventLogger::Critical(String^ message, ... array<Object^>^ params)
	{
		LoggingEventArgs^ args = gcnew LoggingEventArgs();
		args->logger = this->name;
		args->message = String::Format(message, params);
	
		CriticalLoggingEvent(this, args);
	} // end Critical
	
	void EventLogger::CriticalEx(String^ message, Exception^ ex)
	{
		LoggingEventArgs^ args = gcnew LoggingEventArgs();
		args->logger = this->name;
		args->message = message;
		args->ex = ex;
	
		CriticalLoggingEvent(this, args);
	} // end CriticalEx
	
	
	// EventLogger
	static EventLoggerManager::EventLoggerManager(void)
	{
		loggers = gcnew Dictionary<EventLogger^, String^>();
	} // end EventLogger
	
	
	// Return a new logger with name loggerName.
	EventLogger^ EventLoggerManager::getLogger(String^ loggerName)
	{
		EventLogger^ logger = gcnew EventLogger(loggerName);
	
		// Wire up events
		logger->TraceLoggingEvent += gcnew LoggingEventHandler(&EventLoggerManager::TraceLoggingEventHandler);
		logger->DebugLoggingEvent += gcnew LoggingEventHandler(&EventLoggerManager::DebugLoggingEventHandler);
		logger->InfoLoggingEvent += gcnew LoggingEventHandler(&EventLoggerManager::InfoLoggingEventHandler);
		logger->WarnLoggingEvent += gcnew LoggingEventHandler(&EventLoggerManager::WarnLoggingEventHandler);
		logger->ErrorLoggingEvent += gcnew LoggingEventHandler(&EventLoggerManager::ErrorLoggingEventHandler);
		logger->CriticalLoggingEvent += gcnew LoggingEventHandler(&EventLoggerManager::CriticalLoggingEventHandler);
	
		loggers[logger] = loggerName;
	
		return logger;
	} // end getLogger
} // end namespace