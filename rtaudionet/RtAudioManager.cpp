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

#include "RtAudioManager.h"


namespace RtStream
{
	//////////////////////////////////////////////////////////////////////////
	/// RtAudioManager

	// Default Constructor
	RtAudioManager::RtAudioManager()
	{
		_api = ::RtAudioNet::RtAudio::Api::UNSPECIFIED;

		// Finish initialization
		initialize();
	} // end RtAudioManager

	// Default Constructor
	RtAudioManager::RtAudioManager(::RtAudioNet::RtAudio::Api api)
	{
		_api = api;

		// Finish initialization
		initialize();
	} // end RtAudioManager

	// Default Constructor
	RtAudioManager::~RtAudioManager()
	{
		logger->Trace("~RtAudioManager called.");

		if (enumerateTimer->Enabled)
		{
			logger->Trace("Stopping enumerateTimer.");
			enumerateTimer->Stop();
		} // end if
	} // end ~RtAudioManager

	void RtAudioManager::initialize()
	{
		enumerateTimer = nullptr;

		// Initialize the logger
		logger = EventLoggerManager::getLogger("RtAudioManager");
		logger->Trace("Starting initialize.");

		// Initialize RtAudio
		logger->Debug("Initializing internal RtAudioNet.");
		rtaudio = gcnew ::RtAudioNet::RtAudio(_api);

		// Initialize device dictionaries
		logger->Trace("Initializing internal device dictionaries.");
		InputDevices = gcnew Dictionary<String^, int>();
		OutputDevices = gcnew Dictionary<String^, int>();

		// Populate device dictionaries
		logger->Trace("Calling EnumerateDevices.");
		EnumerateDevices();

		logger->Debug("Finished initialize.");
	} // end initialize

	// Start a timer to watch for changes in the number of devices
	void RtAudioManager::enableDeviceWatcher(int interval)
	{
		// Reenumerate the devices every 'interval' milliseconds
		if(enumerateTimer == nullptr)
		{
			logger->Trace("Creating enumerateTimer.");
			enumerateTimer = gcnew System::Timers::Timer();
			enumerateTimer->Elapsed += gcnew ElapsedEventHandler(this, &RtAudioManager::OnEnumerateTimer);
		} // end if

		logger->Trace("Setting enumerateTimer interval.");
		if(enumerateTimer->Enabled)
		{
			enumerateTimer->Stop();
		} // end if
		enumerateTimer->Interval = interval;

		logger->Trace("Starting enumerateTimer.");
		enumerateTimer->Start();
	} // end enableDeviceWatcher

	// Finds the input device id by name
	int RtAudioManager::FindInputDeviceByName(String^ name)
	{
		logger->Trace("FindInputDeviceByName(String^ name) called.");

		int devID = -1;
		for each(KeyValuePair<String^, int>^ kvp in InputDevices)
		{
			// Case insensitive search
			logger->Trace("Searching \"{0}\" for match with \"{1}\".", kvp->Key, name);
			if (kvp->Key->IndexOf(name, StringComparison::OrdinalIgnoreCase) >= 0)
			{
				logger->Debug("Found \"{0}\" with id {1}", kvp->Key, kvp->Value);
				devID = kvp->Value;
			} // end if
		} // end for each

		logger->Trace("Returning \"{0}\".", devID);
		return devID;
	} // end FindInputDeviceByName

	// Finds the output device id by name
	int RtAudioManager::FindOutputDeviceByName(String^ name)
	{
		logger->Trace("FindOutputDeviceByName(String^ name) called.");

		int devID = -1;
		for each(KeyValuePair<String^, int>^ kvp in OutputDevices)
		{
			logger->Trace("Searching \"{0}\" for match with \"{1}\".", kvp->Key, name);
			if (kvp->Key->Contains(name))
			{
				logger->Debug("Found \"{0}\" with id {1}", kvp->Key, kvp->Value);
				devID = kvp->Value;
			} // end if
		} // end for each

		logger->Trace("Returning \"{0}\".", devID);
		return devID;
	} // end FindOutputDeviceByName

	// Gets the device information for the given device id
	::RtAudioNet::RtAudio::DeviceInfo^ RtAudioManager::GetDeviceInfo(int devID)
	{
		logger->Trace("GetDeviceInfo(int devID) called.");
		return rtaudio->getDeviceInfo((unsigned int) devID);
	} // end GetDeviceInfo

