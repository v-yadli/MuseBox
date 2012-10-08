using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace MuseBox
{
    abstract class DSP
    {
        protected void InstallDependency(DSP target)
        {
        }
        protected void RemoveDependency(DSP target)
        {

        }
        List<DSP> DependencyList = new List<DSP>();
        static List<List<DSP>> DependencyMatrix = new List<List<DSP>>();
    }
}
