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
#include "CircularBuffer.h"
#include "EventLogger.h"

using namespace System::Runtime::InteropServices;
using namespace System::IO;
using namespace RtAudioNet;

namespace RtStream
{
	public ref struct RtStreamFormat
	{
	public:
		::RtAudioNet::RtAudioFormat type;
		unsigned int channels;
		unsigned int sampleRate;
		unsigned int bitsPerSample;
		::RtAudioNet::RtAudio::StreamOptions^ options;
	}; // end RtStreamFormat

	public ref class RtAudioStream : Stream
	{
	public:
		// Default Constructor
		RtAudioStream();

		// Format constructor
		RtAudioStream(RtStreamFormat^ format) { Format = format; RtAudioStream(); };

		// Format constructor (I'm sorry for this terribly long inline constructor, buuut... 
		RtAudioStream(::RtAudioNet::RtAudioFormat type, unsigned int channels, unsigned int sampleRate, unsigned int bitsPerSample) { RtStreamFormat^ format = gcnew RtStreamFormat(); format->type = type; format->channels = channels; format->sampleRate = sampleRate; format->bitsPerSample = bitsPerSample; Format = format; RtAudioStream(); };

		// CanRead property required by the stream base class.
		virtual property bool CanRead { bool get () override { return _canRead; }; };

		// CanSeek property required by the stream base class.
		virtual property bool CanSeek { bool get () override { return _canSeek; }; };

		// CanWrite property required by the stream base class.
		virtual property bool CanWrite { bool get () override { return _canWrite; }; };

		// Length property required by the stream base class.
		virtual property long long Length { long long get () override { throw gcnew NotSupportedException(); }; };

		// Position property required by the stream base class.
		virtual property long long Position { long long get () override { return 0; }; void set (long long value) override { throw gcnew NotSupportedException(); }; }

		// Seek property required by the stream base class.
		virtual long long Seek(long long offset, SeekOrigin origin) override { throw gcnew NotSupportedException(); };

		// SetLength property required by the stream base class.
		virtual void SetLength(long long value) override { throw gcnew NotSupportedException(); };

		// Flush method required by the stream base class.
		virtual void Flush() override {internalBuffer->Clear();};

		// Read method required by the stream base class.
		virtual int Read([InAttribute] [OutAttribute] array<unsigned char>^ buffer, int offset, int count) override;

		// Read method required by the stream base class.
		virtual int Read([InAttribute] [OutAttribute] array<float>^ buffer, int offset, int count);

		// Read method required by the stream base class.
		virtual int Read(float buffer[], int offset, int count);

		// Read class that's more convienent.
		int Read([InAttribute] [OutAttribute] array<float>^ buffer);
		
		// Write method required by the stream base class.
		virtual void Write(array<unsigned char>^ buffer, int offset, int count) override;

		// Write method required by the stream base class.
		virtual void Write(array<float>^ buffer, int offset, int count);

		// Write method required by the stream base class.
		virtual void Write(float buffer[], int offset, int count);

		// Write method required by the stream base class.
		void Write(array<float>^ buffer);

		// Opens the stream
		void Open() { };

		// Starts the stream
		void Start() { };

		// Stops the stream
		void Stop() { };

		// Closes the stream
		void Finish() { };

		// Aborts the stream
		void Abort() { };

		// Is the stream a live stream, or a buffered stream?
		bool IsLive() { return false; };

		// Is the stream currently open?
		bool IsStreamOpen() { return rtaudio->isStreamOpen(); };

		// Is the stream currently running?
		bool IsStreamRunning() { return rtaudio->isStreamRunning(); };

		// Stream's format
		property RtStreamFormat^ Format;

		// The frames buffered in the stream
		property unsigned int Frames;

		// Optional name for the stream
		property String^ Name;

		// The Stream's DeviceID.
		property int DeviceID;

		// Called whenever the RtAudio callback is fired.
		event EventHandler^ CallbackFired;

		// Called whenever the RtAudio callback is fired.
		event EventHandler^ BufferUnderrun;

		// Called whenever the RtAudio callback is fired.
		event EventHandler^ BufferOverrun;
		
		// Callback
		::RtAudioNet::RtAudioNetCallback^ _callback;

	protected:
		// Our internal RtAudio instance
		::RtAudioNet::RtAudio^ rtaudio;

		// Our internal buffer
		CircularBuffer<float>^ internalBuffer;

		// Our logger
		EventLogger^ logger;

		// Property variables
		bool _canRead;
		bool _canWrite;
		bool _canSeek;
	}; // end RtWaveStream

