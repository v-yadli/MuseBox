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
            HardwareInterface.StartAudio();
            while (true)
            {
                Thread.Sleep(1000);
            }
            HardwareInterface.StopAudio();
        }
    }
}