	// Creates and returns a mixer based on the inputs and outputs
	RtStreamMixer^ RtAudioManager::CreateMixer(List<int>^ inputs, int output)
	{
		return CreateMixer(inputs, output, true);
	} // end CreateMixer

	// Creates and returns a mixer based on the inputs and outputs
	RtStreamMixer^ RtAudioManager::CreateMixer(List<int>^ inputs, int output, bool useDuplex)
	{
		logger->Trace("CreateMixer(List<int>^ inputs, int output) called.");

		// See how many inputs we have. If we only have a single input (or no inputs), then it's much more performant to use
		// a RtDuplexMixer. This greatly improves performance over using the standard RtStreamMixer class.
		RtStreamMixer^ mixer = nullptr;
		if (inputs->Count < 2 && useDuplex)
		{
			logger->Debug("Creating RtDuplexMixer.");
			mixer = (RtStreamMixer^) gcnew RtDuplexMixer();
		}
		else
		{
			logger->Debug("Creating RtStreamMixer.");
			mixer = gcnew RtStreamMixer();
		} // end if

		logger->Trace("Calling _createMixer with: inputs:{0}, output:{1}, mixer:{2}", inputs, output, mixer);
		return _createMixer(inputs, output, mixer);
	} // end CreateMixer

	// Creates and returns a mixer based on the inputs and outputs
	RtStreamMixer^ RtAudioManager::CreateMixer(List<String^>^ inputs, String^ output)
	{
		return CreateMixer(inputs, output, true);
	} // end CreateMixer

	// Creates and returns a mixer based on the inputs and outputs
	RtStreamMixer^ RtAudioManager::CreateMixer(List<String^>^ inputs, String^ output, bool useDuplex)
	{
		logger->Trace("CreateMixer(List<String^>^ inputs, String^ output) called.");

		// See how many inputs we have. If we only have a single input (or no inputs), then it's much more performant to use
		// a RtDuplexMixer. This greatly improves performance over using the standard RtStreamMixer class.
		RtStreamMixer^ mixer = nullptr;
		if (inputs->Count < 2 && useDuplex)
		{
			logger->Debug("Creating RtDuplexMixer.");
			mixer = (RtStreamMixer^) gcnew RtDuplexMixer();
		}
		else
		{
			logger->Debug("Creating RtStreamMixer.");
			mixer = gcnew RtStreamMixer();
		} // end if

		logger->Trace("Calling _createMixer with: inputs:{0}, output:{1}, mixer:{2}", inputs, output, mixer);
		return _createMixer(inputs, output, mixer);
	} // end CreateMixer

	// Creates and returns a mixer based on the inputs and outputs
	RtStreamMixer^ RtAudioManager::CreateMixer(List<Dictionary<String^, String^>^>^ inputs, int output)
	{
		return CreateMixer(inputs, output, true);
	} // end CreateMixer

	// Creates and returns a mixer based on the inputs and outputs
	RtStreamMixer^ RtAudioManager::CreateMixer(List<Dictionary<String^, String^>^>^ inputs, int output, bool useDuplex)
	{
		logger->Trace("CreateMixer(List<Dictionary<String^, String^>^>^ inputs, int output) called.");

		// See how many inputs we have. If we only have a single input (or no inputs), then it's much more performant to use
		// a RtDuplexMixer. This greatly improves performance over using the standard RtStreamMixer class.
		RtStreamMixer^ mixer = nullptr;
		if (inputs->Count < 2 && useDuplex)
		{
			logger->Debug("Creating RtDuplexMixer.");
			mixer = (RtStreamMixer^) gcnew RtDuplexMixer();
		}
		else
		{
			logger->Debug("Creating RtStreamMixer.");
			mixer = gcnew RtStreamMixer();
		} // end if

		logger->Trace("Calling _createMixer with: inputs:{0}, output:{1}, mixer:{2}", inputs, output, mixer);
		return _createMixer(inputs, output, mixer);
	} // end CreateMixer

	// Creates and returns a mixer based on the inputs and outputs
	RtStreamMixer^ RtAudioManager::CreateMixer(List<int>^ inputs, int output, int sampleRate)
	{
		return CreateMixer(inputs, output, sampleRate, true);
	} // end CreateMixer