	// RtInputStream takes an input device and outputs that to a buffer.
	public ref class RtInputStream : RtAudioStream
	{
	public:
		// Default Constructor
		RtInputStream() { Frames = 512; initialize(); };

		// Default Constructor
		RtInputStream(unsigned int frames) {Frames = frames; initialize();};

		// Format constructor
		RtInputStream(RtStreamFormat^ format){ Format = format; Frames = 512; initialize();};

		// Format constructor
		RtInputStream(RtStreamFormat^ format, unsigned int frames){ Format = format; Frames = frames; initialize();};

		// Format constructor (I'm sorry for this terribly long inline constructor, buuut...) 
		RtInputStream(::RtAudioNet::RtAudioFormat type, unsigned int channels, unsigned int sampleRate, unsigned int bitsPerSample) 
		{ RtStreamFormat^ format = gcnew RtStreamFormat(); format->type = type; format->channels = channels; format->sampleRate = sampleRate; format->bitsPerSample = bitsPerSample; Format = format; Frames = 512; initialize(); };

		// Format constructor (I'm sorry for this terribly long inline constructor, buuut...) 
		RtInputStream(::RtAudioNet::RtAudioFormat type, unsigned int channels, unsigned int sampleRate, unsigned int bitsPerSample, unsigned int frames)
		{ RtStreamFormat^ format = gcnew RtStreamFormat(); format->type = type; format->channels = channels; format->sampleRate = sampleRate; format->bitsPerSample = bitsPerSample; Format = format; Frames = frames; initialize(); };

		// Default Destructor
		~RtInputStream();

		// Selects the correct input device
		void selectInputDevice(int devID);

		// Selects the correct input device
		void selectInputDevice(String^ devString);

		// Read method required by the stream base class.
		virtual int Read([InAttribute] [OutAttribute] array<float>^ buffer, int offset, int count) override;
		
		// Write method required by the stream base class.
		virtual void Write(array<float>^ buffer, int offset, int count) override;

		// Opens the stream
		void Open();

		// Starts the stream
		void Start();

		// Stops the stream
		void Stop();

		// Closes the stream
		void Finish();

		// Aborts the stream
		void Abort();

		// Is the stream a live stream, or a buffered stream?
		bool IsLive();

	protected:
		// Finish initializing the stream
		void initialize();

		// Input stream parameters
		::RtAudioNet::RtAudio::StreamParameters^ inputStreamParams;

		// The stream's callback
        int callback(IntPtr outputBufferPtr, IntPtr inputBufferPtr, unsigned int frames, double streamTime, ::RtAudioNet::RtAudioStreamStatus status, Object^ userData);
	}; // end RtInputStream

	// RtOutputStream takes a buffer and outputs that to the output device.
	public ref class RtOutputStream : RtAudioStream
	{
	public:
		// Default Constructor
		RtOutputStream() { Frames = 512; initialize(); };

		// Default Constructor
		RtOutputStream(unsigned int frames) {this->Frames = frames; initialize();};

		// Format constructor
		RtOutputStream(RtStreamFormat^ format){ Format = format; Frames = 512; initialize();};

		// Format constructor
		RtOutputStream(RtStreamFormat^ format, unsigned int frames){ Format = format; this->Frames = frames; initialize();};

		// Format constructor (I'm sorry for this terribly long inline constructor, buuut...) 
		RtOutputStream(::RtAudioNet::RtAudioFormat type, unsigned int channels, unsigned int sampleRate, unsigned int bitsPerSample) 
		{ RtStreamFormat^ format = gcnew RtStreamFormat(); format->type = type; format->channels = channels; format->sampleRate = sampleRate; format->bitsPerSample = bitsPerSample; Format = format; Frames = 512; initialize(); };

		// Format constructor (I'm sorry for this terribly long inline constructor, buuut...) 
		RtOutputStream(::RtAudioNet::RtAudioFormat type, unsigned int channels, unsigned int sampleRate, unsigned int bitsPerSample, unsigned int frames)
		{ RtStreamFormat^ format = gcnew RtStreamFormat(); format->type = type; format->channels = channels; format->sampleRate = sampleRate; format->bitsPerSample = bitsPerSample; Format = format; this->Frames = frames; initialize(); };

		// Default Destructor
		~RtOutputStream();

		// Selects the correct input device
		void selectOutputDevice(int devID);

		// Selects the correct input device
		void selectOutputDevice(String^ devString);

		// Read method required by the stream base class.
		virtual int Read([InAttribute] [OutAttribute] array<float>^ buffer, int offset, int count) override;
		
		// Write method required by the stream base class.
		virtual void Write(array<float>^ buffer, int offset, int count) override;

		// Opens the stream
		void Open();

		// Starts the stream
		void Start();

		// Stops the stream
		void Stop();

		// Closes the stream
		void Finish();

		// Aborts the stream
		void Abort();

		// Is the stream a live stream, or a buffered stream?
		bool IsLive();

	protected:
		// Finish initializing the stream
		void initialize();

		// Input stream parameters
		::RtAudioNet::RtAudio::StreamParameters^ outputStreamParams;

		// The stream's callback
        int callback(IntPtr outputBufferPtr, IntPtr inputBufferPtr, unsigned int frames, double streamTime, ::RtAudioNet::RtAudioStreamStatus status, Object^ userData);
	}; // end RtOutputStream

