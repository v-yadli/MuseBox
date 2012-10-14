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

#include "RtStream.h"
#include "EventLogger.h"

using namespace System::Collections::Generic;

namespace RtStream
{
	// BufferUnderrun Event Args
	public ref class BufferUnderrunEventArgs : EventArgs
	{
	public:
		property int ExpectedBufferSize;
		property int ActualBufferSize;
	}; // end BufferUnderrunEventArgs

	// Wraps an RtInputStream, and provides Pan/Gain support.
	public ref class RtMixerInput
	{
	public:
		// Constructor	
		RtMixerInput();

		// Constructor	
		RtMixerInput(RtInputStream^ stream);

		// Constructor	
		RtMixerInput(RtInputStream^ stream, float gain, float pan);

		// Stream Gain
		property float Gain;

		// Stream Pan
		property float Pan;

		// Stream
		property RtInputStream^ InputStream;
	};

	public ref class RtStreamMixer
	{
	public:
		// Default Constructor
		RtStreamMixer();

		// Default Destructor
		~RtStreamMixer();

		// Add an input stream to the mixer
		virtual void AddInputStream(RtInputStream^ inputStream);

		// Add an input stream to the mixer
		virtual void AddInputStream(RtInputStream^ inputStream, float gain, float pan);

		// Add an input stream to the mixer
		virtual void AddInputStream(RtMixerInput^ input);

		// Add an outputstream to the mixer
		virtual void SetOutputStream(RtOutputStream^ outputStream);

		// Adjust the gain on the selected input
		virtual void AdjustGain(String^ inputName, float gain);

		// Adjust the pan on the selected input
		virtual void AdjustPan(String^ inputName, float pan);

		// Start the mixer
		virtual void Start();

		// Stop the mixer
		virtual void Stop();

		// Called whenever the RtAudio callback is fired.
		event EventHandler<BufferUnderrunEventArgs^>^ BufferUnderrun;

		// Is the mixer currently running?
		bool IsRunning();

		// Output Format
		property RtStreamFormat^ Format;

		// Frames of audio to buffer
		property unsigned int FramesToBuffer;

		// Overall Gain on the output of this mixer
		property float OutputGain;

		// Dictionary of mixer inputs
		property Dictionary<String^, RtMixerInput^>^ Inputs { Dictionary<String^, RtMixerInput^>^ get() { return inputs;}; };

	protected:
		// Callback Event Handler
		void callbackHandler(Object^ sender, EventArgs^ e);

		// Internal buffer
		CircularBuffer<float>^ internalBuffer;

		// Our logger
		EventLogger^ logger;

		// Is the mixer running
		bool running;

		// Frames we've buffered
		unsigned int framesBuffered;

		// All our internal inputStreams
		Dictionary<String^, RtMixerInput^>^ inputs;

		// Our outputStream
		RtOutputStream^ outputStream;
	}; // end RtStreamMixer

	public ref class RtDuplexMixer : RtStreamMixer
	{
	public:
		// Default Constructor
		RtDuplexMixer();

		// Default Constructor
		~RtDuplexMixer();

		// Add an input stream to the mixer
		virtual void AddInputStream(RtInputStream^ inputStream) override;

		// Add an input stream to the mixer
		virtual void AddInputStream(RtInputStream^ inputStream, float gain, float pan) override;

		// Add an input stream to the mixer
		virtual void AddInputStream(RtMixerInput^ input) override;

		// Add an outputstream to the mixer
		virtual void SetOutputStream(RtOutputStream^ outputStream) override;

		// Start the mixer
		virtual void Start() override;

		// Stop the mixer
		virtual void Stop() override;

	private:
		// Internal Duplex Stream
		RtDuplexStream^ duplexStream;
	}; // end RtDuplexMixer
} // end namespace