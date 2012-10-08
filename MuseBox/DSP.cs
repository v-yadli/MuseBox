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
    abstract unsafe class DSP
    {
        public virtual void Update();
        internal float CurrentOutput;
    }
    static class DSPManager
    {
        public static bool InstallDependency(DSP source, DSP target)
        {
            return true;
        }
        public static bool RemoveDependency(DSP source, DSP target)
        {
            return true;
        }
        private static long dspSeqNum = 0;
    }
}
