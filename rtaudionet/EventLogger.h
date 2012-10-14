#pragma once

using namespace System;
using namespace System::Collections::Generic;

namespace RtAudioNet
{
	// Logging Event Args
	public ref class LoggingEventArgs : EventArgs
	{
	public:
		String^ logger;
		String^ message;
		Exception^ ex;
	}; // end LoggingEventArgs
	
	// Custom Event handler for the error event
	public delegate void LoggingEventHandler(System::Object^ sender, LoggingEventArgs^ e);
	
	// Logger instance; each logger instance has a name
	public ref class EventLogger
	{
	public:
		EventLogger(String^ name);
	
		// Events
		event LoggingEventHandler^ TraceLoggingEvent;
		event LoggingEventHandler^ DebugLoggingEvent;
		event LoggingEventHandler^ InfoLoggingEvent;
		event LoggingEventHandler^ WarnLoggingEvent;
		event LoggingEventHandler^ ErrorLoggingEvent;
		event LoggingEventHandler^ CriticalLoggingEvent;
	
		// Trace Level Logging
		void Trace(String^ message, ... array<Object^>^ params);
		void TraceEx(String^ message, Exception^ ex);
	
		// Debug Level Logging
		void Debug(String^ message, ... array<Object^>^ params);
		void DebugEx(String^ message, Exception^ ex);
	
		// Info Level Logging
		void Info(String^ message, ... array<Object^>^ params);
		void InfoEx(String^ message, Exception^ ex);
	
		// Warn Level Logging
		void Warn(String^ message, ... array<Object^>^ params);
		void WarnEx(String^ message, Exception^ ex);
	
		// Error Level Logging
		void Error(String^ message, ... array<Object^>^ params);
		void ErrorEx(String^ message, Exception^ ex);
	
		// Critical Level Logging
		void Critical(String^ message, ... array<Object^>^ params);
		void CriticalEx(String^ message, Exception^ ex);
	
	private:
		String^ name;
	};
	
	// Main Logger; static.
	public ref class EventLoggerManager
	{
	public:
		static EventLoggerManager(void);
	
		// Get a logger with the name loggerName.
		static EventLogger^ getLogger(String^ loggerName);
	
		// EventHandlers
		static void TraceLoggingEventHandler(Object^ sender, LoggingEventArgs^ args)
		{
			TraceLoggingEvent(sender, args);
		};
	
		static void DebugLoggingEventHandler(Object^ sender, LoggingEventArgs^ args)
		{
			DebugLoggingEvent(sender, args);
		};
	
		static void InfoLoggingEventHandler(Object^ sender, LoggingEventArgs^ args)
		{
			InfoLoggingEvent(sender, args);
		};
	
		static void WarnLoggingEventHandler(Object^ sender, LoggingEventArgs^ args)
		{
			WarnLoggingEvent(sender, args);
		};
	
		static void ErrorLoggingEventHandler(Object^ sender, LoggingEventArgs^ args)
		{
			ErrorLoggingEvent(sender, args);
		};
	
		static void CriticalLoggingEventHandler(Object^ sender, LoggingEventArgs^ args)
		{
			CriticalLoggingEvent(sender, args);
		};
	
		// Events
		static event LoggingEventHandler^ TraceLoggingEvent;
		static event LoggingEventHandler^ DebugLoggingEvent;
		static event LoggingEventHandler^ InfoLoggingEvent;
		static event LoggingEventHandler^ WarnLoggingEvent;
		static event LoggingEventHandler^ ErrorLoggingEvent;
		static event LoggingEventHandler^ CriticalLoggingEvent;
	
	private:
		static Dictionary<EventLogger^, String^>^ loggers;
	};
} // end namespace
