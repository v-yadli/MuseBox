using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace MuseBox.DSP
{
    class Delay : Device
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