	// Creates and returns a mixer based on the inputs and outputs
	RtStreamMixer^ RtAudioManager::CreateMixer(List<int>^ inputs, int output, int sampleRate, bool useDuplex)
	{
		logger->Trace("CreateMixer(List<int>^ inputs, int output, int sampleRate) called.");

		// See how many inputs we have. If we only have a single input (or no inputs), then it's much more performant to use
		// a RtDuplexMixer. This greatly improves performance over using the standard RtStreamMixer class.
		RtStreamMixer^ mixer = nullptr;
		if (inputs->Count < 2 && useDuplex)
		{
			logger->Debug("Creating RtDuplexMixer.");
			mixer = (RtStreamMixer^) gcnew RtDuplexMixer();
		}
		else
		{
			logger->Debug("Creating RtStreamMixer.");
			mixer = gcnew RtStreamMixer();
		} // end if
		mixer->Format->sampleRate = sampleRate;

		logger->Trace("Calling _createMixer with: inputs:{0}, output:{1}, mixer:{2}", inputs, output, mixer);
		return _createMixer(inputs, output, mixer);
	} // end CreateMixer

	// Creates and returns a mixer based on the inputs and outputs
	RtStreamMixer^ RtAudioManager::CreateMixer(List<String^>^ inputs, String^ output, int sampleRate)
	{
		return CreateMixer(inputs, output, sampleRate, true);
	} // end CreateMixer

	// Creates and returns a mixer based on the inputs and outputs
	RtStreamMixer^ RtAudioManager::CreateMixer(List<String^>^ inputs, String^ output, int sampleRate, bool useDuplex)
	{
		logger->Trace("CreateMixer(List<String^>^ inputs, String^ output, int sampleRate) called.");

		// See how many inputs we have. If we only have a single input (or no inputs), then it's much more performant to use
		// a RtDuplexMixer. This greatly improves performance over using the standard RtStreamMixer class.
		RtStreamMixer^ mixer = nullptr;
		if (inputs->Count < 2 && useDuplex)
		{
			logger->Debug("Creating RtDuplexMixer.");
			mixer = (RtStreamMixer^) gcnew RtDuplexMixer();
		}
		else
		{
			logger->Debug("Creating RtStreamMixer.");
			mixer = gcnew RtStreamMixer();
		} // end if
		mixer->Format->sampleRate = sampleRate;

		logger->Trace("Calling _createMixer with: inputs:{0}, output:{1}, mixer:{2}", inputs, output, mixer);
		return _createMixer(inputs, output, mixer);
	} // end CreateMixer

	// Returns true if the list of devices changed.
	bool RtAudioManager::EnumerateDevices()
	{
		logger->Trace("EnumerateDevices() called.");

		// Update DeviceCount
		logger->Trace("Calling rtaudio->getDeviceCount().");
		DeviceCount = rtaudio->getDeviceCount();

		logger->Trace("Initializing internal dictionaries.");
		Dictionary<String^, int>^ inputDevices = gcnew Dictionary<String^, int>();
		Dictionary<String^, int>^ outputDevices = gcnew Dictionary<String^, int>();

		// Update Device Dictionaries
		for (int idx = 0; DeviceCount > idx; idx++)
		{
			logger->Trace("Iterating over devices. DeviceCount: {0}, idx: {1}.", DeviceCount, idx);

			::RtAudioNet::RtAudio::DeviceInfo^ info = rtaudio->getDeviceInfo(idx);

			if (info->inputChannels > 0)
			{
				logger->Debug("Setting inputDevices[{0}] to {1}", info->name, idx);
				inputDevices[info->name] = idx;
			} // end if

			if (info->outputChannels > 0)
			{
				logger->Debug("Setting outputDevices[{0}] to {1}", info->name, idx);
				outputDevices[info->name] = idx;
			} // end if
		} // end for

		logger->Trace("Setting class device properties to internal values.");
		InputDevices = inputDevices;
		OutputDevices = outputDevices;

		// TODO: Actually look for equality, return true or false if changed.
		logger->Trace("Returning false; no change.");
		return false;
	} // end EnumerateDevices

	void RtAudioManager::OnEnumerateTimer(Object^ sender, ElapsedEventArgs^ args)
	{
		logger->Trace("OnEnumerateTimer(Object^ sender, ElapsedEventArgs^ args) called.");

		if(DeviceCount != rtaudio->getDeviceCount())
		{
			enumerateTimer->Stop();

			int devCount = DeviceCount;
			bool changed = EnumerateDevices();

			logger->Trace("Checking devCount: {0}, DeviceCount: {1}, changed: {2}", devCount, DeviceCount, changed);
			if ((devCount != DeviceCount) || changed)
			{
				logger->Debug("Firing DeviceEnumerationChanged!");
				DeviceEnumerationChanged(this, gcnew EventArgs());
			} // end if

			enumerateTimer->Start();
		} // end if
	} // end OnEnumerateTimer

