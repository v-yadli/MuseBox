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

#include "RtMixer.h"

using namespace RtStream;

//////////////////////////////////////////////////////////////////////////
/// RtMixerInput Class
#pragma region RtMixerInput

// Constructor	
RtMixerInput::RtMixerInput()
{
	Gain = 0.0;
	Pan = 0.0;
	InputStream = nullptr;
} // end RtMixerInput

// Constructor	
RtMixerInput::RtMixerInput(RtInputStream^ stream)
{
	Gain = 1.0;
	Pan = 0.0;
	InputStream = stream;
} // end RtMixerInput

// Constructor	
RtMixerInput::RtMixerInput(RtInputStream^ stream, float gain, float pan)
{
	Gain = gain;
	Pan = pan;
	InputStream = stream;
} // end RtMixerInput
#pragma endregion

//////////////////////////////////////////////////////////////////////////
/// RtStreamMixer Class
#pragma region RtStreamMixer

// Default Constructor
RtStreamMixer::RtStreamMixer()
{
	logger = EventLoggerManager::getLogger("RtStreamMixer");

	logger->Trace("RtStreamMixer() called.");

	logger->Trace("Creating new Format instance.");
	Format = gcnew RtStreamFormat();
	Format->type = ::RtAudioNet::RtAudioFormat::RTAUDIO_FLOAT32;
	Format->sampleRate = 11025;
	Format->channels = 2;
	Format->bitsPerSample = 16;

	logger->Trace("Setting FramesToBuffer to 1, and OutputGain to 1.");
	FramesToBuffer = 1;
	OutputGain = 1;

	// Internal buffer
	logger->Debug("Creating new internal buffer of size: {0}", 1024);
	internalBuffer = gcnew CircularBuffer<float>(1024, true);

	running = false;

	// All our internal inputStreams
	logger->Trace("Initializing internal device dictionary.");
	inputs = gcnew Dictionary<String^, RtMixerInput^>();
} // end RtStreamMixer

// Default Destructor
RtStreamMixer::~RtStreamMixer()
{
	logger->Trace("~RtStreamMixer() called.");

	logger->Debug("In destructor. Stopping mixer.");
	Stop();

	for each(KeyValuePair<String^, RtMixerInput^>^ kvp in inputs)
	{
		logger->Trace("Telling inputstream to finish.");
		kvp->Value->InputStream->Finish();
	} // end for

	logger->Trace("Telling outputstream to finish.");
	outputStream->Finish();
} // end ~RtStreamMixer

// Add an input stream to the mixer
void RtStreamMixer::AddInputStream(RtInputStream^ inputStream)
{
	logger->Trace("AddInputStream(RtInputStream^ inputStream) called.");

	if (inputStream->Name == "")
	{
		logger->Error("Adding InputStreams without names is unsupported! Ignoring stream.");
		return;
	} // end if

	// Close the stream if it's open
	if (inputStream->IsStreamOpen())
	{
		logger->Warn("Attempting to add open InputStream! Closing.");
		inputStream->Close();
	} // end if

	// Reset the format to our required format and then reopen the stream
	logger->Trace("Resetting InputStream format to mixer's format");
	inputStream->Format->type = Format->type;
	inputStream->Format->sampleRate = Format->sampleRate;

	if (inputStream->Format->channels > 2)
	{
		logger->Warn("Attempted to add InputStream with more than 2 channels. Resetting to 2.");
		inputStream->Format->channels = 2;
	} // end if

	logger->Trace("Opening InputStream.");
	inputStream->Open();

	// Add the string to our internal list
	logger->Trace("Adding InputStream to internal input list.");
	inputs[inputStream->Name] = gcnew RtMixerInput(inputStream);
} // end AddInputStream

