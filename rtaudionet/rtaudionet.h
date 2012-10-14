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
// This MUST be the first file included. Thank you windows.h for fucking us hard.
#include "RtAudio\RtAudio.h"

#include "RtErrorNet.h"
#include "EventLogger.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Runtime::InteropServices;

namespace RtAudioNet {

    // RtAudio data format type.
	public enum class RtAudioFormat
	{
		RTAUDIO_NULL = 0x0,			// Null Audio Format.
		RTAUDIO_SINT8 = 0x1,		// 8-bit signed integer.
		RTAUDIO_SINT16 = 0x2,		// 16-bit signed integer.
		RTAUDIO_SINT24 = 0x4,		// Lower 3 bytes of 32-bit signed integer.
		RTAUDIO_SINT32 = 0x8,		// 32-bit signed integer.
		RTAUDIO_FLOAT32 = 0x10,		// Normalized between plus/minus 1.0.
		RTAUDIO_FLOAT64 = 0x20		// Normalized between plus/minus 1.0.
	};

    // RtAudio stream option flags.
	public enum class RtAudioStreamFlags
	{
		RTAUDIO_NONINTERLEAVED = 0x1,		// Use non-interleaved buffers (default = interleaved).
		RTAUDIO_MINIMIZE_LATENCY = 0x2,		// Attempt to set stream parameters for lowest possible latency.
		RTAUDIO_HOG_DEVICE = 0x4,			// Attempt grab device and prevent use by others.
		RTAUDIO_SCHEDULE_REALTIME = 0x8,	// Try to select realtime scheduling for callback thread.
		RTAUDIO_ALSA_USE_DEFAULT = 0x10		// Use the "default" PCM device (ALSA only).
	};
	
	// RtAudio stream status (over- or underflow) flags.
	typedef unsigned int RtAudioStreamStatus;
	static const RtAudioStreamStatus RTAUDIO_INPUT_OVERFLOW = 0x1;    // Input data was discarded because of an overflow condition at the driver.
	static const RtAudioStreamStatus RTAUDIO_OUTPUT_UNDERFLOW = 0x2;  // The output buffer ran low, likely causing a gap in the output sound.
	
	// RtAudio callback function prototype.
	/*typedef int (*RtAudioCallback)( void *outputBuffer, void *inputBuffer,
									unsigned int nFrames,
									double streamTime,
									RtAudioStreamStatus status,
									void *userData );*/

	//typedef Func<IntPtr, IntPtr, unsigned int, double, RtAudioStreamStatus, IntPtr, int> RtAudioCallback;

	public delegate int RtAudioNetCallback(IntPtr, IntPtr, unsigned int, double, RtAudioStreamStatus, Object^);

