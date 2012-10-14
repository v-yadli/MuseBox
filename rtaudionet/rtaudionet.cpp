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

#include <msclr\marshal.h>
#include <msclr\marshal_cppstd.h>

#include "RtAudioNet.h"

using namespace msclr::interop;
using namespace System::Runtime::InteropServices;

namespace RtAudioNet
{
	
	//////////////////////////////////////////////////////////////////////////
	/// RtAudio Class

	int RtAudio::GetSuggestedBufferLength()
	{
		return _rtaudio->getPreferredBufferSize();
	}

	// A static function to determine the available compiled audio APIs.
	List<RtAudio::Api>^ RtAudio::getCompiledApi()
	{
		List<RtAudio::Api>^ apiList = gcnew List<RtAudio::Api>();
		::std::vector<::RtAudio::Api> _apiList;

		// Call the unmanaged function
		::RtAudio::getCompiledApi(_apiList);
		
		// Iterate over the vector and populate apiList
		for(std::vector<::RtAudio::Api>::iterator it = _apiList.begin(); it != _apiList.end(); ++it) 
		{
			apiList->Add(static_cast<RtAudio::Api>(*it));
		} // end for

		return apiList;
	} // end getCompiledApi
		
	// The class constructor.
	RtAudio::RtAudio()
	{
		logger = EventLoggerManager::getLogger("RtAudioNet");

		// This should be the only line in this function. All other code should be in initialize.
		initialize(Api::UNSPECIFIED);
	} // end RtAudio

	// The class constructor.
	RtAudio::RtAudio(RtAudio::Api api)
	{
		// This should be the only line in this function. All other code should be in initialize.
		initialize(api);
	} // end RtAudio
	
	// The destructor.
	RtAudio::~RtAudio()
	{
		delete _rtaudio;
		// Cleanup GCHandle
		//handle.Free();
	} // end ~RtAudio
	
	// Returns the audio API specifier for the current instance of RtAudio.
	RtAudio::Api RtAudio::getCurrentApi()
	{
		try
		{
    		return static_cast<RtAudio::Api>(_rtaudio->getCurrentApi());
		}
        catch (::RtError &exception)
		{
			throwError(&exception);
			return Api::UNSPECIFIED;
		} // end try/catch
	} // end getCurrentApi
	
	// A public function that queries for the number of audio devices available.
	unsigned int RtAudio::getDeviceCount()
	{
		try
		{
    		return _rtaudio->getDeviceCount();
		}
        catch (::RtError &exception)
		{
			throwError(&exception);
			return 0;
		} // end try/catch
	} // end getDeviceCount
	
	// Return an RtAudio::DeviceInfo structure for a specified device number.
	RtAudio::DeviceInfo^ RtAudio::getDeviceInfo(unsigned int device)
	{
		try
		{
    		// Get the unmanaged DeviceInfo class
    		return gcnew RtAudio::DeviceInfo(_rtaudio->getDeviceInfo(device));
		}
        catch (::RtError &exception)
		{
			throwError(&exception);
			return nullptr;
		} // end try/catch
	} // end getDeviceInfo
	
	// A function that returns the index of the default output device.
	unsigned int RtAudio::getDefaultOutputDevice()
	{
		try
		{
    		return _rtaudio->getDefaultOutputDevice();
		}
        catch (::RtError &exception)
		{
			throwError(&exception);
			return 0;
		} // end try/catch
	} // end getDefaultOutputDevice
	
	// A function that returns the index of the default input device.
	unsigned int RtAudio::getDefaultInputDevice()
	{
		try
		{
    		return _rtaudio->getDefaultInputDevice();
		}
        catch (::RtError &exception)
		{
			throwError(&exception);
			return 0;
		} // end try/catch
	} // end getDefaultInputDevice
	
	// A public function for opening a stream with the specified parameters.
	void RtAudio::openStream(RtAudio::StreamParameters^ outputParameters, RtAudio::StreamParameters^ inputParameters, RtAudioFormat format, 
		unsigned int sampleRate, unsigned int bufferFrames, RtAudioNetCallback^ callback, Object^ userData, RtAudio::StreamOptions^ options)
	{
		// Painful conversion code
		GCHandle handle = GCHandle::Alloc(this, GCHandleType::Normal);

		try
		{
    		// Call the unmanaged function
    		_rtaudio->openStream(convertManagedToUnmanaged(outputParameters), convertManagedToUnmanaged(inputParameters), (int) format, sampleRate, &bufferFrames, &audioCallback, static_cast<IntPtr>(handle).ToPointer(), convertManagedToUnmanaged(options));
		}
        catch (::RtError &exception)
		{
			throwError(&exception);
		} // end try/catch

		// Set our callback to the delegate we got from the caller.
		_callback = callback;

		//Set userData
		this->userData = userData;
	} // end openStream
	
	// A function that closes a stream and frees any associated stream memory.
	void RtAudio::closeStream()
	{
		try
		{
    		_rtaudio->closeStream();
		}
        catch (::RtError &exception)
		{
			throwError(&exception);
		} // end try/catch
	} // end closeStream()
	