void RtStreamMixer::AddInputStream(RtInputStream^ inputStream, float gain, float pan)
{
	logger->Trace("AddInputStream(RtInputStream^ inputStream, float gain, float pan) called.");
	if (inputStream->Name == "")
	{
		logger->Error("Adding InputStreams without names is unsupported! Ignoring stream.");
		return;
	} // end if

	// Close the stream if it's open
	if (inputStream->IsStreamOpen())
	{
		logger->Warn("Attempting to add open InputStream! Closing.");
		inputStream->Close();
	} // end if

	// Reset the format to our required format and then reopen the stream
	logger->Trace("Resetting InputStream format to mixer's format");
	inputStream->Format->type = Format->type;
	inputStream->Format->sampleRate = Format->sampleRate;

	if (inputStream->Format->channels > 2)
	{
		logger->Warn("Attempted to add InputStream with more than 2 channels. Resetting to 2.");
		inputStream->Format->channels = 2;
	} // end if

	logger->Trace("Opening InputStream.");
	inputStream->Open();

	// Add the string to our internal list
	logger->Trace("Adding InputStream to internal input list with gain: {0}, pan: {1}.", gain, pan);
	inputs[inputStream->Name] = gcnew RtMixerInput(inputStream, gain, pan);
} // end AddInputStream

// Add an input stream to the mixer
void RtStreamMixer::AddInputStream(RtMixerInput^ input)
{
	if (input->InputStream->Name == "")
	{
		logger->Error("Adding InputStreams without names is unsupported! Ignoring stream.");
		return;
	} // end if

	// Close the stream if it's open
	if (input->InputStream->IsStreamOpen())
	{
		logger->Warn("Attempting to add open InputStream! Closing.");
		input->InputStream->Close();
	} // end if

	// Reset the format to our required format and then reopen the stream
	logger->Trace("Resetting InputStream format to mixer's format");
	input->InputStream->Format->type = Format->type;
	input->InputStream->Format->sampleRate = Format->sampleRate;

	if (input->InputStream->Format->channels > 2)
	{
		logger->Warn("Attempted to add InputStream with more than 2 channels. Resetting to 2.");
		input->InputStream->Format->channels = 2;
	} // end if

	logger->Trace("Opening InputStream.");
	input->InputStream->Open();

	// Add the string to our internal list
	logger->Trace("Adding InputStream to internal input list.");
	inputs[input->InputStream->Name] = input;
} // end AddInputStream

// Add an outputstream to the mixer
void RtStreamMixer::SetOutputStream(RtOutputStream^ outputStream)
{
	logger->Trace("SetOutputStream(RtOutputStream^ outputStream) called.");

	// Close the stream if it's open
	if (outputStream->IsStreamOpen())
	{
		logger->Warn("Attempting to add open OutputStream! Closing.");
		outputStream->Close();
	} // end if

	// Reset the format to our required format and then reopen the stream
	logger->Trace("Resetting OutputStream format to mixer's format");
	outputStream->Format->type = Format->type;
	outputStream->Format->sampleRate = Format->sampleRate;

	if (outputStream->Format->channels > 2)
	{
		logger->Warn("Attempted to add OutputStream with more than 2 channels. Resetting to 2.");
		outputStream->Format->channels = 2;
	} // end if

	logger->Trace("Opening OutputStream.");
	outputStream->Open();

	// Set out outputStream
	logger->Trace("Setting OutputStream.");
	this->outputStream = outputStream;

	// Set our eventhandler
	logger->Trace("Setting OutputStream->CallbackFired handler.");
	this->outputStream->CallbackFired += gcnew EventHandler(this, &RtStreamMixer::callbackHandler);
} // end SetOutputStream

// Adjust the gain on the selected input
void RtStreamMixer::AdjustGain(String^ inputName, float gain)
{
	logger->Trace("AdjustGain(String^ inputName, float gain) called.");

	logger->Debug("Setting Gain to {0}", gain);
	inputs[inputName]->Gain = gain;
} // end AdjustGaim

// Adjust the pan on the selected input
void RtStreamMixer::AdjustPan(String^ inputName, float pan)
{
	logger->Trace("AdjustPan(String^ inputName, float pan) called.");

	logger->Debug("Setting Pan to {0}", pan);
	inputs[inputName]->Pan = pan;
} // end AdjustPan

// Start the mixer
void RtStreamMixer::Start()
{
	logger->Trace("Start() called.");

	for each(KeyValuePair<String^, RtMixerInput^>^ kvp in inputs)
	{
		logger->Trace("Calling InputStream(\"{0}\")->Start()", kvp->Key);
		kvp->Value->InputStream->Start();
	} // end for

	logger->Trace("Calling OutputStream->Start().");
	outputStream->Start();
} // end Start