	// Creates and returns a mixer based on the inputs and outputs
	RtStreamMixer^ RtAudioManager::_createMixer(List<int>^ inputs, int output, RtStreamMixer^ mixer)
	{
		logger->Trace("_createMixer(List<int>^ inputs, int output, RtStreamMixer^ mixer) called.");
		for each(int inputID in inputs)
		{
			logger->Trace("Iterating over inputs: {0}; on inputID: {1}.", inputs, inputID);

			RtInputStream^ inputStream = gcnew RtInputStream(512);
			logger->Debug("Setting inputStream name to \"Input {0}\".", inputID);
			inputStream->Name = String::Format("Input {0}", inputID);
			inputStream->selectInputDevice(inputID);

			// Check for more than our maximum number of channels
			if (inputStream->Format->channels > 2)
			{
				logger->Warn("Detected {0} channels. This is greater than our maximum number of channels (2). Resetting to maximum (2).",
					inputStream->Format->channels);
				inputStream->Format->channels = 2;
			}
			else
			{
				logger->Debug("Passed number of channels check: {0} channels detected.", inputStream->Format->channels);
			} // end if

			try
			{
				logger->Debug("Adding inputStream {0} with Format: channels: {1}, sampleRate: {2}, bitsPerSample: {3}.", inputStream->Name,
					inputStream->Format->channels, inputStream->Format->sampleRate, inputStream->Format->bitsPerSample);
				mixer->AddInputStream(inputStream);
			}
			catch(System::Exception^ ex)
			{
				logger->WarnEx(String::Format("Failed to add inputStream {0}.", inputStream->Name), ex);

				// TODO: Consider removing this in favor of new logging system
				RtStream::RtAudioErrorEventArgs^ eventArgs = gcnew RtStream::RtAudioErrorEventArgs();
				eventArgs->errorString->Format("Failed to add inputStream {0}.", inputStream->Name);
				ErrorOccured(this, eventArgs);
			} // end try/catch
		} // end for

		logger->Trace("Creating outputStream with device id {0}.", output);
		RtOutputStream^ outputStream = gcnew RtOutputStream(512);
		outputStream->selectOutputDevice(output);

		logger->Trace("Setting outputStream on mixer.");
		mixer->SetOutputStream(outputStream);

		logger->Trace("Returning mixer.");
		return mixer;
	} // end CreateMixer

	// Creates and returns a mixer based on the inputs and outputs
	RtStreamMixer^ RtAudioManager::_createMixer(List<String^>^ inputs, String^ output, RtStreamMixer^ mixer)
	{
		logger->Trace("_createMixer(List<String^>^ inputs, String^ output, RtStreamMixer^ mixer) called.");

		for each(String^ inputString in inputs)
		{
			logger->Trace("Iterating over inputs: {0}; on inputString: {1}.", inputs, inputString);

			int inputID = FindInputDeviceByName(inputString);
			RtInputStream^ inputStream = gcnew RtInputStream(512);
			logger->Debug("Setting inputStream name to \"{0}\".", inputString);
			inputStream->Name = inputString;
			inputStream->selectInputDevice(inputID);

			// Check for more than our maximum number of channels
			if (inputStream->Format->channels > 2)
			{
				logger->Warn(String::Format("Detected {0} channels. This is greater than our maximum number of channels (2). Resetting to maximum (2).",
					inputStream->Format->channels));
				inputStream->Format->channels = 2;
			}
			else
			{
				logger->Debug("Passed number of channels check: {0} channels detected.", inputStream->Format->channels);
			} // end if

			try
			{
				logger->Debug("Adding inputStream {0} with Format: channels: {1}, sampleRate: {2}, bitsPerSample: {3}.", inputStream->Name,
					inputStream->Format->channels, inputStream->Format->sampleRate, inputStream->Format->bitsPerSample);
				mixer->AddInputStream(inputStream);
			}
			catch(System::Exception^ ex)
			{
				logger->WarnEx(String::Format("Failed to add inputStream {0}.", inputStream->Name), ex);

				// TODO: Consider removing this in favor of new logging system
				RtStream::RtAudioErrorEventArgs^ eventArgs = gcnew RtStream::RtAudioErrorEventArgs();
				eventArgs->errorString->Format("Failed to add inputStream {0}.", inputStream->Name);
				ErrorOccured(this, eventArgs);
			} // end try/catch
		} // end for

		logger->Trace("Creating outputStream with device name {0}.", output);
		RtOutputStream^ outputStream = gcnew RtOutputStream(512);
		outputStream->selectOutputDevice(output);

		logger->Trace("Setting outputStream on mixer.");
		mixer->SetOutputStream(outputStream);

		logger->Trace("Returning mixer.");
		return mixer;
	} // end CreateMixer