	public ref class RtAudio : RtError
	{
	public:

		// Audio API specifier arguments.
		enum class Api
		{
			UNSPECIFIED,    // Search for a working compiled API.
			LINUX_ALSA,     // The Advanced Linux Sound Architecture API.
			LINUX_OSS,      // The Linux Open Sound System API.
			UNIX_JACK,      // The Jack Low-Latency Audio Server API.
			MACOSX_CORE,    // Macintosh OS-X Core Audio API.
			WINDOWS_ASIO,   // The Steinberg Audio Stream I/O API.
			WINDOWS_DS,     // The Microsoft Direct Sound API.
			RTAUDIO_DUMMY   // A compilable but non-functional API.
		}; // end Api

		// The public device information structure for returning queried values.
		ref struct DeviceInfo 
		{
			bool probed;					// true if the device capabilities were successfully probed.
			String^ name;			// Character string device identifier.
			unsigned int outputChannels;	// Maximum output channels supported by device.
			unsigned int inputChannels;		// Maximum input channels supported by device.
			unsigned int duplexChannels;	// Maximum simultaneous input/output channels supported by device.
			bool isDefaultOutput;			// true if this is the default output device.
			bool isDefaultInput;			// true if this is the default input device.
			List<unsigned int>^ sampleRates;	// Supported sample rates (queried from list of standard rates).
			int nativeFormats;	// Bit mask of supported data formats.
			
			// Default constructor.
			DeviceInfo() : probed(false), outputChannels(0), inputChannels(0), duplexChannels(0),
				isDefaultOutput(false), isDefaultInput(false), nativeFormats(0) {}

			DeviceInfo(::RtAudio::DeviceInfo _info)
			{
				// Translate from the unmanaged instance to the managed instance
				probed = _info.probed;
				name = gcnew String(_info.name.c_str());;
				outputChannels = _info.outputChannels;
				inputChannels = _info.inputChannels;
				duplexChannels = _info.duplexChannels;
				isDefaultOutput = _info.isDefaultOutput;
				isDefaultInput = _info.isDefaultInput;
				nativeFormats = _info.nativeFormats;
				
				sampleRates = gcnew List<unsigned int>();
		
				// Get the sample rates
				for(std::vector<unsigned int>::iterator it = _info.sampleRates.begin(); it != _info.sampleRates.end(); ++it) 
				{
					sampleRates->Add(*it);
				} // end for
				
			} // end DefaultAudio
		}; // end DeviceInfo
		
		// The structure for specifying input or ouput stream parameters.
		ref struct StreamParameters 
		{
			unsigned int deviceId;     // Device index (0 to getDeviceCount() - 1).
			unsigned int nChannels;    // Number of channels.
			unsigned int firstChannel; // First channel index on device (default = 0).
			
			// Default constructor.
			StreamParameters() : deviceId(0), nChannels(0), firstChannel(0) {}
		}; // end StreamParameters

		// The structure for specifying stream options.
		ref struct StreamOptions 
		{
			int flags;		// A bit-mask of stream flags (RTAUDIO_NONINTERLEAVED, RTAUDIO_MINIMIZE_LATENCY, RTAUDIO_HOG_DEVICE, RTAUDIO_ALSA_USE_DEFAULT).
			unsigned int numberOfBuffers;	// Number of stream buffers.
			String^ streamName;				// A stream name (currently used only in Jack).
			int priority;					// Scheduling priority of callback thread (only used with flag RTAUDIO_SCHEDULE_REALTIME).
			
			// Default constructor.
			StreamOptions() : flags(0), numberOfBuffers(0), priority(0) {}
		};
		
		// A static function to determine the available compiled audio APIs.
		static List<RtAudio::Api>^ getCompiledApi();
		
		// The class constructor.
		RtAudio();

		// The class constructor.
		RtAudio(RtAudio::Api api);

		// The destructor.
		~RtAudio();
		
		// Returns the audio API specifier for the current instance of RtAudio.
		RtAudio::Api getCurrentApi();
		
		// A public function that queries for the number of audio devices available.
		unsigned int getDeviceCount();
		
		// Return an RtAudio::DeviceInfo structure for a specified device number.
		RtAudio::DeviceInfo^ getDeviceInfo(unsigned int device);
		
		// A function that returns the index of the default output device.
		unsigned int getDefaultOutputDevice();
		
		// A function that returns the index of the default input device.
		unsigned int getDefaultInputDevice();
		
		// A public function for opening a stream with the specified parameters.
		void openStream( RtAudio::StreamParameters^ outputParameters, RtAudio::StreamParameters^ inputParameters, RtAudioFormat format, unsigned int sampleRate, 
			[InAttribute] [OutAttribute] unsigned int bufferFrames, RtAudioNetCallback^ callback) {openStream(outputParameters, inputParameters, format, sampleRate, bufferFrames, callback, nullptr, gcnew RtAudio::StreamOptions());};
		
		// A public function for opening a stream with the specified parameters.
		void openStream( RtAudio::StreamParameters^ outputParameters, RtAudio::StreamParameters^ inputParameters, RtAudioFormat format, unsigned int sampleRate, 
			[InAttribute] [OutAttribute] unsigned int bufferFrames, RtAudioNetCallback^ callback, Object^ userData) {openStream(outputParameters, inputParameters, format, sampleRate, bufferFrames, callback, userData, gcnew RtAudio::StreamOptions());};
		
		// A public function for opening a stream with the specified parameters.
		void openStream( RtAudio::StreamParameters^ outputParameters, RtAudio::StreamParameters^ inputParameters, RtAudioFormat format, unsigned int sampleRate, 
			[InAttribute] [OutAttribute] unsigned int bufferFrames, RtAudioNetCallback^ callback, RtAudio::StreamOptions^ options) {openStream(outputParameters, inputParameters, format, sampleRate, bufferFrames, callback, nullptr, options);};
		
		// A public function for opening a stream with the specified parameters.
		void openStream( RtAudio::StreamParameters^ outputParameters, RtAudio::StreamParameters^ inputParameters, RtAudioFormat format, unsigned int sampleRate, 
			[InAttribute] [OutAttribute] unsigned int bufferFrames, RtAudioNetCallback^ callback, Object^ userData, RtAudio::StreamOptions^ options);
		
		// A function that closes a stream and frees any associated stream memory.
		void closeStream();
		
		// A function that starts a stream.
		void startStream();
		
		// Stop a stream, allowing any samples remaining in the output queue to be played.
		void stopStream();
		
		// Stop a stream, discarding any samples remaining in the input/output queue.
		void abortStream();
		
		// Returns true if a stream is open and false if not.
		bool isStreamOpen();
		
		// Returns true if the stream is running and false if it is stopped or not open.
		bool isStreamRunning();
		
		// Returns the number of elapsed seconds since the stream was started.
		double getStreamTime();
		
		// Returns the internal stream latency in sample frames.
		long getStreamLatency();
		
		// Returns actual sample rate in use by the stream.
		unsigned int getStreamSampleRate();
		
		// Specify whether warning messages should be printed to stderr.
		void showWarnings() {showWarnings(true);};

		// Specify whether warning messages should be printed to stderr.
		void showWarnings(bool value);
		
		// Callback
		RtAudioNetCallback^ _callback;

		// Callback
		Object^ userData;

		// return a suggested buffer length when the stream cannot be opened
		int GetSuggestedBufferLength();

	protected:
		
		// Actual constructor. This is a work around for issues using default parameters in managed C++.
		void initialize(RtAudio::Api api);

		// Converstion of StreamParameters^ to StreamParameters*
		::RtAudio::StreamParameters* convertManagedToUnmanaged(RtAudio::StreamParameters^ _params);

		// Converstion of StreamOptions^ to StreamOptions*
		::RtAudio::StreamOptions* convertManagedToUnmanaged(RtAudio::StreamOptions^ _options);

		// Actual RtAudio instance
		::RtAudio* _rtaudio;

		// Our logger
		EventLogger^ logger;
	};

	// Internal Callback
	int audioCallback(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void *userData);
} // end namespace
