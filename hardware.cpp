#include "hardware.h"
#include <QMap>
#include <QSettings>
#include <RtAudio.h>
#include <QVector>
#include <vector>
#include <QDebug>
#include "DSP/devicetuple.h"
#include "DSP/audiointerface.h"
#include <cmath>

RtAudio* Hardware::rtAudioInstance;
RtAudio::Api Hardware::currentApi;
QString Hardware::InputDeviceName;
QString Hardware::OutputDeviceName;
RtAudio::StreamParameters Hardware::InputDeviceParameters;
RtAudio::StreamParameters Hardware::OutputDeviceParameters;
unsigned int Hardware::SampleRate;
unsigned int Hardware::BufferFrames;
unsigned int Hardware::TimeStamp;
QVector<Device*> Hardware::DeviceList;
QVector<DeviceTuple> Hardware::DependencyList;
QMutex Hardware::locker;
AudioInterfaceInput* Hardware::AudioInput;
AudioInterfaceOutput* Hardware::AudioOutput;
QVector<RtAudio::DeviceInfo> Hardware::audioDeviceList;
StereoMixer* Hardware::MainMixer;
Transpose* Hardware::TransposeMachine;
bool Hardware::Monitor = true;
QVector<float> MasterLevel;
MessageBus Hardware::messageBus;


unsigned int ProbeDeviceId(QString devName)
{
    for(unsigned int i=0,j=Hardware::audioDeviceList.count();i<j;++i)
    {
        RtAudio::DeviceInfo info = Hardware::audioDeviceList[i];
        if(devName == QString(info.name.c_str()))
            return i;
    }
    return -1;
}
//I curse scoped enum!
enum Api {
    UNSPECIFIED,    /*!< Search for a working compiled API. */
    LINUX_ALSA,     /*!< The Advanced Linux Sound Architecture API. */
    LINUX_OSS,      /*!< The Linux Open Sound System API. */
    UNIX_JACK,      /*!< The Jack Low-Latency Audio Server API. */
    MACOSX_CORE,    /*!< Macintosh OS-X Core Audio API. */
    WINDOWS_ASIO,   /*!< The Steinberg Audio Stream I/O API. */
    WINDOWS_DS,     /*!< The Microsoft Direct Sound API. */
    RTAUDIO_DUMMY   /*!< A compilable but non-functional API. */
  };

void Hardware::Init()
{
#ifdef Q_WS_WIN
    currentApi = (RtAudio::Api)WINDOWS_ASIO;
#endif
#ifdef Q_WS_MAC
    currentApi = MACOSX_CORE;
#endif
    rtAudioInstance = new RtAudio(currentApi);
    LoadConfig();
    PrintConfig();

    AudioInput = new AudioInterfaceInput(InputDeviceParameters.nChannels);
    AudioOutput = new AudioInterfaceOutput(OutputDeviceParameters.nChannels);
    Hardware::InstallDevice(AudioOutput);
    MainMixer = new StereoMixer();
    TransposeMachine = new Transpose(120);
    Hardware::InstallDevice(MainMixer);
    Hardware::InstallDevice(TransposeMachine);
    AudioOutput->PlugInput(0,MainMixer,0);
    AudioOutput->PlugInput(1,MainMixer,1);
    MasterLevel.push_back(0);
    MasterLevel.push_back(0);//Assuming stereo configuration
}

//Should be called in Update() procedures
float Hardware::ReadAudioInput(int channel)
{
    return AudioInput->OutputChannels[channel]->Data;
}

float Hardware::MasterDb(int channel)
{
    return MasterLevel[channel];
}

void Hardware::DeInit()
{
    SaveConfig();
    Lock();//Important. Otherwise the rt thread may get killed while still holding the lock
    StopAudio();
    Unlock();
    //delete rtAudioInstance;
    //rtAudioInstance = NULL;
    //delete AudioInput;
    //RemoveDevice(AudioOutput);
    //RemoveDevice(MainMixer);
    //RemoveDevice(TransposeMachine);
}

