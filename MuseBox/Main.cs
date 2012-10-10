using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using RtAudioNet;
using System.Threading;
using MuseBox.DSP;

namespace MuseBox
{
    class MuseBoxEntryPoint
    {
        [STAThread]
        static void Main(string[] args)
        {
            Hardware.StartAudio();
            Delay l = new Delay();
            Delay r = new Delay();

            Sampler s = new Sampler(2);
            s.SetBufferLength(88200);//2s

            Hardware.InstallDevice(l);
            Hardware.InstallDevice(r);
            Hardware.InstallDevice(s);

            Hardware.AudioOutput.PlugInput(0, s, 0);
            Hardware.AudioOutput.PlugInput(1, s, 1);
            l.PlugInput(0, Hardware.AudioInput, 0);
            r.PlugInput(0, Hardware.AudioInput, 1);
            s.PlugInput(0, l, 0);
            s.PlugInput(1, r, 0);
            s.StartPlay();
            while (true)
            {
                s.StartRecord();
                Console.ReadKey();
            }
        }
    }
}