	// RtDuplexStream takes an input device and outputs that to the output device.
	public ref class RtDuplexStream : RtAudioStream
	{
	public:
		// Default Constructor
		RtDuplexStream() { Frames = 512; initialize(); };

		// Default Constructor
		RtDuplexStream(unsigned int frames) {this->Frames = frames; initialize();};

		// Format constructor
		RtDuplexStream(RtStreamFormat^ format){ Format = format; Frames = 512; initialize();};

		// Format constructor
		RtDuplexStream(RtStreamFormat^ format, unsigned int frames){ Format = format; this->Frames = frames; initialize();};

		// Format constructor (I'm sorry for this terribly long inline constructor, buuut...) 
		RtDuplexStream(::RtAudioNet::RtAudioFormat type, unsigned int channels, unsigned int sampleRate, unsigned int bitsPerSample) 
		{ RtStreamFormat^ format = gcnew RtStreamFormat(); format->type = type; format->channels = channels; format->sampleRate = sampleRate; format->bitsPerSample = bitsPerSample; Format = format; Frames = 512; initialize(); };

		// Format constructor (I'm sorry for this terribly long inline constructor, buuut...) 
		RtDuplexStream(::RtAudioNet::RtAudioFormat type, unsigned int channels, unsigned int sampleRate, unsigned int bitsPerSample, unsigned int frames)
		{ RtStreamFormat^ format = gcnew RtStreamFormat(); format->type = type; format->channels = channels; format->sampleRate = sampleRate; format->bitsPerSample = bitsPerSample; Format = format; this->Frames = frames; initialize(); };

		// Default Destructor
		~RtDuplexStream();

		// Selects the correct input device
		void selectInputDevice(int devID);

		// Selects the correct input device
		void selectInputDevice(String^ devString);

		// Selects the correct input device
		void selectOutputDevice(int devID);

		// Selects the correct input device
		void selectOutputDevice(String^ devString);

		// Read method required by the stream base class.
		virtual int Read([InAttribute] [OutAttribute] array<float>^ buffer, int offset, int count) override;
		
		// Write method required by the stream base class.
		virtual void Write(array<float>^ buffer, int offset, int count) override;

		// Opens the stream
		void Open();

		// Starts the stream
		void Start();

		// Stops the stream
		void Stop();

		// Closes the stream
		void Finish();

		// Aborts the stream
		void Abort();

		// Is the stream a live stream, or a buffered stream?
		bool IsLive();

	protected:
		// Finish initializing the stream
		void initialize();

		// Input stream parameters
		::RtAudioNet::RtAudio::StreamParameters^ inputStreamParams;

		// Output stream parameters
		::RtAudioNet::RtAudio::StreamParameters^ outputStreamParams;

		// The stream's callback
        int callback(IntPtr outputBufferPtr, IntPtr inputBufferPtr, unsigned int frames, double streamTime, ::RtAudioNet::RtAudioStreamStatus status, Object^ userData);
	}; // end RtDuplexStream

	// RtStreamConverter will be a lightweight wrapper around an RtAudioStream. Everytime a Read is called, it will read from the RtAudioStream
	// and then convert the results into the desired format. All other functionality is passthrough to the wrapped RtAudioStream.
	public ref class RtStreamConverter : RtAudioStream
	{
	public:
		RtStreamConverter(RtAudioStream^ stream);

	protected:
		RtAudioStream^ _stream;
	}; // end RtStreamConverter
} // end namespace