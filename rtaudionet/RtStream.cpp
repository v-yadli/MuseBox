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

#include "RtStream.h"
#include <msclr\lock.h>

namespace RtStream
{
	//////////////////////////////////////////////////////////////////////////
	/// RtAudioStream Class

	RtAudioStream::RtAudioStream()
	{
		logger = EventLoggerManager::getLogger("RtStream");

		// Since we don't use it, there's no need to instantiate it.
		//rtaudio = gcnew ::RtAudioNet::RtAudio();
		logger->Trace("Setting internal buffer.");
		internalBuffer = gcnew CircularBuffer<float>(4096);

		logger->Trace("Setting properties...");
		_canRead = true;
		_canWrite = true;
		_canSeek = false;
		logger->Trace("Properties set: _canRead: {0}, _canWrite: {1}, _canSeek: {2}.", _canRead, _canWrite, _canSeek);

		Name = "";

		logger->Trace("Finished constructor.");
	} // end RtWaveStream
	
	// Read method required by the stream base class.
	int RtAudioStream::Read([InAttribute] [OutAttribute] array<unsigned char>^ buffer, int offset, int count)
	{
		logger->Trace("Read([InAttribute] [OutAttribute] array<unsigned char>^ buffer, int offset, int count) called.");

		logger->Trace("Creating tempBuff of size {0}", (buffer->Length / 4));
		array<float>^ tempBuff = gcnew array<float>(buffer->Length / 4);

		logger->Trace("Populating tempBuff from internal buffer.");
		int bytesRead = 4 * internalBuffer->Get(tempBuff, offset, count);

		// Copy to byte array
		logger->Trace("Performing BlockCopy.");
		Buffer::BlockCopy(tempBuff, 0, buffer, 0, buffer->Length);

		logger->Trace("Returning bytesRead({0})", bytesRead);
		return bytesRead;
	} // end Read

	// Read method required by the stream base class.
	int RtAudioStream::Read([InAttribute] [OutAttribute] array<float>^ buffer, int offset, int count)
	{
		logger->Trace("Read([InAttribute] [OutAttribute] array<float>^ buffer, int offset, int count) called.");
		return internalBuffer->Get(buffer, offset, count); 
	} // end Read

	// Read method required by the stream base class.
	int RtAudioStream::Read(float buffer[], int offset, int count)
	{
		logger->Trace("Read(float buffer[], int offset, int count) called.");

		logger->Trace("Creating tempBuff of size {0}", count);
		array<float>^ tempBuff = gcnew array<float>(count);

		logger->Trace("Populating tempBuff from internal buffer.");
		int read = internalBuffer->Get(tempBuff, 0, count); 

		// Copy to float array
		logger->Trace("Performing Marshal::Copy.");
		Marshal::Copy(tempBuff, 0, IntPtr(buffer + offset), read);

		if (read != count)
		{
			logger->Trace("Stream \"{0}\": Buffer Underrun detected!", Name);
			BufferUnderrun(this, gcnew EventArgs());
		} // end if

		logger->Trace("Returning read({0})", read);
		return read;
	} // end Read

	// Read class that's more convienent.
	int RtAudioStream::Read([InAttribute] [OutAttribute] array<float>^ buffer)
	{
		logger->Trace("Read([InAttribute] [OutAttribute] array<float>^ buffer) called.");
		return Read(buffer, 0, buffer->Length);
	} // end Read
	
	// Write method required by the stream base class.
	void RtAudioStream::Write(array<unsigned char>^ buffer, int offset, int count)
	{
		logger->Trace("Write(array<unsigned char>^ buffer, int offset, int count) called.");


		logger->Trace("Creating tempBuff of size {0}", (buffer->Length / 4));
		array<float>^ tempBuff = gcnew array<float>(buffer->Length / 4);

		logger->Trace("Populating tempBuff from internal buffer.");
		int bytesRead = 4 * internalBuffer->Get(tempBuff, offset, count);

		// Copy to byte array
		logger->Trace("Performing BlockCopy.");
		Buffer::BlockCopy(buffer, 0, tempBuff, 0, buffer->Length);

		logger->Trace("Writing {0} bytes.", bytesRead);
		internalBuffer->Put(tempBuff, offset, count);
	} //end Write

