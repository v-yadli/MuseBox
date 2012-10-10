using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace MuseBox
{
    /// <summary>
    /// In a computer, everything is digital signal.
    /// In MuseBox, we'll treat everything, from audio signals, to
    /// control signals and MIDI notes as digital signal.
    /// </summary>
    class EndPoint
    {
        public float Data;
    }
    abstract unsafe class DSP
    {
        public DSP(int InputChannelCount, int OutputChannelCount)
        {
            InputChannels = new EndPoint[InputChannelCount];
            OutputChannels = new EndPoint[OutputChannelCount];
            InputChannelProviders = new DSP[InputChannelCount];
            for (int i = 0; i < OutputChannelCount; ++i)
            {
                OutputChannels[i] = new EndPoint();
            }
        }
        public void PlugInput(int channel, DSP sourceDsp, int sourceChannel)
        {
            InputChannels[channel] = sourceDsp.OutputChannels[sourceChannel];
            InputChannelProviders[channel] = sourceDsp;
            Hardware.InstallDependency(this, sourceDsp);
        }
        public void UnplugInput(int channel)
        {
            Hardware.RemoveDependency(InputChannelProviders[channel], this);
            InputChannels[channel] = null;
            InputChannelProviders[channel] = null;
        }

        public void OnInputDeviceRemoved(DSP dsp)
        {
            for (int i = 0; i < InputChannels.Length; ++i)
                if (InputChannelProviders[i] == dsp)
                {
                    InputChannelProviders[i] = null;
                    InputChannels[i] = null;
                }
        }

        public float ReadInput(int channel)
        {
            if (InputChannels[channel] != null)
                return InputChannels[channel].Data;
            else return 0;
        }
        public void WriteOutput(int channel, float value)
        {
            OutputChannels[channel].Data = value;
        }
        private EndPoint[] InputChannels;
        private EndPoint[] OutputChannels;
        private DSP[] InputChannelProviders;
        public abstract void Update();
    }
    class AudioInterfaceInput : DSP
    {
        public AudioInterfaceInput(int channelCount)
            : base(0, channelCount) // Input device output data
        {
        }
        public override void Update()
        {
            //Nothing, and won't execute
        }
    }
    class AudioInterfaceOutput : DSP
    {
        public AudioInterfaceOutput(int channelCount)
            : base(channelCount, 0) // Output device eat data
        {
        }
        public override void Update()
        {
            //Nothing, and won't execute
        }
    }

    class Sampler : DSP
    {
        public Sampler(int channelCount)
            : base(channelCount, channelCount)
        {
            ChannelCount = channelCount;
        }
        #region Fields
        readonly int ChannelCount;
        float[,] Buffer = null;
        int currentBufferLength = 0;
        int currentPlaying = 0;
        int currentRecording = 0;
        bool playing = false;
        bool recording = false;
        int waitingForRecord = 0;
        int waitingForPlay = 0;
        bool playingLoop = true;
        #endregion
        public void SetBufferLength(int targetLength)
        {
            Buffer = new float[ChannelCount, targetLength];
            currentBufferLength = targetLength;
        }
        public void StartRecord(int wait = 0)
        {
            waitingForRecord = wait;
            currentRecording = 0;
            recording = true;
        }
        public void StopRecord()
        {
            recording = false;
        }
        public void StartPlay(int wait = 0)
        {
            waitingForPlay = wait;
            currentPlaying = 0;
            playing = true;
        }
        public void StopPlay()
        {
            playing = false;
        }
        public override void Update()
        {
            if (Buffer == null)
                return;
            if (playing)
            {
                if (waitingForPlay != 0)
                {
                    --waitingForPlay;
                }
                else
                {
                    for (int i = 0; i < ChannelCount; ++i)
                    {
                        WriteOutput(i, Buffer[i, currentPlaying]);
                    }
                    ++currentPlaying;
                    if (currentPlaying >= currentBufferLength)
                    {
                        Console.WriteLine("Looping...");
                        currentPlaying = 0;
                        if (!playingLoop)
                            playing = false;
                    }
                }
            }
            else
            {
                for (int i = 0; i < ChannelCount; ++i)
                    WriteOutput(i, .0F);
            }
            if (recording)
            {
                if (waitingForRecord != 0)
                {
                    --waitingForRecord;
                }
                else
                {
                    for (int i = 0; i < ChannelCount; ++i)
                    {
                        Buffer[i, currentRecording] = ReadInput(i);
                    }
                    ++currentRecording;
                    if (currentRecording >= currentBufferLength)
                    {
                        currentRecording = 0;
                        recording = false;
                    }
                }
            }
        }
    }

    class Delay : DSP
    {
        public Delay()
            : base(1, 1)
        {
        }
        public override void Update()
        {
            float input = ReadInput(0);
            float delay = delayBuffer[(currentCursor + 1) % delayBuffer.Length];
            float output = input * DryRatio + delay * (1.0F - DryRatio);
            delayBuffer[currentCursor] = output * Damping;
            WriteOutput(0, output);
            currentCursor = (currentCursor + 1) % delayBuffer.Length;
        }
        private float[] delayBuffer = new float[20480];
        int currentCursor = 0;
        private float DryRatio = 0.5F;
        private float Damping = 0.7F;
    }
}
