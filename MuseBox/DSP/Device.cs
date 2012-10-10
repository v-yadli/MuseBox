using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace MuseBox.DSP
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
    abstract unsafe class Device
    {
        public Device(int InputChannelCount, int OutputChannelCount)
        {
            InputChannels = new EndPoint[InputChannelCount];
            OutputChannels = new EndPoint[OutputChannelCount];
            InputChannelProviders = new Device[InputChannelCount];
            for (int i = 0; i < OutputChannelCount; ++i)
            {
                OutputChannels[i] = new EndPoint();
            }
        }
        public void PlugInput(int channel, Device sourceDsp, int sourceChannel)
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

        public void OnInputDeviceRemoved(Device dsp)
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
        private Device[] InputChannelProviders;
        public abstract void Update();
    }

    


}