	// Write method required by the stream base class.
	void RtAudioStream::Write(array<float>^ buffer, int offset, int count)
	{
		logger->Trace("Write(array<float>^ buffer, int offset, int count) called.");
		internalBuffer->Put(buffer, offset, count);
	} //end Write

	// Write method required by the stream base class.
	void RtAudioStream::Write(float buffer[], int offset, int count)
	{
		logger->Trace("Write(float buffer[], int offset, int count) called.");

		logger->Trace("Creating tempBuff of size {0}", count);
		array<float>^ tempBuff = gcnew array<float>(count);

		// Copy to float array
		logger->Trace("Performing Marshal::Copy.");
		Marshal::Copy(IntPtr(buffer + offset), tempBuff, 0, count);

		logger->Trace("Writing {0} bytes.", count);
		internalBuffer->Put(tempBuff, 0, count);
	} //end Write

	// Write class that's more convienent.
	void RtAudioStream::Write(array<float>^ buffer)
	{
		logger->Trace("Write(array<float>^ buffer) called.");
		Write(buffer, 0, buffer->Length);
	} // end Write


	//////////////////////////////////////////////////////////////////////////
	/// RtInputStream Class

	// Finish initializing the stream
	void RtInputStream::initialize()
	{
		logger = EventLoggerManager::getLogger("RtInputStream");

		logger->Trace("initialize called.");

		logger->Debug("Initializing RtAudioNet instance.");
		rtaudio = gcnew ::RtAudioNet::RtAudio();

		logger->Trace("Initializing new StreamParameters.");
		inputStreamParams = gcnew ::RtAudioNet::RtAudio::StreamParameters();
		
		if (Format == nullptr)
		{
			logger->Trace("Format null, creating new Format instance.");
			Format = gcnew RtStreamFormat();
			Format->sampleRate = 22050;
			Format->channels = 2;
			Format->bitsPerSample = 16;
		} // end if

		// (Frames per sample * channels) * numSamplesToBuffer
		logger->Debug("Creating new internal buffer of size: {0}", (Frames * Format->channels * 2));
		internalBuffer = gcnew CircularBuffer<float>((Frames * Format->channels * 2), true);

		// Set stream properties
		logger->Trace("Setting properties...");
		_canRead = true;
		_canWrite = false;
		_canSeek = false;
		logger->Trace("Properties set: _canRead: {0}, _canWrite: {1}, _canSeek: {2}.", _canRead, _canWrite, _canSeek);

		Name = "";

		logger->Trace("Finished initialize.");
	} // end initialize
	
	RtInputStream::~RtInputStream()	
	{
		logger->Trace("~RtInputStream called.");
		if (rtaudio->isStreamRunning())
		{
			logger->Debug("In destructor; stopping stream.");
			rtaudio->stopStream();
		} // end if

		if (rtaudio->isStreamOpen())
		{
			logger->Debug("In destructor; closing stream.");
			rtaudio->closeStream();
		} // end if
	} // end ~RtDuplexStream

	// Selects the correct input device
	void RtInputStream::selectInputDevice(int devID)
	{
		logger->Trace("selectInputDevice(int devID) called.");

		logger->Debug("Setting DeviceID to {0}", devID);
		DeviceID = devID;

		logger->Trace("Getting device info, checking channels.");
		RtAudioNet::RtAudio::DeviceInfo^ info = rtaudio->getDeviceInfo(devID);
		int channels = info->inputChannels;

		if(channels > 0 && channels <= 2)
		{
			logger->Debug("Detected {0} channel(s). Setting Format.", channels);
			Format->channels = channels;
		}
		else
		{
			logger->Debug("Detected {0} channel(s). Setting Format to 2.", channels);
			Format->channels = 2;
		} // end if

		logger->Trace("Setting inputStreamParams: deviceId: {0}, nChannels: {1}", devID, Format->channels);
		inputStreamParams->deviceId = devID;
		inputStreamParams->nChannels = Format->channels;
	} // end selectInputDevice

