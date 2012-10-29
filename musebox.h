#ifndef MUSEBOX_H
#define MUSEBOX_H

#include <QObject>
#include "DSP/track.h"
#include "audioconfigurationdialog.h"
#include "hardware.h"
#include <QDebug>
#include <QMainWindow>
#include <DataModel/trackmodel.h>
#include "newprojectdialog.h"
#include "exportaudiodialog.h"
#include <QFileDialog>
#include <sndfile.h>

//Main MuseBox class. Handles Track addition/remove logic, interaction between audio engine and GUI, and file load/save.

class MuseBox : public QMainWindow
{
    Q_OBJECT
public:
    explicit MuseBox(QWidget *parent = 0);

    //Project related
    void NewProject();
    void LoadProject(QString path);
    void SaveProject(QString path);

    //Track manipulation
    void AddTrack();
    void RemoveTrack(int idx);
    void MoveTrack(int from, int to);
    Track* GetTrack(int idx);

    bool playStateBeforeMove;//Used to recover play state after a FF/RW action
    TrackModel trackModel;

    Q_INVOKABLE void resetAudio()
    {
        Hardware::ResetAudio();
    }
    Q_INVOKABLE void startAudio()
    {
        Hardware::StartAudio();
    }
    Q_INVOKABLE void stopAudio()
    {
        Hardware::StopAudio();
    }

    Q_INVOKABLE void openConfigurationDialog() {
        qDebug()<<"Opening dialog...";
        AudioConfigurationDialog* dialog = new AudioConfigurationDialog();
        dialog->show();
    }
    void onExit()
    {
        Hardware::SaveConfig();
        exit(0);//Surprise!!! LEAK LEAK LEAK!
        Hardware::DeInit();
    }

    Q_INVOKABLE void play()
    {
        HWLOCK;
        Hardware::TransposeMachine->Play();
    }
    Q_INVOKABLE void stop()
    {
        HWLOCK;
        Hardware::TransposeMachine->Stop();
    }
    Q_INVOKABLE void setRecord(bool flag)
    {
        HWLOCK;
        Hardware::TransposeMachine->SetRecord(flag);
    }
    Q_INVOKABLE void togglePlayStop()
    {
        HWLOCK;
        Hardware::TransposeMachine->TogglePlayStop();
    }
    Q_INVOKABLE void moveStart()
    {
        HWLOCK;
        playStateBeforeMove = Hardware::TransposeMachine->Playing;
        Hardware::TransposeMachine->Play();
    }
    Q_INVOKABLE void moveEnd()
    {
        HWLOCK;
        if(!playStateBeforeMove)
            Hardware::TransposeMachine->Stop();
    }

    Q_INVOKABLE void setSpeed(float speed)
    {
        HWLOCK;
        Hardware::TransposeMachine->Speed = speed;
    }
    Q_INVOKABLE void setClick(bool click)
    {
        HWLOCK;
        Hardware::TransposeMachine->Click = click;
    }
    Q_INVOKABLE void setLoop(bool loop)
    {
        HWLOCK;
        Hardware::TransposeMachine->Loop = loop;
    }

    Q_INVOKABLE void beginUpdateGUI()
    {
        Hardware::Lock();
    }
    Q_INVOKABLE void endUpdateGUI()
    {
        Hardware::Unlock();
    }

    Q_INVOKABLE float query_dBL(int channel)
    {
        return Hardware::MainMixer->InputMixerChannels[channel]->dbL();
    }
    Q_INVOKABLE float query_dBR(int channel)
    {
        return Hardware::MainMixer->InputMixerChannels[channel]->dbR();
    }

    Q_INVOKABLE float masterL()
    {
        return Hardware::MasterDb(0);
    }
    Q_INVOKABLE float masterR()
    {
        return Hardware::MasterDb(1);
    }

    //posOnGUI is measured in pixels
    Q_INVOKABLE void setCurrentPosition(int posOnGUI)
    {
        HWLOCK;
        Hardware::TransposeMachine->setCurrentPosition(posOnGUI);
    }
    Q_INVOKABLE void setLoopStart(int posOnGUI)
    {
        HWLOCK;
        Hardware::TransposeMachine->setLoopStart(posOnGUI);
    }
    Q_INVOKABLE void setLoopEnd(int posOnGUI)
    {
        HWLOCK;
        Hardware::TransposeMachine->setLoopEnd(posOnGUI);
    }


