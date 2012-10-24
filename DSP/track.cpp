#include "track.h"
#include "../hardware.h"
#include <QDebug>
#include "../DataModel/trackmodel.h"

Track::Track(int ChannelCount):Device(0,ChannelCount)
{
    for(int i=0;i<ChannelCount;++i)
        recordingChannels.push_back(i);
    recording = false;
    readyToRecord = false;
    currentRecordingSession = NULL;
    currentRecordingPattern = NULL;
}

void Track::Update()
{
    if(Hardware::TransposeMachine->Playing)
    {
        if(Hardware::TransposeMachine->Recording && (this->recording || this->readyToRecord))
        {
            if(this->readyToRecord){
                readyToRecord = false;
                recording = true;
                this->currentRecordingSession = new PatternModel();
                this->model->pulse(this);
            }
            //Recording...
            if(currentRecordingPattern == NULL || Hardware::TransposeMachine->PositionJumped)
            {
                //qDebug()<<"recording a new pattern! :-D";
                //if(currentRecordingPattern != NULL)
                //    qDebug()<<QString("%1 samples collected!").arg(currentRecordingPattern->length());
                this->currentRecordingPattern = this->currentRecordingSession->recordNewPattern();
            }
            //TODO monitor flag
            //for(int i=0;i<OutputChannelCount;++i)
            //{
            //    float data = Hardware::ReadAudioInput(recordingChannels[i]);
            //    currentRecordingPattern->Put(i,data);
            //    WriteOutput(i,data);
            //}
            float l = Hardware::ReadAudioInput(0);
            float r = Hardware::ReadAudioInput(1);
            WriteOutput(0,l);
            WriteOutput(1,r);
            currentRecordingPattern->Put(0,l);
            currentRecordingPattern->Put(1,r);

        }else//Play current arrangement, or monitor
        {
            if(readyToRecord){
                //TODO monitor flag
                //for(int i=0;i<OutputChannelCount;++i)
                //    WriteOutput(i,Hardware::ReadAudioInput(recordingChannels[i]));
                float l = Hardware::ReadAudioInput(0);
                float r = Hardware::ReadAudioInput(1);
                WriteOutput(0,l);
                WriteOutput(1,r);
            }else{
                StereoData s = arrangement.query(Hardware::TransposeMachine->Time);
                WriteOutput(0,s.l);
                WriteOutput(1,s.r);
                //TODO arrangement playing
            }
        }
    }else
    {
        for(int i=0;i<OutputChannelCount;++i)
            WriteOutput(i,0);
    }
}

int offset = 0;
int idx = 0;

void Track::setRecording(bool flag)
{
    if(flag)
    {
        if(!readyToRecord && !recording)
        {
            readyToRecord = true;
        }
        //otherwise, one of them is true (since they're mutex
        //and in this case(which shouldn't happen), just ignore it
    }else
    {
        //turning off
        if(readyToRecord)
            readyToRecord = false;//ignore it
        if(recording)
        {
            recording = false;
            //dump data into pool
            qDebug()<<QString("recorded %1 patterns!").arg(currentRecordingSession->rowCount());
            patternPool.import(currentRecordingSession);
            Pattern* p = currentRecordingSession->Get(0);
            PatternNote note;
            note.pattern = p;
            note.position = offset;
            note.length = p->length();
            note.offset = 0;
            offset += p->length();
            arrangement.insertNote(note,note.position);
            currentRecordingSession = NULL;
            currentRecordingPattern = NULL;
        }
    }
}