	// Selects the correct input device
	void RtInputStream::selectInputDevice(String^ devString)
	{
		logger->Trace("selectInputDevice(String^ devString) called.");

		unsigned int devID = 0;

		for (unsigned int idx = 0; rtaudio->getDeviceCount() > idx; idx++)
		{
			logger->Trace("Getting device info.");
			::RtAudioNet::RtAudio::DeviceInfo^ info = rtaudio->getDeviceInfo(idx);

			// Case insensitive search
			logger->Debug("Checking to see if \"{0}\" is in \"{1}\".", devString, info->name);
			if (info->name->IndexOf(devString, StringComparison::OrdinalIgnoreCase) >= 0)
			{
				logger->Debug("Found \"{0}\". Setting devID to {0}.", devString, idx);
				devID = idx;
			} // end if
		} // end for

		selectInputDevice(devID);
	} // end selectInputDevice

	// Opens the stream
	void RtInputStream::Open()
	{
		logger->Trace("Open() called.");

		if (Format->options == nullptr)
		{
			logger->Trace("Format->options is null. Calling rtaudio->openStream.");
			rtaudio->openStream(nullptr, inputStreamParams, Format->type, Format->sampleRate, Frames, gcnew ::RtAudioNet::RtAudioNetCallback(this, &RtInputStream::callback));
		}
		else
		{
			logger->Trace("Format->options is not null. Calling rtaudio->openStream.");
			rtaudio->openStream(nullptr, inputStreamParams, Format->type, Format->sampleRate, Frames, gcnew ::RtAudioNet::RtAudioNetCallback(this, &RtInputStream::callback), nullptr, Format->options);
		} // end if
	} // end Open

	// Starts the stream
	void RtInputStream::Start()
	{
		logger->Trace("Start() called.");

		int count = 0;	
		while (!(rtaudio->isStreamOpen() && count < 100))
		{
			logger->Trace("Stream not open. Sleeping for 10ms.");
			System::Threading::Thread::Sleep(10);
			count++;
		} // end while

		logger->Trace("Calling rtaudio->startStream()");
		rtaudio->startStream();
	} // end Start

	// Stops the stream
	void RtInputStream::Stop()
	{
		logger->Trace("Stop() called.");

		logger->Trace("Calling rtaudio->stopStream().");
		rtaudio->stopStream();
	} // end Stop
	
	// Stops the stream
	void RtInputStream::Finish()
	{
		logger->Trace("Finish() called.");

		logger->Trace("Calling rtaudio->closeStream().");
		rtaudio->closeStream();
	} // end Stop

	// Aborts the stream
	void RtInputStream::Abort()
	{
		logger->Trace("Abort() called.");

		logger->Trace("Calling rtaudio->abortStream().");
		rtaudio->abortStream();
	} // end Abort
	
	// Read class required by the stream base class.
	int RtInputStream::Read([InAttribute] [OutAttribute] array<float>^ buffer, int offset, int count)
	{
		logger->Trace("Read([InAttribute] [OutAttribute] array<float>^ buffer, int offset, int count) called.");

		int dataRead = 0;

		logger->Trace("Aquiring lock on internal buffer.");
		msclr::lock lk(internalBuffer);


		logger->Trace("Populating buffer from internal buffer.");
		dataRead = internalBuffer->Get(buffer, offset, count); 

		if (dataRead != count)
		{
			logger->Trace("Stream \"{0}\": Buffer Underrun detected!", Name);
			BufferUnderrun(this, gcnew EventArgs());
		} // end if

		logger->Trace("Returning dataRead({0})", dataRead);
		return dataRead;
	} // end Read
	
	// Write class required by the stream base class.
	void RtInputStream::Write(array<float>^ buffer, int offset, int count)
	{
		logger->Warn("Called Write on an RtInputStream!");
		throw gcnew System::NotImplementedException();
	} // end Write

	// Is the stream a live stream, or a buffered stream?
	bool RtInputStream::IsLive()
	{
		logger->Trace("IsLive() called. Returning true.");
		return true;
	} // end write

	// XXX: No logging in the callback for performance reasons. Not actually sure if it will make a difference, but for now, I'm leaving it out.
	// For reference: http://stackoverflow.com/questions/3847727/performance-implications-of-net-events
	int RtInputStream::callback(IntPtr outputBufferPtr, IntPtr inputBufferPtr, unsigned int frames, double streamTime, ::RtAudioNet::RtAudioStreamStatus status, Object^ userData)
	{
		// Create our temporary buffer
		unsigned int floatsToCopy = frames * Format->channels;
		array<float>^ tempBuff = gcnew array<float>(floatsToCopy);

		// Not sure this shouldn't be a memcopy. However, I know this works for managed types, and we don't lose that much speed. Future optimization?
		Marshal::Copy(inputBufferPtr, tempBuff, 0, floatsToCopy);
		
		msclr::lock lk(internalBuffer);

		// And now, we add out temp buffer to our main buffer.
		internalBuffer->Put(tempBuff);
		
		lk.release();

		// Fire the callback event.
		CallbackFired(this, nullptr);
		
		// We always return zero.
		return 0;
	} // end callback