    Q_INVOKABLE QString saveProject(QString filename = "")
    {
        if(filename == ""){
            filename = QFileDialog::getSaveFileName(this, "Save to...",QString(),"*.mb",NULL,0);
        }
        if(filename != "")
        {
            trackModel.saveFile(filename);
        }
        return filename;
    }
    Q_INVOKABLE QString loadProject(QString filename = "")
    {
        if(filename == ""){
            filename = QFileDialog::getOpenFileName(this, "Open...",QString(),"*.mb",NULL,0);
        }
        if(filename != "")
        {
            trackModel.clear();
            trackModel.loadFile(filename);
        }
        return filename;
    }
    Q_INVOKABLE void clearProject()
    {
        trackModel.clear();
    }
    Q_INVOKABLE void newProject()
    {
        NewProjectDialog dialog;
        if( QDialog::Accepted == dialog.exec())
        {
            trackModel.clear();
            Hardware::TransposeMachine->beatCount = dialog.beatCount;
            Hardware::TransposeMachine->unitCount = dialog.unitCount;
            Hardware::TransposeMachine->SetBPM(dialog.bpm);
        }

    }
    Q_INVOKABLE void exportSong(){
        Hardware::StopAudio();
        ExportAudioDialog dialog(this);
        if(QDialog::Accepted != dialog.exec())
            return;

        int loopStart,loopEnd,time;
        loopStart = Hardware::TransposeMachine->LoopStart;
        loopEnd = Hardware::TransposeMachine->LoopEnd;
        time = Hardware::TransposeMachine->Time;
        short* dataFrame = new short[2];

        SF_INFO info;
        info.channels=2;
        info.samplerate = Hardware::SampleRate;
        info.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
        SNDFILE* file = sf_open(dialog.filename().toLocal8Bit().data(),SFM_WRITE,&info);
        for(int i=0;i<Hardware::AudioInput->OutputChannelCount;++i)
        {
            Hardware::AudioInput->WriteOutput(i,0);
        }

        Hardware::TransposeMachine->Play();

        if(dialog.isSong()){
            int SamplePerBar = Hardware::TransposeMachine->BeatsToSample(Hardware::TransposeMachine->beatCount);
            int startPosition = SamplePerBar * dialog.fromBar();
            int endPosition = SamplePerBar * (dialog.toBar() + 1);

            Hardware::TransposeMachine->Time = startPosition;
            Hardware::TransposeMachine->LoopEnd = startPosition - 1;//To prevent looping
            for(int i=startPosition;i < endPosition;++i)
            {
                Hardware::Update();
                dataFrame[0] = Hardware::AudioOutput->ReadInput(0) * 32767;
                dataFrame[1] = Hardware::AudioOutput->ReadInput(1) * 32767;
                sf_write_short(file,dataFrame,2);
            }

        }else{
            Hardware::TransposeMachine->Time = loopStart;
            for(int i=0;i<dialog.loopCount();++i){
                for(int j=0;j<(loopEnd - loopStart+1);++j)
                {
                    Hardware::Update();
                    dataFrame[0] = Hardware::AudioOutput->ReadInput(0) * 32767;
                    dataFrame[1] = Hardware::AudioOutput->ReadInput(1) * 32767;
                    sf_write_short(file,dataFrame,2);
                }
            }
        }

        sf_close(file);
        Hardware::TransposeMachine->Stop();
        qDebug()<<"File write complete!";
        Hardware::TransposeMachine->LoopStart = loopStart;
        Hardware::TransposeMachine->LoopEnd = loopEnd;
        Hardware::TransposeMachine->Time = time;
        Hardware::StartAudio();
        delete[] dataFrame;
    }

    virtual void closeEvent(QCloseEvent *event)
    {
        onExit();
        QMainWindow::closeEvent(event);
    }
signals:
    
public slots:
    
};

#endif // MUSEBOX_H