	// A function that starts a stream.
	void RtAudio::startStream()
	{
		try
		{
    		_rtaudio->startStream();
		}
        catch (::RtError &exception)
		{
			throwError(&exception);
		} // end try/catch
	} // end startStream
	
	// Stop a stream, allowing any samples remaining in the output queue to be played.
	void RtAudio::stopStream()
	{
		try
		{
    		_rtaudio->stopStream();
		}
        catch (::RtError &exception)
		{
			throwError(&exception);
		} // end try/catch
	} // end stopStream
	
	// Stop a stream, discarding any samples remaining in the input/output queue.
	void RtAudio::abortStream()
	{
		try
		{
    		_rtaudio->abortStream();
		}
        catch (::RtError &exception)
		{
			throwError(&exception);
		} // end try/catch
	} // end abortStream
	
	// Returns true if a stream is open and false if not.
	bool RtAudio::isStreamOpen()
	{
		try
		{
    		return _rtaudio->isStreamOpen();
		}
        catch (::RtError &exception)
		{
			throwError(&exception);
			return false;
		} // end try/catch
	} // end isStreamOpen()
	
	// Returns true if the stream is running and false if it is stopped or not open.
	bool RtAudio::isStreamRunning()
	{
		try
		{
    		return _rtaudio->isStreamRunning();
		}
        catch (::RtError &exception)
		{
			throwError(&exception);
			return false;
		} // end try/catch
	} // end isStreamRunning
	
	// Returns the number of elapsed seconds since the stream was started.
	double RtAudio::getStreamTime()
	{
		try
		{
    		return _rtaudio->getStreamTime();
		}
        catch (::RtError &exception)
		{
			throwError(&exception);
			return 0.0;
		} // end try/catch
	} // end getStreamTime
	
	// Returns the internal stream latency in sample frames.
	long RtAudio::getStreamLatency()
	{
		try
		{
    		return _rtaudio->getStreamLatency();
		}
        catch (::RtError &exception)
		{
			throwError(&exception);
			return -1;
		} // end try/catch
	} // end getStreamLatency
	
	// Returns actual sample rate in use by the stream.
	unsigned int RtAudio::getStreamSampleRate()
	{
		try
		{
    		return _rtaudio->getStreamSampleRate();
		}
        catch (::RtError &exception)
		{
			throwError(&exception);
			return 0;
		} // end try/catch
	} // end getStreamSampleRate
	
	// Specify whether warning messages should be printed to stderr.
	void RtAudio::showWarnings(bool value)
	{
		try
		{
    		_rtaudio->showWarnings(value);
		}
        catch (::RtError &exception)
		{
			throwError(&exception);
		} // end try/catch
	} // end showWarnings
	
	/// Protected \\\
	
	// Actual constructor. This is a work around for issues using default parameters in managed C++.
	void RtAudio::initialize(RtAudio::Api api)
	{
		_rtaudio = new ::RtAudio(::RtAudio::Api(int(api)));
	} // end initialize

	// Converstion of StreamParameters^ to StreamParameters*
	::RtAudio::StreamParameters* RtAudio::convertManagedToUnmanaged(RtAudio::StreamParameters^ _params)
	{
		if (_params != nullptr)
		{
			::RtAudio::StreamParameters* params = new ::RtAudio::StreamParameters();
	
			params->deviceId = _params->deviceId;
			params->nChannels = _params->nChannels;
			params->firstChannel = _params->firstChannel;
			
			return params;
		} // end if

		return nullptr;
	} // end convertManagedToUnmanaged

	// Converstion of StreamOptions^ to StreamOptions*
	::RtAudio::StreamOptions* RtAudio::convertManagedToUnmanaged(RtAudio::StreamOptions^ _options)
	{
		if (_options != nullptr)
		{
			::RtAudio::StreamOptions* options = new ::RtAudio::StreamOptions();
	
			options->numberOfBuffers = _options->numberOfBuffers;
	
			// Convert streamName
			std::string streamName;
			
			if(_options->streamName != nullptr)
			{
				String^ name = gcnew String(_options->streamName);
				streamName = marshal_as<std::string>(name);
			} // end if
	
			options->streamName = streamName;
			
			options->priority = _options->priority;
	
			return options;
		} // end if

		return nullptr;
	} // end convertManagedToUnmanaged

	//////////////////////////////////////////////////////////////////////////
	/// Audio Callback

	int audioCallback(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void *userData)
	{
		// We've stored a pointer to the instance of our managed RtAudio class in the userData.
		// This will retrieve it, and cast it appropriately.
		GCHandle instHandle = GCHandle::FromIntPtr(IntPtr(userData));
		RtAudio^ instance = (RtAudio^) instHandle.Target;

		// Call the callback registered with the instance.
		instance->_callback(IntPtr(outputBuffer), IntPtr(inputBuffer), nBufferFrames, streamTime, status, instance->userData);

		return 0;
	} // end audioCallback
} // end namespace