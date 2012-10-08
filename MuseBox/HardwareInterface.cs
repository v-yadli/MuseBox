using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using RtAudioNet;

namespace MuseBox
{
    static class HardwareInterface
    {
        /**
        typedef int(* RtAudioCallback)(void *outputBuffer, void *inputBuffer, unsigned int nFrames, double streamTime, RtAudioStreamStatus status, void *userData)
        RtAudio callback function prototype.

        All RtAudio clients must create a function of type RtAudioCallback to read and/or write data from/to the audio stream. When the underlying audio system is ready for new input or output data, this function will be invoked.

        Parameters:
                outputBuffer 	For output (or duplex) streams, the client should write nFrames of audio sample frames into this buffer. This argument should be recast to the datatype specified when the stream was opened. For input-only streams, this argument will be NULL.
                inputBuffer 	For input (or duplex) streams, this buffer will hold nFrames of input audio sample frames. This argument should be recast to the datatype specified when the stream was opened. For output-only streams, this argument will be NULL.
                nFrames 	The number of sample frames of input or output data in the buffers. The actual buffer size in bytes is dependent on the data type and number of channels in use.
                streamTime 	The number of seconds that have elapsed since the stream was started.
                status 	If non-zero, this argument indicates a data overflow or underflow condition for the stream. The particular condition can be determined by comparison with the RtAudioStreamStatus flags.
                userData 	A pointer to optional data provided by the client when opening the stream (default = NULL).

        To continue normal stream operation, the RtAudioCallback function should return a value of zero. To stop the stream and drain the output buffer, the function should return a value of one. To abort the stream immediately, the client should return a value of two.
        */
        //public delegate int RtAudioNetCallback(IntPtr value, IntPtr value, uint value, double value, uint value, object value);
        public static unsafe int RtAudioCallback(IntPtr outputBuffer, IntPtr inputBuffer, uint nFrames, double streamTime, uint status, object value)
        {
            void* outputPtr = outputBuffer.ToPointer();
            void* inputPtr = inputBuffer.ToPointer();
            return 0;
        }
        public static unsafe void StartAudio()
        {
            RtAudioInstance.openStream(
                new RtAudio.StreamParameters()
                {
                    deviceId = 0,
                    firstChannel = 0,
                    nChannels = 2
                },
                new RtAudio.StreamParameters()
                {
                    deviceId = 0,
                    firstChannel = 0,
                    nChannels = 2
                },
                RtAudioFormat.RTAUDIO_FLOAT32,
                44100,
                512,
                new RtAudioNetCallback(HardwareInterface.RtAudioCallback)
                );
            RtAudioInstance.startStream();
        }
        public static unsafe void StopAudio()
        {
            RtAudioInstance.stopStream();
            RtAudioInstance.closeStream();
        }
        public static unsafe void ResetAudio()
        {
            RtAudioInstance.stopStream();
            RtAudioInstance.startStream();
        }
        public static int TimeStamp = 0;

        private static int SampleRate = 44100;
        private static RtAudioNet.RtAudio RtAudioInstance = new RtAudioNet.RtAudio(RtAudio.Api.WINDOWS_ASIO);
    }
}