//{{{ Config related

void Hardware::PrintConfig()
{
    qDebug()<<"Input device : "<<InputDeviceName;
    qDebug()<<"Output device : "<<OutputDeviceName;
}

void Hardware::LoadConfig()
{
    int hwCount = rtAudioInstance->getDeviceCount();
    for(int i=0;i<hwCount;++i)
        audioDeviceList.push_back(rtAudioInstance->getDeviceInfo(i));


    QSettings setting("Yadli studio","MuseBox");
    InputDeviceName  = setting.value("hardware/inputDeviceName", "").value<QString>();
    OutputDeviceName = setting.value("hardware/outputDeviceName", "").value<QString>();
    SampleRate       = setting.value("hardware/sampleRate", 44100).value<unsigned int>();
    BufferFrames     = setting.value("hardware/bufferFrames", 512).value<unsigned int>();

    InputDeviceParameters.deviceId = ProbeDeviceId(InputDeviceName);
    OutputDeviceParameters.deviceId = ProbeDeviceId(OutputDeviceName);

    if(InputDeviceParameters.deviceId == -1 || OutputDeviceParameters.deviceId == -1)
        NewConfig();
    else
    {
        InputDeviceParameters.firstChannel = setting.value("hardware/inputFirstChannel").value<unsigned int>();
        InputDeviceParameters.nChannels = setting.value("hardware/inputChannelCount").value<unsigned int>();

        OutputDeviceParameters.firstChannel = setting.value("hardware/outputFirstChannel").value<unsigned int>();
        OutputDeviceParameters.nChannels = setting.value("hardware/outputChannelCount").value<unsigned int>();
    }
}

void Hardware::SaveConfig()
{
    QSettings setting("Yadli studio","MuseBox");
    setting.setValue("hardware/inputDeviceName", InputDeviceName);
    setting.setValue("hardware/outputDeviceName", OutputDeviceName);
    setting.setValue("hardware/sampleRate",SampleRate);
    setting.setValue("hardward/bufferFrames", BufferFrames);

    setting.setValue("hardware/inputFirstChannel",InputDeviceParameters.firstChannel);
    setting.setValue("hardware/inputChannelCount",InputDeviceParameters.nChannels);

    setting.setValue("hardware/outputFirstChannel",OutputDeviceParameters.firstChannel);
    setting.setValue("hardware/outputChannelCount",OutputDeviceParameters.nChannels);
}

void Hardware::NewConfig()
{
    RtAudio::DeviceInfo inputInfo,outputInfo;
    inputInfo = rtAudioInstance->getDeviceInfo(rtAudioInstance->getDefaultInputDevice());
    outputInfo = rtAudioInstance->getDeviceInfo(rtAudioInstance->getDefaultOutputDevice());
    InputDeviceName = QString(inputInfo.name.c_str());
    OutputDeviceName = QString(outputInfo.name.c_str());
    InputDeviceParameters.deviceId = rtAudioInstance->getDefaultInputDevice();
    OutputDeviceParameters.deviceId = rtAudioInstance->getDefaultOutputDevice();
    InputDeviceParameters.firstChannel = 0;
    InputDeviceParameters.nChannels = inputInfo.inputChannels;

    OutputDeviceParameters.firstChannel = 0;
    OutputDeviceParameters.nChannels = outputInfo.outputChannels;

    //TODO check if samplerate is supported.
}

QVector<RtAudio::DeviceInfo> Hardware::GetDevices()
{
    return audioDeviceList;
}