	//////////////////////////////////////////////////////////////////////////
	/// RtOutputStream Class

	// Finish initializing the stream
	void RtOutputStream::initialize()
	{
		logger = EventLoggerManager::getLogger("RtOutputStream");

		logger->Trace("initialize called.");

		logger->Debug("Initializing RtAudioNet instance.");
		rtaudio = gcnew ::RtAudioNet::RtAudio();

		logger->Trace("Initializing new StreamParameters.");
		outputStreamParams = gcnew ::RtAudioNet::RtAudio::StreamParameters();

		if (Format == nullptr)
		{
			logger->Trace("Format null, creating new Format instance.");
			Format = gcnew RtStreamFormat();
			Format->sampleRate = 22050;
			Format->channels = 2;
			Format->bitsPerSample = 16;
		} // end if
		
		// (Frames per sample * channels) * numSamplesToBuffer
		logger->Debug("Creating new internal buffer of size: {0}", (Frames * Format->channels * 2));
		internalBuffer = gcnew CircularBuffer<float>((Frames * Format->channels * 2), true);

		// Set stream properties
		logger->Trace("Setting properties...");
		_canRead = false;
		_canWrite = true;
		_canSeek = false;
		logger->Trace("Properties set: _canRead: {0}, _canWrite: {1}, _canSeek: {2}.", _canRead, _canWrite, _canSeek);

		Name = "";

		logger->Trace("Finished initialize.");
	} // end initialize
	
	RtOutputStream::~RtOutputStream()	
	{
		logger->Trace("~RtOutputStream called.");

		if (rtaudio->isStreamRunning())
		{
			logger->Debug("In destructor; stopping stream.");
			rtaudio->stopStream();
		} // end if

		if (rtaudio->isStreamOpen())
		{
			logger->Debug("In destructor; closing stream.");
			rtaudio->closeStream();
		} // end if
	} // end ~RtDuplexStream

	// Selects the correct output device
	void RtOutputStream::selectOutputDevice(int devID)
	{
		logger->Trace("selectOutputDevice(int devID) called.");

		logger->Debug("Setting DeviceID to {0}", devID);
		DeviceID = devID;

		logger->Trace("Getting device info, checking channels.");
		RtAudioNet::RtAudio::DeviceInfo^ info = rtaudio->getDeviceInfo(devID);
		int channels = info->outputChannels;

		if(channels > 0 && channels <= 2)
		{
			logger->Debug("Detected {0} channel(s). Setting Format.", channels);
			Format->channels = channels;
		}
		else
		{
			logger->Debug("Detected {0} channel(s). Setting Format to 2.", channels);
			Format->channels = 2;
		} // end if

		logger->Trace("Setting outputStreamParams: deviceId: {0}, nChannels: {1}", devID, Format->channels);
		outputStreamParams->deviceId = devID;
		outputStreamParams->nChannels = Format->channels;
	} // end selectOutputDevice

	// Selects the correct output device
	void RtOutputStream::selectOutputDevice(String^ devString)
	{
		logger->Trace("selectOutputDevice(String^ devString) called.");

		unsigned int devID = 0;

		for (unsigned int idx = 0; rtaudio->getDeviceCount() > idx; idx++)
		{
			logger->Trace("Getting device info.");
			::RtAudioNet::RtAudio::DeviceInfo^ info = rtaudio->getDeviceInfo(idx);

			// Case insensitive search
			logger->Debug("Checking to see if \"{0}\" is in \"{1}\".", devString, info->name);
			if (info->name->IndexOf(devString, StringComparison::OrdinalIgnoreCase) >= 0)
			{
				logger->Debug("Found \"{0}\". Setting devID to {0}.", devString, idx);
				devID = idx;
			} // end if
		} // end for

		selectOutputDevice(devID);
	} // end selectOutputDevice