	// Creates and returns a mixer based on the inputs and outputs
	RtStreamMixer^ RtAudioManager::_createMixer(List<Dictionary<String^, String^>^>^ inputs, int output, RtStreamMixer^ mixer)
	{
		logger->Trace("_createMixer(List<Dictionary<String^, String^>^>^ inputs, int output, RtStreamMixer^ mixer) called.");

		for each(Dictionary<String^, String^>^ input in inputs)
		{
			logger->Trace("Iterating over inputs: {0}; on input: {1}.", inputs, input);

			logger->Debug("Got input dictionary with id: {0}, pan: {1}, gain: {2}", input["id"], input["pan"], input["gain"]);

			RtInputStream^ inputStream = gcnew RtInputStream(512);
			logger->Debug("Setting inputStream name to \"Input {0}\".", input["id"]);

			// Look for a name for this input in the dictionary, otherwise, name it appropriately.
			if (input->ContainsKey("name"))
			{
				inputStream->Name = input["name"];
			}
			else
			{
				inputStream->Name = String::Format("Input with ID {0}", input["id"]);
			} // end try/catch

			inputStream->selectInputDevice(System::Convert::ToInt32(input["id"]));

			// Check for more than our maximum number of channels
			if (inputStream->Format->channels > 2)
			{
				logger->Warn("Detected {0} channels. This is greater than our maximum number of channels (2). Resetting to maximum (2).",
					inputStream->Format->channels);
				inputStream->Format->channels = 2;
			}
			else
			{
				logger->Debug("Passed number of channels check: {0} channels detected.", inputStream->Format->channels);
			} // end if

			try
			{
				logger->Debug("Adding inputStream {0} with Format: channels: {1}, sampleRate: {2}, bitsPerSample: {3}.", inputStream->Name,
					inputStream->Format->channels, inputStream->Format->sampleRate, inputStream->Format->bitsPerSample);
				mixer->AddInputStream(inputStream, System::Convert::ToSingle(input["gain"]), System::Convert::ToSingle(input["pan"]));
			}
			catch(System::Exception^ ex)
			{
				logger->WarnEx(String::Format("Failed to add inputStream {0}.", inputStream->Name), ex);

				// TODO: Consider removing this in favor of new logging system
				RtStream::RtAudioErrorEventArgs^ eventArgs = gcnew RtStream::RtAudioErrorEventArgs();
				eventArgs->errorString->Format("Failed to add inputStream {0}.", inputStream->Name);
				ErrorOccured(this, eventArgs);
			} // end try/catch
		} // end for

		logger->Trace("Creating outputStream with device id {0}.", output);
		RtOutputStream^ outputStream = gcnew RtOutputStream(512);
		outputStream->selectOutputDevice(output);

		logger->Trace("Setting outputStream on mixer.");
		mixer->SetOutputStream(outputStream);

		logger->Trace("Returning mixer.");
		return mixer;
	} // end CreateMixer

	//////////////////////////////////////////////////////////////////////////
	/// Static Methods

	// Simply returns the static instance, or creates a new one if it's null
	RtAudioManager^ RtAudioManager::GetInstance()
	{
		if (_manager == nullptr)
		{
			_manager = gcnew RtAudioManager();
		} // end if

		return _manager;
	} // end GetInstance

	// Attempts to create a new instance with the given api, or throws an exception if the static instance is not null
	RtAudioManager^ RtAudioManager::GetInstance(::RtAudioNet::RtAudio::Api api)
	{
		if (_manager == nullptr)
		{
			_manager = gcnew RtAudioManager(api);
		}
		else
		{
			if (_manager->GetCurrentApi() != api)
			{
				throw gcnew RtAudioManagerApiException("Tried to instantiate RtAudioManager with a different API than the current global instance!");
			} // end if
		} // end if

		return _manager;
	} // end GetInstance
} // end namespace
