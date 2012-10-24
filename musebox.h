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

    //TODO expose TransposeMachine to QML to reduce call counts
    Q_INVOKABLE int getBar()
    {
        return Hardware::TransposeMachine->getBar();
    }
    Q_INVOKABLE int getBeat()
    {
        return Hardware::TransposeMachine->getBeat();
    }
    //Measured in pixels
    Q_INVOKABLE int getLoopStart()
    {
        return Hardware::TransposeMachine->getLoopStart();
    }
    //Measured in pixels
    Q_INVOKABLE int getLoopEnd()
    {
        return Hardware::TransposeMachine->getLoopEnd();
    }
    Q_INVOKABLE float getPositionInBeat()//Measured in percentage, from 0 to 1
    {
        return Hardware::TransposeMachine->getPositionInBeat();
    }
    Q_INVOKABLE int getMinute()
    {
        return Hardware::TransposeMachine->getMinute();
    }
    Q_INVOKABLE int getSecond()
    {
        return Hardware::TransposeMachine->getSecond();
    }
    Q_INVOKABLE int getMillisecond()
    {
        return Hardware::TransposeMachine->getMillisecond();
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
    }
    Q_INVOKABLE QString loadProject(QString filename = "")
    {
    }
    Q_INVOKABLE void newProject()
    {
        Hardware::Lock();
        Hardware::StopAudio();
        Hardware::Unlock();

        NewProjectDialog dialog;
        if( QDialog::Accepted == dialog.exec())
        {
            trackModel.clear();
            Hardware::TransposeMachine->beatCount = dialog.beatCount;
            Hardware::TransposeMachine->unitCount = dialog.unitCount;
            Hardware::TransposeMachine->SetBPM(dialog.bpm);
        }

        Hardware::StartAudio();
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