// Stop the mixer
void RtStreamMixer::Stop()
{
	logger->Trace("Stop() called.");

	for each(KeyValuePair<String^, RtMixerInput^>^ kvp in inputs)
	{
		logger->Trace("Calling InputStream(\"{0}\")->Stop()", kvp->Key);
		kvp->Value->InputStream->Stop();
	} // end for

	logger->Trace("Calling OutputStream->Stop().");
	outputStream->Stop();
} // end Stop

bool RtStreamMixer::IsRunning()
{
	logger->Trace("IsRunning() called. Returning {0}", running);
	return running;
} // end IsRunning

// Callback Event Handler
// XXX: No logging in the callback for performance reasons. Not actually sure if it will make a difference, but for now, I'm leaving it out.
// For reference: http://stackoverflow.com/questions/3847727/performance-implications-of-net-events
void RtStreamMixer::callbackHandler(Object^ sender, EventArgs^ e)
{
	array<float>^ tempBuff = gcnew array<float>(outputStream->Frames * outputStream->Format->channels);

	for each(KeyValuePair<String^, RtMixerInput^>^ kvp in inputs)
	{
		RtInputStream^ inputStream = kvp->Value->InputStream;
		int toRead = outputStream->Frames * inputStream->Format->channels;
		array<float>^ inputBuff = gcnew array<float>(toRead);
		int floatsRead = inputStream->Read(inputBuff);
		float gain = kvp->Value->Gain;
		float leftGain = gain;
		float rightGain = gain;

		if (toRead != floatsRead)
		{
			BufferUnderrunEventArgs^ args = gcnew BufferUnderrunEventArgs();
			args->ExpectedBufferSize = toRead;
			args->ActualBufferSize = floatsRead;

			BufferUnderrun(this, args);
		} // end if

		// Currently, we only support pan on 2 channel inputs
		if (inputStream->Format->channels == 2)
		{
			leftGain = Convert::ToSingle(Math::Cos((kvp->Value->Pan + 1) * Math::PI / 4) * kvp->Value->Gain);
			rightGain = Convert::ToSingle(Math::Cos((kvp->Value->Pan - 1) * Math::PI / 4) * kvp->Value->Gain);
			//Console::WriteLine("Values are: {0}, {1}, {2}, {3}", leftGain, rightGain, kvp->Value->Pan, kvp->Value->Gain);
		} // end if

		for(int idx = 0; idx < floatsRead; idx++)
		{
			if (inputStream->Format->channels == 1)
			{
				tempBuff[idx*2] += (inputBuff[idx] * leftGain);
				tempBuff[idx*2+1] += (inputBuff[idx] * rightGain);
			}
			else
			{
				// This code assumes interleaved audio! 
				if (idx % 2 == 0)
				{
					gain = leftGain;
				}
				else
				{
					gain = rightGain;
				} // end if

				// TODO: This only works for 2 channels; add support for channels > 2.
				tempBuff[idx] += (inputBuff[idx] * gain);
			} // end if
		} // end for
	} // end for

	// Optimixation; skip this loop if our gain is 1.
	if (OutputGain != 1)
	{
		for(int idx = 0; idx < tempBuff->Length; idx++)
		{
			tempBuff[idx] = tempBuff[idx] * OutputGain;
		} // end for
	} // end if

	outputStream->Write(tempBuff);
	//internalBuffer->Put(tempBuff, 0, tempBuff->Length);

	// Should we be buffering, or should we be outputting?
	//if(running || framesBuffered >= FramesToBuffer)	
	//{
	//	if (!running && framesBuffered >= FramesToBuffer)	
	//	{
	//		running = true;
	//	} // end if

	//	internalBuffer->Get(tempBuff, 0, tempBuff->Length);
	//	outputStream->Write(tempBuff);
		//Console::WriteLine("Do Stuff!");
	//}
	//else
	//{
	//	framesBuffered += 1;
	//} // end if
} // end callbackHandler
#pragma endregion