	// Opens the stream
	void RtOutputStream::Open()
	{
		logger->Trace("Open() called.");

		if (Format->options == nullptr)
		{
			logger->Trace("Format->options is null. Calling rtaudio->openStream.");
			rtaudio->openStream(outputStreamParams, nullptr, Format->type, Format->sampleRate, Frames, gcnew ::RtAudioNet::RtAudioNetCallback(this, &RtOutputStream::callback));
		}
		else
		{
			logger->Trace("Format->options is not null. Calling rtaudio->openStream.");
			rtaudio->openStream(outputStreamParams, nullptr, Format->type, Format->sampleRate, Frames, gcnew ::RtAudioNet::RtAudioNetCallback(this, &RtOutputStream::callback), nullptr, Format->options);
		} // end if
	} // end Open

	// Starts the stream
	void RtOutputStream::Start()
	{
		logger->Trace("Start() called.");

		int count = 0;	
		while (!(rtaudio->isStreamOpen() && count < 100))
		{
			logger->Trace("Stream not open. Sleeping for 10ms.");
			System::Threading::Thread::Sleep(10);
			count++;
		} // end while

		logger->Trace("Calling rtaudio->startStream()");
		rtaudio->startStream();
	} // end Start

	// Stops the stream
	void RtOutputStream::Stop()
	{
		logger->Trace("Stop() called.");

		logger->Trace("Calling rtaudio->stopStream().");
		rtaudio->stopStream();
	} // end Stop
	
	// Stops the stream
	void RtOutputStream::Finish()
	{
		logger->Trace("Finish() called.");

		logger->Trace("Calling rtaudio->closeStream().");
		rtaudio->closeStream();
	} // end Stop

	// Aborts the stream
	void RtOutputStream::Abort()
	{
		logger->Trace("Abort() called.");

		logger->Trace("Calling rtaudio->abortStream().");
		rtaudio->abortStream();
	} // end Abort

	// Read class required by the stream base class.
	int RtOutputStream::Read([InAttribute] [OutAttribute] array<float>^ buffer, int offset, int count)
	{
		logger->Warn("Called Read on an RtOutputStream!");
		throw gcnew System::NotImplementedException();
	} // end Read
	
	// Write class required by the stream base class.
	void RtOutputStream::Write(array<float>^ buffer, int offset, int count)
	{
		logger->Trace("Write(array<float>^ buffer, int offset, int count) called.");

		logger->Trace("Aquiring lock on internal buffer.");
		msclr::lock lk(internalBuffer);

		logger->Trace("Writing {0} bytes.", count);
		internalBuffer->Put(buffer, offset, count);
	} // end Write

	// Is the stream a live stream, or a buffered stream?
	bool RtOutputStream::IsLive()
	{
		logger->Trace("IsLive() called. Returning true.");
		return true;
	} // end write

	// XXX: No logging in the callback for performance reasons. Not actually sure if it will make a difference, but for now, I'm leaving it out.
	// For reference: http://stackoverflow.com/questions/3847727/performance-implications-of-net-events
	int RtOutputStream::callback(IntPtr outputBufferPtr, IntPtr inputBufferPtr, unsigned int frames, double streamTime, ::RtAudioNet::RtAudioStreamStatus status, Object^ userData)
	{
		// Create our temporary buffer
		unsigned int floatsToCopy = frames * Format->channels;
		array<float>^ tempBuff = gcnew array<float>(floatsToCopy);

		msclr::lock lk(internalBuffer);
		
		// And now, we add out temp buffer to our main buffer.
		int bytesCopied = internalBuffer->Get(tempBuff);

		lk.release();

		// Not sure this shouldn't be a memcopy. However, I know this works for managed types, and we don't lose that much speed. Future optimization?
		Marshal::Copy(tempBuff, 0, outputBufferPtr, bytesCopied);

		// Fire the callback event.
		CallbackFired(this, nullptr);
		
		// We always return zero.
		return 0;
	} // end callback


	//////////////////////////////////////////////////////////////////////////
	/// RtDuplexStream Class

