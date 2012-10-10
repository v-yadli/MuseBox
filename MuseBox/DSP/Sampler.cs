using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace MuseBox.DSP
{
    class Sampler : Device
    {
        public Sampler(int channelCount)
            : base(channelCount, channelCount)
        {
            ChannelCount = channelCount;
        }
        #region Fields
        readonly int ChannelCount;
        float[,] Buffer = null;
        int currentBufferLength = 0;
        int currentPlaying = 0;
        int currentRecording = 0;
        bool playing = false;
        bool recording = false;
        int waitingForRecord = 0;
        int waitingForPlay = 0;
        bool playingLoop = true;
        #endregion
        public void SetBufferLength(int targetLength)
        {
            Buffer = new float[ChannelCount, targetLength];
            currentBufferLength = targetLength;
        }
        public void StartRecord(int wait = 0)
        {
            waitingForRecord = wait;
            currentRecording = 0;
            recording = true;
        }
        public void StopRecord()
        {
            recording = false;
        }
        public void StartPlay(int wait = 0)
        {
            waitingForPlay = wait;
            currentPlaying = 0;
            playing = true;
        }
        public void StopPlay()
        {
            playing = false;
        }
        public override void Update()
        {
            if (Buffer == null)
                return;
            if (playing)
            {
                if (waitingForPlay != 0)
                {
                    --waitingForPlay;
                }
                else
                {
                    for (int i = 0; i < ChannelCount; ++i)
                    {
                        WriteOutput(i, Buffer[i, currentPlaying]);
                    }
                    ++currentPlaying;
                    if (currentPlaying >= currentBufferLength)
                    {
                        currentPlaying = 0;
                        if (!playingLoop)
                            playing = false;
                    }
                }
            }
            else
            {
                for (int i = 0; i < ChannelCount; ++i)
                    WriteOutput(i, .0F);
            }
            if (recording)
            {
                if (waitingForRecord != 0)
                {
                    --waitingForRecord;
                }
                else
                {
                    for (int i = 0; i < ChannelCount; ++i)
                    {
                        Buffer[i, currentRecording] = ReadInput(i);
                    }
                    ++currentRecording;
                    if (currentRecording >= currentBufferLength)
                    {
                        currentRecording = 0;
                        recording = false;
                    }
                }
            }
        }
    }
}
