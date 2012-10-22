#ifndef HARDWARE_H
#define HARDWARE_H

#include <RtAudio.h>
#include <QVector>
#include <QMutex>
#include "DSP/device.h"
#include "DSP/devicetuple.h"
#include "DSP/audiointerface.h"
#include "DSP/transpose.h"
#include "DSP/mixer.h"

class Hardware
{
public:
    static void Init();
    static void DeInit();

    static void LoadConfig();
    static void SaveConfig();
    static void NewConfig();
    static void PrintConfig();
    static QVector<RtAudio::DeviceInfo> GetDevices();

    static void StartAudio();
    static void StopAudio();
    static void ResetAudio();
    static void UpdateSettings( int deviceID, RtAudio::DeviceInfo devInfo, unsigned int sr, unsigned int bufferSize);

    static bool InstallDependency(Device* source, Device* target);
    static bool RemoveDependency(Device* source, Device* target);
    static bool InstallDevice(Device* dsp);
    static bool RemoveDevice(Device* dsp);
    static void UpdateComputationSequence();
    static void Update();

    static QString InputDeviceName;
    static QString OutputDeviceName;
    static RtAudio::StreamParameters InputDeviceParameters;
    static RtAudio::StreamParameters OutputDeviceParameters;
    static unsigned int SampleRate;
    static unsigned int BufferFrames;

    static float ReadAudioInput(int channel);
    static float MasterDb(int channel);

    static void Lock(){locker.lock();}
    static void Unlock(){locker.unlock();}


    static float Jiffy()
    {
        return 1000.0 / SampleRate;
    }
    static float CurrentTime()
    {
        return Jiffy() * TimeStamp;
    }
    static AudioInterfaceInput* AudioInput;
    static AudioInterfaceOutput* AudioOutput;
    static QVector<RtAudio::DeviceInfo> audioDeviceList;

    static Transpose* TransposeMachine;
    static StereoMixer* MainMixer;

    static bool Monitor;

private:
    static RtAudio* rtAudioInstance;
    static RtAudio::Api currentApi;
    static unsigned int TimeStamp;

    static QVector<Device*> DeviceList;
    static QVector<DeviceTuple> DependencyList;


    static int rtAudioCallback(void *outputBuffer,
                               void *inputBuffer,
                               unsigned int nFrames,
                               double streamTime,
                               RtAudioStreamStatus status,
                               void *userData);
    //Always lock up the hardware when doing modifications to some values!
    static QMutex locker;

};

class _HWLock
{
    public:
    _HWLock()
    {
        Hardware::Lock();
    }
    ~_HWLock()
    {
        Hardware::Unlock();
    }
};

#define HWLOCK _HWLock lock;

#endif // HARDWARE_H
