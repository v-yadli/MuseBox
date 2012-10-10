using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace MuseBox.DSP
{
    class AudioInterfaceInput : Device
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
    class AudioInterfaceOutput : Device
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

}