//////////////////////////////////////////////////////////////////////////
/// RtDuplexMixer Class
#pragma region RtDuplexMixer
// Default Constructor
RtDuplexMixer::RtDuplexMixer()
{
	logger = EventLoggerManager::getLogger("RtDuplexMixer");
	logger->Trace("RtDuplexMixer() called.");

	logger->Trace("Creating new Format instance.");
	Format = gcnew RtStreamFormat();
	Format->type = ::RtAudioNet::RtAudioFormat::RTAUDIO_FLOAT32;
	Format->sampleRate = 11025;
	Format->channels = 2;
	Format->bitsPerSample = 32;

	logger->Trace("Setting FramesToBuffer to 1, and OutputGain to 1.");
	FramesToBuffer = 1;
	OutputGain = 1;

	running = false;

	logger->Trace("Creating internal DuplexStream.");
	duplexStream = gcnew RtDuplexStream(512);
} // end RtDuplexMixer

// Default Destructor
RtDuplexMixer::~RtDuplexMixer()
{
	logger->Trace("~RtDuplexMixer() called.");

	logger->Debug("In destructor. Stopping mixer.");
	Stop();

	logger->Trace("Telling DuplexStream to finish.");
	duplexStream->Finish();
} // end ~RtDuplexMixer

// Add an input stream to the mixer
void RtDuplexMixer::AddInputStream(RtInputStream^ inputStream)
{
	logger->Trace("AddInputStream(RtInputStream^ inputStream) called.");

	logger->Debug("Can't add an InputStream. Instead, setting DuplexStream's input DeviceID to {0}.", inputStream->DeviceID);
	duplexStream->selectInputDevice(inputStream->DeviceID);

	logger->Trace("Setting DuplexStream Format.");
	duplexStream->Format = Format;
} // end AddInputStream

// Add an input stream to the mixer
void RtDuplexMixer::AddInputStream(RtInputStream^ inputStream, float gain, float pan)
{
	logger->Trace("AddInputStream(RtInputStream^ inputStream, float gain, float pan) called.");

	logger->Debug("Can't add an InputStream. Instead, setting DuplexStream's input DeviceID to {0}.", inputStream->DeviceID);
	duplexStream->selectInputDevice(inputStream->DeviceID);

	logger->Trace("Setting DuplexStream Format.");
	duplexStream->Format = Format;
} // end AddInputStream

// Add an input stream to the mixer
void RtDuplexMixer::AddInputStream(RtMixerInput^ input)
{
	logger->Trace("AddInputStream(RtMixerInput^ input) called.");

	logger->Debug("Can't add an InputStream. Instead, setting DuplexStream's input DeviceID to {0}.", input->InputStream->DeviceID);
	duplexStream->selectInputDevice(input->InputStream->DeviceID);

	logger->Trace("Setting DuplexStream Format.");
	duplexStream->Format = Format;
} // AddInputStream

// Add an outputstream to the mixer
void RtDuplexMixer::SetOutputStream(RtOutputStream^ outputStream)
{
	logger->Trace("SetOutputStream(RtOutputStream^ outputStream) called.");

	logger->Debug("Can't add an OutputStream. Instead, setting DuplexStream's output DeviceID to {0}.", outputStream->DeviceID);
	duplexStream->selectOutputDevice(outputStream->DeviceID);

	logger->Trace("Setting DuplexStream Format.");
	duplexStream->Format = Format;
} // SetOutputStream

// Start the mixer
void RtDuplexMixer::Start()
{
	logger->Trace("Start() called.");

	logger->Trace("Calling DuplexStream->Open().");
	duplexStream->Open();

	logger->Trace("Calling DuplexStream->Start().");
	duplexStream->Start();
} // end Start

// Stop the mixer
void RtDuplexMixer::Stop()
{
	logger->Trace("Stop() called.");

	logger->Trace("Calling DuplexStream->Stop().");
	duplexStream->Stop();

	logger->Trace("Sleeping 50ms.");
	System::Threading::Thread::Sleep(50);

	logger->Trace("Calling DuplexStream->Finish().");
	duplexStream->Finish();
} // end Stop

#pragma endregion