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

#include "RtAudio\RtError.h"

using namespace System;

namespace RtAudioNet
{
	// Generic Exception for handling RtErrors.
	public ref class RtAudioException: ApplicationException
    {
	public:
		RtAudioException() : ApplicationException() { };

        RtAudioException(String^ message) : ApplicationException(message) { };
        
        RtAudioException(String^ message, Exception^ innerException) : ApplicationException(message, innerException) { }; 

    }; // end RtAudioException

	public ref class RtAudioNoDevicesFoundException : RtAudioException
    {
	public:
		RtAudioNoDevicesFoundException() : RtAudioException() { };

		RtAudioNoDevicesFoundException(String^ message) : RtAudioException(message) { };
        
        RtAudioNoDevicesFoundException(String^ message, Exception^ innerException) : RtAudioException(message, innerException) { }; 

    }; // end RtAudioNoDevicesFoundException

    public ref class RtAudioInvalidDeviceException : RtAudioException
    {
	public:
		RtAudioInvalidDeviceException() : RtAudioException() { };

        RtAudioInvalidDeviceException(String^ message) : RtAudioException(message) { };
        
        RtAudioInvalidDeviceException(String^ message, Exception^ innerException) : RtAudioException(message, innerException) { }; 

    }; // end RtAudioInvalidDeviceException
    
	public ref class RtAudioMemoryErrorException : RtAudioException
    {
	public:
		RtAudioMemoryErrorException() : RtAudioException() { };

        RtAudioMemoryErrorException(String^ message) : RtAudioException(message) { };
        
        RtAudioMemoryErrorException(String^ message, Exception^ innerException) : RtAudioException(message, innerException) { }; 

    }; // end RtAudioMemoryErrorException
    
	public ref class RtAudioInvalidParameterException : RtAudioException
    {
	public:
		RtAudioInvalidParameterException() : RtAudioException() { };

        RtAudioInvalidParameterException(String^ message) : RtAudioException(message) { };
        
        RtAudioInvalidParameterException(String^ message, Exception^ innerException) : RtAudioException(message, innerException) { }; 

    }; // end RtAudioInvalidParameterException
    
	public ref class RtAudioInvalidUseException : RtAudioException
    {
	public:
		RtAudioInvalidUseException() : RtAudioException() { };

        RtAudioInvalidUseException(String^ message) : RtAudioException(message) { };
        
        RtAudioInvalidUseException(String^ message, Exception^ innerException) : RtAudioException(message, innerException) { }; 

    }; // end RtAudioInvalidUseException
    
	public ref class RtAudioDriverErrorException : RtAudioException
    {
	public:
		RtAudioDriverErrorException() : RtAudioException() { };

        RtAudioDriverErrorException(String^ message) : RtAudioException(message) { };
        
        RtAudioDriverErrorException(String^ message, Exception^ innerException) : RtAudioException(message, innerException) { }; 

    }; // end RtAudioDriverErrorException
    
	public ref class RtAudioSystemErrorException : RtAudioException
    {
	public:
		RtAudioSystemErrorException() : RtAudioException() { };

        RtAudioSystemErrorException(String^ message) : RtAudioException(message) { };
        
        RtAudioSystemErrorException(String^ message, Exception^ innerException) : RtAudioException(message, innerException) { }; 

    }; // end RtAudioSystemErrorException

    public ref class RtAudioThreadErrorException : RtAudioException
    {
	public:
		RtAudioThreadErrorException() : RtAudioException() { };

        RtAudioThreadErrorException(String^ message) : RtAudioException(message) { };
        
        RtAudioThreadErrorException(String^ message, Exception^ innerException) : RtAudioException(message, innerException) { }; 

    }; // end RtAudioThreadErrorException

	// The intention is for this clase to be used as a base clase by any other class that needs to work
	// with ::RtError. When an ::RtError is returned, the class would call throwError, and the correct
	// exception will fire.
	public ref class RtError
	{
	public:
		void throwError(::RtError* error)
		{
			// Handle error types appropriately
			switch(error->getType())
			{
				case ::RtError::WARNING:
				{
					throw gcnew RtAudioException(gcnew String(error->what()));
					break;
				} // end case

				case ::RtError::DEBUG_WARNING:
				{
					throw gcnew RtAudioException(gcnew String(error->what()));
					break;
				} // end case

				case ::RtError::NO_DEVICES_FOUND:
				{
					throw gcnew RtAudioNoDevicesFoundException(gcnew String(error->what()));
					break;
				} // end case

				case ::RtError::INVALID_DEVICE:
				{
					throw gcnew RtAudioInvalidDeviceException(gcnew String(error->what()));
					break;
				} // end case

				case ::RtError::MEMORY_ERROR:
				{
					throw gcnew RtAudioMemoryErrorException(gcnew String(error->what()));
					break;
				} // end case

				case ::RtError::INVALID_PARAMETER:
				{
					throw gcnew RtAudioInvalidParameterException(gcnew String(error->what()));
					break;
				} // end case

				case ::RtError::INVALID_USE:
				{
					throw gcnew RtAudioInvalidUseException(gcnew String(error->what()));
					break;
				} // end case

				case ::RtError::DRIVER_ERROR:
				{
					throw gcnew RtAudioDriverErrorException(gcnew String(error->what()));
					break;
				}// end case

				case ::RtError::SYSTEM_ERROR:
				{
					throw gcnew RtAudioSystemErrorException(gcnew String(error->what()));
					break;
				} // end case

				case ::RtError::THREAD_ERROR:
				{
					throw gcnew RtAudioThreadErrorException(gcnew String(error->what()));
					break;
				 } // end case

				case ::RtError::UNSPECIFIED:
				default:
				{
					throw gcnew RtAudioException(gcnew String(error->what()));
					break;
				} // end case
			} // end switch
		} // end throwError
	}; // end RtError
} // end namespace