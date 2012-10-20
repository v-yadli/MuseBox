#include "audioconfigurationdialog.h"
#include "ui_audioconfigurationdialog.h"
#include "hardware.h"

AudioConfigurationDialog::AudioConfigurationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AudioConfigurationDialog)
{
    ui->setupUi(this);

    devices = Hardware::GetDevices();
    devInUse = -1;
    for(int i=0;i<devices.count();++i)
    {
        RtAudio::DeviceInfo info = devices[i];
        QString devName = QString(info.name.c_str());
        if(devName == Hardware::InputDeviceName)
        {
            devInUse = i;
            devName = "[Current]" + devName;
        }
        ui->audioIFComboBox->addItem(devName);
    }

    if(devInUse == -1)
        devInUse = 0;

    ui->audioIFComboBox->setCurrentIndex(devInUse);
    ui->bufferSizeSlider->setValue(Hardware::BufferFrames);

}

AudioConfigurationDialog::~AudioConfigurationDialog()
{
    delete ui;
}

void AudioConfigurationDialog::audioInterfaceChanged(int idx)
{
    ui->sampleRateComboBox->clear();
    int selected = 0;
    for(int i=0;i<devices[idx].sampleRates.size();++i)
    {
        ui->sampleRateComboBox->addItem(QString("%0").arg(devices[idx].sampleRates[i]));
        if(devices[idx].sampleRates[i] == Hardware::SampleRate)
            selected = i;
    }
    ui->sampleRateComboBox->setCurrentIndex(selected);
}

void AudioConfigurationDialog::ApplyClicked()
{
    RtAudio::DeviceInfo currentDev = devices[ui->audioIFComboBox->currentIndex()];
    int idx = ui->audioIFComboBox->currentIndex();
//    if(idx == devInUse)
//        return;
    unsigned int sr = currentDev.sampleRates[ui->sampleRateComboBox->currentIndex()];
    unsigned int bufferSize = ui->bufferSizeSlider->value();
    Hardware::UpdateSettings( idx, currentDev, sr, bufferSize);
}

void AudioConfigurationDialog::CancelClicked()
{
    reject();
}

void AudioConfigurationDialog::OkClicked()
{
    ApplyClicked();
    accept();
}

void AudioConfigurationDialog::BufferSizeSliderChanged(int val)
{
    ui->bufferSizeLabel->setText(QString("%0 frames").arg(val));
}