//}}}
void Hardware::StartAudio()
{
    StopAudio();
    try{
    rtAudioInstance->openStream(&OutputDeviceParameters,
                                &InputDeviceParameters,
                                RTAUDIO_FLOAT32,
                                SampleRate,
                                &BufferFrames,
                                rtAudioCallback,
                                NULL, // User data
                                NULL  // Stream options
                                );
    }catch(...)
    {
        qDebug()<<"Having some problem with buffer size... Fallback to a preferred size";
        BufferFrames = rtAudioInstance->getPreferredBufferSize();
        rtAudioInstance->openStream(&OutputDeviceParameters,
                                    &InputDeviceParameters,
                                    RTAUDIO_FLOAT32,
                                    SampleRate,
                                    &BufferFrames,
                                    rtAudioCallback,
                                    NULL, // User data
                                    NULL  // Stream options
                                    );
    }
    rtAudioInstance->startStream();
    qDebug()<<"Audio started";
}

void Hardware::StopAudio()
{
//    if(rtAudioInstance->isStreamRunning())
//        rtAudioInstance->abortStream();
    if(rtAudioInstance->isStreamOpen())
        rtAudioInstance->closeStream();
    qDebug()<<"Audio stopped";
}

void Hardware::ResetAudio()
{
    StopAudio();
    StartAudio();
}

void Hardware::UpdateSettings(int deviceID, RtAudio::DeviceInfo devInfo, unsigned int sr, unsigned int bufferSize)
{
    StopAudio();

    OutputDeviceParameters.deviceId = InputDeviceParameters.deviceId = deviceID;
    OutputDeviceParameters.firstChannel = InputDeviceParameters.firstChannel = 0;
    OutputDeviceParameters.nChannels = devInfo.outputChannels;
    InputDeviceParameters.nChannels = devInfo.inputChannels;

    SampleRate = sr;
    BufferFrames = bufferSize;

    InputDeviceName = OutputDeviceName = QString(devInfo.name.c_str());
    TransposeMachine->SetBPM(TransposeMachine->BPM);
    //TODO what if channel count have changed?
    StartAudio();
}

/**
typedef int(* RtAudioCallback)(void *outputBuffer, void *inputBuffer, unsigned int nFrames, double streamTime, RtAudioStreamStatus status, void *userData)
RtAudio callback function prototype.

All RtAudio clients must create a function of type RtAudioCallback to read and/or write data from/to the audio stream. When the underlying audio system is ready for new input or output data, this function will be invoked.

Parameters:
        outputBuffer 	For output (or duplex) streams, the client should write nFrames of audio sample frames into this buffer. This argument should be recast to the datatype specified when the stream was opened. For input-only streams, this argument will be NULL.
        inputBuffer 	For input (or duplex) streams, this buffer will hold nFrames of input audio sample frames. This argument should be recast to the datatype specified when the stream was opened. For output-only streams, this argument will be NULL.
        nFrames 	The number of sample frames of input or output data in the buffers. The actual buffer size in bytes is dependent on the data type and number of channels in use.
        streamTime 	The number of seconds that have elapsed since the stream was started.
        status 	If non-zero, this argument indicates a data overflow or underflow condition for the stream. The particular condition can be determined by comparison with the RtAudioStreamStatus flags.
        userData 	A pointer to optional data provided by the client when opening the stream (default = NULL).

To continue normal stream operation, the RtAudioCallback function should return a value of zero. To stop the stream and drain the output buffer, the function should return a value of one. To abort the stream immediately, the client should return a value of two.
*/
int Hardware::rtAudioCallback(void* outputBuffer, void* inputBuffer, uint nFrames, double streamTime, RtAudioStreamStatus status, void* value)
{
    float* outputPtr = (float*)outputBuffer;
    float* inputPtr = (float*)inputBuffer;

    int maxMutexWaitTime = (nFrames / (float)SampleRate) *1000.0;

    if(!locker.tryLock(maxMutexWaitTime))//TODO emit CPU overload signal
        return 0;

    for (uint i = 0; i != nFrames; ++i)
    {
        for (unsigned int j = 0; j < InputDeviceParameters.nChannels; ++j)
            AudioInput->WriteOutput(j, *(inputPtr++));
        Update();
        for (unsigned int j = 0; j < OutputDeviceParameters.nChannels; ++j)
        {
            float val = AudioOutput->ReadInput(j);
            if(j < 2)
                val += TransposeMachine->OutputChannels[j]->Data;
            *(outputPtr++) = val;
            if(fabs(val) > MasterLevel[j])
                MasterLevel[j] = fabs(val);
        }
    }

    for(int i=0;i<OutputDeviceParameters.nChannels;++i)
        MasterLevel[i] *= 0.7;

    locker.unlock();

    return 0;
}
//Source depends on target, sounds weird isn't it...
//Note that if a device is not registered as a global device, the dependency won't be count in.
bool Hardware::InstallDependency(Device* source, Device* target)
{
    if(!DeviceList.contains(source))
        return false;
    if(!DeviceList.contains(target))
        return false;
    DeviceTuple t(source, target);
    DependencyList.push_back(t);
    UpdateComputationSequence();
    return true;
}
bool Hardware::RemoveDependency(Device* source, Device* target)
{
    for(int i=0;i<DependencyList.count();++i)
        if(DependencyList[i].Source == source && DependencyList[i].Target == target)
        {
            DependencyList.remove(i);
            break;
        }
    UpdateComputationSequence();
    return true;
}
bool Hardware::InstallDevice(Device* dsp)
{
    DeviceList.push_back(dsp);
    return true;
}
bool Hardware::RemoveDevice(Device* dsp)
{
    qDebug()<<"RemoveDevice";
    for(int i=0;i<DeviceList.count();++i)
        if(DeviceList[i] == dsp)
        {
            DeviceList.remove(i);
            --i;
        }
    for (int i = 0; i < DependencyList.count(); )
    {
        if (DependencyList[i].Source == dsp || DependencyList[i].Target == dsp)
        {
            if (dsp == DependencyList[i].Target)
            {
                qDebug()<<"RemoveInputDevice";
                DependencyList[i].Source->__RemoveInputDevice__(dsp);
            }
            DependencyList.remove(i);
            continue;
        }
        ++i;
    }
    UpdateComputationSequence();
    delete dsp;
    return true;
}

