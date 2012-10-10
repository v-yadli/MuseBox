using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using RtAudioNet;

namespace MuseBox
{
    static class Hardware
    {
        #region RtAudio routines
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
            float* outputPtr = (float*)outputBuffer.ToPointer();
            float* inputPtr = (float*)inputBuffer.ToPointer();

            for (uint i = 0; i != nFrames; ++i)
            {
                for (int j = 0; j < inputChannelCount; ++j)
                    AudioInput.WriteOutput(j, *(inputPtr++));
                Update();
                for (int j = 0; j < outputChannelCount; ++j)
                    *(outputPtr++) = AudioOutput.ReadInput(j);
            }

            return 0;
        }
        public static unsafe void StartAudio()
        {
            RtAudioInstance.openStream(
                new RtAudio.StreamParameters()
                {
                    deviceId = RtAudioDeviceID,
                    firstChannel = RtAudioInputChannelOffset,
                    nChannels = inputChannelCount
                },
                new RtAudio.StreamParameters()
                {
                    deviceId = RtAudioDeviceID,
                    firstChannel = RtAudioOutputChannelOffset,
                    nChannels = outputChannelCount
                },
                RtAudioFormat.RTAUDIO_FLOAT32,
                sampleRate,
                bufferLength,
                new RtAudioNetCallback(Hardware.RtAudioCallback)
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
        public static unsafe void Test()
        {
            Console.WriteLine("Compiled APIs:");
            foreach (var i in RtAudio.getCompiledApi())
                Console.WriteLine(i);
            var deviceCount = RtAudioInstance.getDeviceCount();
            Console.WriteLine("Device count = {0}", deviceCount);
            for (uint i = 0; i < deviceCount; ++i)
            {
                var deviceInfo = RtAudioInstance.getDeviceInfo(i);
                Console.WriteLine("\t" + "deviceInfo.duplexChannels:\t" + deviceInfo.duplexChannels);
                Console.WriteLine("\t" + "deviceInfo.inputChannels:\t" + deviceInfo.inputChannels);
                Console.WriteLine("\t" + "deviceInfo.isDefaultInput:\t" + deviceInfo.isDefaultInput);
                Console.WriteLine("\t" + "deviceInfo.isDefaultOutput:\t" + deviceInfo.isDefaultOutput);
                Console.WriteLine("\t" + "deviceInfo.name:\t" + deviceInfo.name);
                Console.WriteLine("\t" + "deviceInfo.nativeFormats:\t" + deviceInfo.nativeFormats);
                Console.WriteLine("\t" + "deviceInfo.outputChannels:\t" + deviceInfo.outputChannels);
                Console.WriteLine("\t" + "deviceInfo.probed:\t" + deviceInfo.probed);
                Console.WriteLine("\t" + "deviceInfo.sampleRates:\t" + string.Join(";", deviceInfo.sampleRates.Select(x => x.ToString())));
            }
        }
        #endregion//RtAudio
        #region DSPManager routines
        public static bool InstallDependency(DSP source, DSP target)
        {
            lock (locker)
            {
                var t = Tuple.Create(source, target);
                DependencyList.Add(t);
                UpdateComputationSequence();
                return true;
            }
        }
        public static bool RemoveDependency(DSP source, DSP target)
        {
            lock (locker)
            {
                var t = Tuple.Create(source, target);
                DependencyList.Remove(t);
                UpdateComputationSequence();
                return true;
            }
        }
        public static bool InstallDSP(DSP dsp)
        {
            lock (locker)
            {
                DeviceList.Add(dsp);
                return true;
            }
        }
        public static bool RemoveDSP(DSP dsp)
        {
            lock (locker)
            {
                DeviceList.Remove(dsp);
                for (int i = 0; i < DependencyList.Count; )
                {
                    if (DependencyList[i].Item1 == dsp || DependencyList[i].Item2 == dsp)
                    {
                        if (dsp == DependencyList[i].Item2)
                        {
                            DependencyList[i].Item1.OnInputDeviceRemoved(dsp);
                        }
                        DependencyList.RemoveAt(i);
                        continue;
                    }
                    ++i;
                }
                UpdateComputationSequence();
                return true;
            }
        }
        //Caller should hold locker
        private static void UpdateComputationSequence()
        {
            Dictionary<DSP, int> dspMap = new Dictionary<DSP, int>();
            for (int i = 0; i < DeviceList.Count; ++i)
                dspMap[DeviceList[i]] = i;
            bool[,] graph = new bool[DeviceList.Count, DeviceList.Count];
            bool[] visited = new bool[DeviceList.Count];
            List<DSP> computedSequence = new List<DSP>();
            foreach (var dep in DependencyList)
            {
                //Item1 depends on item2
                graph[dspMap[dep.Item1], dspMap[dep.Item2]] = true;
            }
            #region DFS closure
            Action<int> DFS = null;
            DFS =
                (node) =>
                {
                    visited[node] = true;
                    for (int i = 0; i < visited.Length; ++i)
                    {
                        if (graph[node, i] && !visited[i])
                            DFS(i);
                    }
                    computedSequence.Add(DeviceList[node]);
                };
            #endregion
            for (int i = 0; i < DeviceList.Count; ++i)
            {
                if (!visited[i])
                {
                    DFS(i);
                }
            }
            DeviceList = computedSequence;
        }
        public static unsafe void Update()
        {
            lock (locker)
            {
                foreach (var dsp in DeviceList)
                    dsp.Update();
                ++TimeStamp;
            }
        }
        #endregion

        static Hardware()
        {
            AudioInput = new AudioInterfaceInput((int)inputChannelCount);
            AudioOutput = new AudioInterfaceOutput((int)outputChannelCount);
            InstallDSP(AudioInput);
            InstallDSP(AudioOutput);
        }

        public static AudioInterfaceInput AudioInput;
        public static AudioInterfaceOutput AudioOutput;

        private static List<DSP> DeviceList = new List<DSP>();
        private static List<Tuple<DSP, DSP>> DependencyList = new List<Tuple<DSP, DSP>>();
        private static int nextDspId = 0;
        private static object locker = new object();
        /// <summary>
        /// Jiffy : The length of a sample, measured in milliseconds
        /// </summary>
        public static double Jiffy { get { return 1000.0 / sampleRate; } }
        public static int TimeStamp = 0;
        /// <summary>
        /// CurrentTime : Query the current time, measured in milliseconds
        /// </summary>
        public static double CurrentTime { get { return Jiffy * TimeStamp; } }

        private static uint sampleRate = 44100;//TODO
        private static uint bufferLength = 512;
        private static uint inputChannelCount = 2;
        private static uint outputChannelCount = 2;

        private static uint RtAudioInputChannelOffset = 0;
        private static uint RtAudioOutputChannelOffset = 0;
        private static uint RtAudioDeviceID = 0;

        private static RtAudioNet.RtAudio RtAudioInstance = new RtAudioNet.RtAudio(RtAudio.Api.WINDOWS_ASIO);
    }
}
