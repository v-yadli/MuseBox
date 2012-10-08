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
        public DSP()
        {
        }
        internal readonly void* OutputBuffer;
    }
    static class DSPManager
    {
        static List<List<DSP>> DependencyMatrix = new List<List<DSP>>();
        protected void InstallDependency(DSP target)
        {
        }
        protected void RemoveDependency(DSP target)
        {
        }
    }
}