void DFS(int node, bool* visited, bool** graph, QVector<Device*>& computedSequence, QVector<Device*>& DeviceList, int Length)
{
    visited[node] = true;
    for (int i = 0; i < Length; ++i)
    {
        if (graph[node][i] && !visited[i])
            DFS(i, visited, graph, computedSequence, DeviceList,Length);
    }
    computedSequence.push_back(DeviceList[node]);
}

//Caller should hold locker
void Hardware::UpdateComputationSequence()
{
    QMap<Device*, int> dspMap;
    for (int i = 0; i < DeviceList.count(); ++i)
        dspMap[DeviceList[i]] = i;
    bool **graph = new bool*[DeviceList.count()];
    for(int i=0;i<DeviceList.count();++i)
    {
        graph[i] = new bool[DeviceList.count()];
        memset(graph[i],0,DeviceList.count()*sizeof(bool));
    }
    bool *visited = new bool[DeviceList.count()];
    memset(visited,0, sizeof(bool)*DeviceList.count());
    QVector<Device*> computedSequence;
    foreach (DeviceTuple dep,DependencyList)
    {
        //Item1 depends on item2
        graph[dspMap[dep.Source]][dspMap[dep.Target]] = true;
    }
    for (int i = 0; i < DeviceList.count(); ++i)
    {
        if (!visited[i])
        {
            DFS(i,visited,graph,computedSequence,DeviceList,DeviceList.count());
        }
    }
    DeviceList = computedSequence;
    //qDebug()<<"Device computation sequence updated:";
    //foreach (Device* dev , computedSequence)
    //{
    //    qDebug()<<(dev->DeviceType());
    //}
    delete[] visited;
    for(int i=0;i<DeviceList.count();++i)
        delete[] graph[i];
    delete[] graph;
}
/////////Called by the RT-Thread, need to lock
void Hardware::Update()
{
    foreach (Device* dsp ,DeviceList)
        dsp->Update();
    ++TimeStamp;
}