	// Finish initializing the stream
	void RtDuplexStream::initialize()
	{
		logger = EventLoggerManager::getLogger("RtDuplexStream");

		logger->Trace("initialize called.");

		logger->Debug("Initializing RtAudioNet instance.");
		rtaudio = gcnew ::RtAudioNet::RtAudio();

		logger->Trace("Initializing new StreamParameters.");
		inputStreamParams = gcnew ::RtAudioNet::RtAudio::StreamParameters();
		outputStreamParams = gcnew ::RtAudioNet::RtAudio::StreamParameters();

		if (Format == nullptr)
		{
			logger->Trace("Format null, creating new Format instance.");
			Format = gcnew RtStreamFormat();
			Format->sampleRate = 22050;
			Format->channels = 2;
			Format->bitsPerSample = 16;
		} // end if
		
		DeviceID = -1;

		// (Frames per sample * channels * bytes) * numSamplesToBuffer in an int	
		//internalBuffer = gcnew CircularBuffer<unsigned char>((frames * Format->channels * 4 * 2), true);

		// Set stream properties
		logger->Trace("Setting properties...");
		_canRead = false;
		_canWrite = false;
		_canSeek = false;
		logger->Trace("Properties set: _canRead: {0}, _canWrite: {1}, _canSeek: {2}.", _canRead, _canWrite, _canSeek);

		Name = "";

		logger->Trace("Finished initialize.");
	} // end initialize
	
	RtDuplexStream::~RtDuplexStream()	
	{
		logger->Trace("~RtDuplexStream called.");

		if (rtaudio->isStreamRunning())
		{
			logger->Debug("In destructor; stopping stream.");
			rtaudio->stopStream();
		} // end if

		if (rtaudio->isStreamOpen())
		{
			logger->Debug("In destructor; closing stream.");
			rtaudio->closeStream();
		} // end if
	} // end ~RtDuplexStream

	// Selects the correct input device
	void RtDuplexStream::selectInputDevice(int devID)
	{
		logger->Trace("selectInputDevice(int devID) called.");

		logger->Trace("Setting devideID to {0}, nChannels to {1}.", devID, Format->channels);
		inputStreamParams->deviceId = devID;
		inputStreamParams->nChannels = Format->channels;
	} // end selectInputDevice

	// Selects the correct input device
	void RtDuplexStream::selectInputDevice(String^ devString)
	{
		logger->Trace("selectInputDevice(String^ devString) called.");

		unsigned int devID = 0;

		for (unsigned int idx = 0; rtaudio->getDeviceCount() > idx; idx++)
		{
			::RtAudioNet::RtAudio::DeviceInfo^ info = rtaudio->getDeviceInfo(idx);

			// Case insensitive search
			logger->Debug("Checking to see if \"{0}\" is in \"{1}\".", devString, info->name);
			if (info->name->IndexOf(devString, StringComparison::OrdinalIgnoreCase) >= 0)
			{
				logger->Debug("Found \"{0}\". Setting devID to {0}.", devString, idx);
				devID = idx;
			} // end if
		} // end for

		selectInputDevice(devID);
	} // end selectInputDevice
	// Selects the correct output device
	void RtDuplexStream::selectOutputDevice(int devID)
	{
		logger->Trace("selectOutputDevice(int devID) called.");

		logger->Trace("Setting devideID to {0}, nChannels to {1}.", devID, Format->channels);
		outputStreamParams->deviceId = devID;
		outputStreamParams->nChannels = Format->channels;
	} // end selectOutputDevice

	// Selects the correct output device
	void RtDuplexStream::selectOutputDevice(String^ devString)
	{
		logger->Trace("selectOutputDevice(String^ devString) called.");

		unsigned int devID = 0;

		for (unsigned int idx = 0; rtaudio->getDeviceCount() > idx; idx++)
		{
			logger->Trace("Getting device info.");
			::RtAudioNet::RtAudio::DeviceInfo^ info = rtaudio->getDeviceInfo(idx);

			// Case insensitive search
			logger->Debug("Checking to see if \"{0}\" is in \"{1}\".", devString, info->name);
			if (info->name->IndexOf(devString, StringComparison::OrdinalIgnoreCase) >= 0)
			{
				logger->Debug("Found \"{0}\". Setting devID to {0}.", devString, idx);
				devID = idx;
			} // end if
		} // end for

		selectOutputDevice(devID);
	} // end selectOutputDevice

