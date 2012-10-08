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
            Console.WriteLine("Compiled APIs:");
            foreach (var i in RtAudio.getCompiledApi())
                Console.WriteLine(i);
            using (RtAudio rtAudioInstance = new RtAudio(RtAudio.Api.WINDOWS_ASIO))
            {
                var deviceCount = rtAudioInstance.getDeviceCount();
                Console.WriteLine("Device count = {0}", deviceCount);
                for (uint i = 0; i < deviceCount; ++i)
                {
                    var deviceInfo = rtAudioInstance.getDeviceInfo(i);
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
                while (true)
                {
                    Thread.Sleep(1000);
                }
            }//using (RtAudio rtAudioInstance = new RtAudio(RtAudio.Api.WINDOWS_ASIO))
        }
    }
}