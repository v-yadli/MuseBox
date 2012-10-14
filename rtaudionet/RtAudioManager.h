#pragma region License
/*
 * Copyright (c) 2011 Christopher S. Case

 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#pragma endregion

#pragma once

#include "RtAudioNet.h"
#include "RtMixer.h"
#include "EventLogger.h"

using namespace System::Timers;

namespace RtStream
{
	// Generic Exception for handling
	public ref class RtAudioManagerException: ApplicationException
    {
	public:
		RtAudioManagerException() : ApplicationException() { };

        RtAudioManagerException(String^ message) : ApplicationException(message) { };

        RtAudioManagerException(String^ message, Exception^ innerException) : ApplicationException(message, innerException) { };

    }; // end RtAudioManagerException


	// Exception for API related errors
	public ref class RtAudioManagerApiException : RtAudioException
    {
	public:
		RtAudioManagerApiException() : RtAudioException() { };

		RtAudioManagerApiException(String^ message) : RtAudioException(message) { };

        RtAudioManagerApiException(String^ message, Exception^ innerException) : RtAudioException(message, innerException) { };

    }; // end RtAudioManagerApiException

	// RtAudioNetErrorEvent EventArgs
	public ref class RtAudioErrorEventArgs : EventArgs
	{
	public:
		String^ errorString;
	}; // end RtAudioErrorEventArgs

	// Custom Event handler for the error event
	public delegate void RtAudioErrorEventHandler(System::Object^ sender, RtAudioErrorEventArgs^ e);

	public ref class RtAudioManager
	{
		static RtAudioManager^ _manager = nullptr;

	public:
		// Default Constructor
		RtAudioManager();

		// Default Constructor
		RtAudioManager(::RtAudioNet::RtAudio::Api api);

		// Default Destructor
		~RtAudioManager();

		// Start a timer to watch for changes in the number of devices
		void enableDeviceWatcher(int interval);

		// Fires when a device is added or removed from the system
		event EventHandler^ DeviceEnumerationChanged;

		// Fires when there is an error
		event RtAudioErrorEventHandler^ ErrorOccured;

		// Finds the input device id by name
		int FindInputDeviceByName(String^ name);

		// Finds the output device id by name
		int FindOutputDeviceByName(String^ name);

		// Gets the device information for the given device id
		::RtAudioNet::RtAudio::DeviceInfo^ GetDeviceInfo(int devID);

		// Creates and returns a mixer based on the inputs and outputs
		RtStreamMixer^ CreateMixer(List<int>^ inputs, int output);

		// Creates and returns a mixer based on the inputs and outputs
		RtStreamMixer^ CreateMixer(List<int>^ inputs, int output, bool useDuplex);

		// Creates and returns a mixer based on the inputs and outputs
		RtStreamMixer^ CreateMixer(List<String^>^ inputs, String^ output);

		// Creates and returns a mixer based on the inputs and outputs
		RtStreamMixer^ CreateMixer(List<String^>^ inputs, String^ output, bool useDuplex);

		// Creates and returns a mixer based on the inputs and outputs
		RtStreamMixer^ CreateMixer(List<Dictionary<String^, String^>^>^ inputs, int output);

		// Creates and returns a mixer based on the inputs and outputs
		RtStreamMixer^ CreateMixer(List<Dictionary<String^, String^>^>^ inputs, int output, bool useDuplex);

		// Creates and returns a mixer based on the inputs and outputs
		RtStreamMixer^ CreateMixer(List<int>^ inputs, int output, int sampleRate);

		// Creates and returns a mixer based on the inputs and outputs
		RtStreamMixer^ CreateMixer(List<int>^ inputs, int output, int sampleRate, bool useDuplex);

		// Creates and returns a mixer based on the inputs and outputs
		RtStreamMixer^ CreateMixer(List<String^>^ inputs, String^ output, int sampleRate);

		// Creates and returns a mixer based on the inputs and outputs
		RtStreamMixer^ CreateMixer(List<String^>^ inputs, String^ output, int sampleRate, bool useDuplex);

		// Returns the current Api
		::RtAudioNet::RtAudio::Api GetCurrentApi() {return _api;};

		// Properties \\

		// Device Count
		property int DeviceCount;

		// Dictionary of device name to device id
		property Dictionary<String^, int>^ InputDevices;

		// Dictionary of device name to device id
		property Dictionary<String^, int>^ OutputDevices;

		// Get Instance Methods \\

		// Simply returns the static instance, or creates a new one if it's null
		static RtAudioManager^ GetInstance();

		// Attempts to create a new instance with the given api, or throws an exception if the static instance is not null
		static RtAudioManager^ GetInstance(::RtAudioNet::RtAudio::Api api);

	protected:
		// Creates and returns a mixer based on the inputs and outputs
		RtStreamMixer^  _createMixer(List<int>^ inputs, int output, RtStreamMixer^ mixer);

		// Creates and returns a mixer based on the inputs and outputs
		RtStreamMixer^ _createMixer(List<String^>^ inputs, String^ output, RtStreamMixer^ mixer);

		// Creates and returns a mixer based on the inputs and outputs
		RtStreamMixer^ _createMixer(List<Dictionary<String^, String^>^>^ inputs, int output, RtStreamMixer^ mixer);

		// Enumerate Devices
		bool EnumerateDevices();

		// Enumeration event handler
		void OnEnumerateTimer(Object^ sender, ElapsedEventArgs^ args);

		// Common initialization
		void initialize();

		// Logger
		EventLogger^ logger;

		// Enumerate Devices timer
		System::Timers::Timer^ enumerateTimer;

		// Current RtAudio::Api
		::RtAudioNet::RtAudio::Api _api;

		// Local RtAudioNet instance.
		::RtAudioNet::RtAudio^ rtaudio;
	}; // end RtAudioManager
} // end namespace