	// Opens the stream
	void RtDuplexStream::Open()
	{
		logger->Trace("Open() called.");

		if (Format->options == nullptr)
		{
			logger->Trace("Format->options is null. Calling rtaudio->openStream.");
			rtaudio->openStream(outputStreamParams, inputStreamParams, Format->type, Format->sampleRate, Frames, gcnew ::RtAudioNet::RtAudioNetCallback(this, &RtDuplexStream::callback));
		}
		else
		{
			logger->Trace("Format->options is not null. Calling rtaudio->openStream.");
			rtaudio->openStream(outputStreamParams, inputStreamParams, Format->type, Format->sampleRate, Frames, gcnew ::RtAudioNet::RtAudioNetCallback(this, &RtDuplexStream::callback), nullptr, Format->options);
		} // end if
	} // end Open

	// Starts the stream
	void RtDuplexStream::Start()
	{
		logger->Trace("Start() called.");

		int count = 0;	
		while (!(rtaudio->isStreamOpen() && count < 100))
		{
			logger->Trace("Stream not open. Sleeping for 10ms.");
			System::Threading::Thread::Sleep(10);
			count++;
		} // end while

		logger->Trace("Calling rtaudio->startStream()");
		rtaudio->startStream();
	} // end Start

	// Stops the stream
	void RtDuplexStream::Stop()
	{
		logger->Trace("Stop() called.");

		logger->Trace("Calling rtaudio->stopStream().");
		rtaudio->stopStream();
	} // end Stop
	
	// Stops the stream
	void RtDuplexStream::Finish()
	{
		logger->Trace("Finish() called.");

		logger->Trace("Calling rtaudio->closeStream().");
		rtaudio->closeStream();
	} // end Stop

	// Aborts the stream
	void RtDuplexStream::Abort()
	{
		logger->Trace("Abort() called.");

		logger->Trace("Calling rtaudio->abortStream().");
		rtaudio->abortStream();
	} // end Abort

	// Read class required by the stream base class.
	int RtDuplexStream::Read([InAttribute] [OutAttribute] array<float>^ buffer, int offset, int count)
	{
		logger->Warn("Called Read on an RtDuplexStream!");
		throw gcnew System::NotImplementedException();
	} // end Read
	
	// Write class required by the stream base class.
	void RtDuplexStream::Write(array<float>^ buffer, int offset, int count)
	{
		logger->Warn("Called Write on an RtDuplexStream!");
		throw gcnew System::NotImplementedException();
	} // end Write

	// Is the stream a live stream, or a buffered stream?
	bool RtDuplexStream::IsLive()
	{
		logger->Trace("IsLive() called. Returning true.");
		return true;
	} // end write

	// XXX: No logging in the callback for performance reasons. Not actually sure if it will make a difference, but for now, I'm leaving it out.
	// For reference: http://stackoverflow.com/questions/3847727/performance-implications-of-net-events
	int RtDuplexStream::callback(IntPtr outputBufferPtr, IntPtr inputBufferPtr, unsigned int frames, double streamTime, ::RtAudioNet::RtAudioStreamStatus status, Object^ userData)
	{
		// Create our temporary buffer
		unsigned int bytesToCopy = frames * Format->channels * (Format->bitsPerSample / 8);

		// XXX: On Winows XP, this code gets an access violation:

		/*
		array<float>^ tempBuff = gcnew array<float>(bytesToCopy);

		// Not sure this shouldn't be a memcopy. However, I know this works for managed types, and we don't lose that much speed. Future optimization?
		Marshal::Copy(inputBufferPtr, tempBuff, 0, bytesToCopy);

		// Not sure this shouldn't be a memcopy. However, I know this works for managed types, and we don't lose that much speed. Future optimization?
		Marshal::Copy(tempBuff, 0, outputBufferPtr, bytesToCopy);
		*/

		// This is the absolute fastest option available to us, and it seems to work well.
		memcpy(outputBufferPtr.ToPointer(), inputBufferPtr.ToPointer(), bytesToCopy);

		// We always return zero.
		return 0;
	} // end callback


	//////////////////////////////////////////////////////////////////////////
	/// RtStreamConverter Class

	// TODO: Finish this...
	RtStreamConverter::RtStreamConverter(RtAudioStream^ stream)
	{
		rtaudio = gcnew ::RtAudioNet::RtAudio();
		internalBuffer = gcnew CircularBuffer<float>(4096);
		_stream = stream;
	} // end RtStreamConverter

} // end namespace