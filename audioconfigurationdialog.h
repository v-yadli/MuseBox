#ifndef AUDIOCONFIGURATIONDIALOG_H
#define AUDIOCONFIGURATIONDIALOG_H

#include <QDialog>
#include <QVector>
#include <RtAudio.h>


namespace Ui {
class AudioConfigurationDialog;
}

class AudioConfigurationDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit AudioConfigurationDialog(QWidget *parent = 0);
    ~AudioConfigurationDialog();

public slots:
    void ApplyClicked();
    void CancelClicked();
    void OkClicked();
    void BufferSizeSliderChanged(int);
    void audioInterfaceChanged(int);

private:
    Ui::AudioConfigurationDialog *ui;
    QVector<RtAudio::DeviceInfo> devices;
    int devInUse;
};

#endif // AUDIOCONFIGURATIONDIALOG_H
