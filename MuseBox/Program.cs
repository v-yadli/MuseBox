using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using RtAudioNet;
using System.Threading;

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

            Hardware.InstallDSP(l);
            Hardware.InstallDSP(r);

            Hardware.AudioOutput.PlugInput(0, l, 0);
            Hardware.AudioOutput.PlugInput(1, r, 0);
            l.PlugInput(0, Hardware.AudioInput, 0);
            r.PlugInput(0, Hardware.AudioInput, 1);
            Console.ReadKey();
            Hardware.StopAudio();
            Console.ReadKey();
            Hardware.StartAudio();
            Console.ReadKey();
            Hardware.StopAudio();
        }
    }
}