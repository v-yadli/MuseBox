#ifndef MUSEBOX_H
#define MUSEBOX_H

#include <QObject>
#include "DSP/track.h"
#include "audioconfigurationdialog.h"
#include "hardware.h"
#include <QDebug>

//Main MuseBox class. Handles Track addition/remove logic, interaction between audio engine and GUI, and file load/save.

class MuseBox : public QObject
{
    Q_OBJECT
public:
    explicit MuseBox(QObject *parent = 0);

//Project related
    void NewProject();
    void LoadProject(QString path);
    void SaveProject(QString path);

//Track manipulation
    void AddTrack();
    void RemoveTrack(int idx);
    void MoveTrack(int from, int to);
    Track* GetTrack(int idx);

    Q_INVOKABLE void OpenConfigurationDialog() {
        qDebug()<<"Opening dialog...";
        AudioConfigurationDialog* dialog = new AudioConfigurationDialog();
        dialog->show();
    }
    Q_INVOKABLE void OnExit()
    {
        Hardware::DeInit();
    }

    Q_INVOKABLE void Play()
    {
        HWLOCK;
        Hardware::TransposeMachine->Play();
    }

    Q_INVOKABLE void Stop()
    {
        HWLOCK;
        Hardware::TransposeMachine->Stop();
    }

    Q_INVOKABLE void SetRecord(bool flag)
    {
        HWLOCK;
        Hardware::TransposeMachine->SetRecord(flag);
    }

    Q_INVOKABLE void TogglePlayStop()
    {
        HWLOCK;
        Hardware::TransposeMachine->TogglePlayStop();
    }


signals:
    
public slots:
    
};

#endif // MUSEBOX_H
