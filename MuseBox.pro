# Add more folders to ship with the application, here
folder_01.source = qml/MuseBox
folder_01.target = qml
DEPLOYMENTFOLDERS = folder_01

# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH =

symbian:TARGET.UID3 = 0xE14BCCBF

# Smart Installer package's UID
# This UID is from the protected range and therefore the package will
# fail to install if self-signed. By default qmake uses the unprotected
# range value if unprotected UID is defined for the application and
# 0x2002CCCF value if protected UID is given to the application
#symbian:DEPLOYMENT.installer_header = 0x2002CCCF

# If your application uses the Qt Mobility libraries, uncomment the following
# lines and add the respective components to the MOBILITY variable.
# CONFIG += mobility
# MOBILITY +=

# Speed up launching on MeeGo/Harmattan when using applauncherd daemon
# CONFIG += qdeclarative-boostable

# Add dependency to Symbian components
# CONFIG += qt-components

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += main.cpp \
    hardware.cpp \
    RtAudio.cpp \
    include/iasiothiscallresolver.cpp \
    include/asiolist.cpp \
    include/asiodrivers.cpp \
    include/asio.cpp \
    audioconfigurationdialog.cpp \
    qmlbridge.cpp \
    DSP/endpoint.cpp \
    DSP/devicetuple.cpp \
    DSP/device.cpp \
    DSP/audiointerface.cpp \
    DSP/delay.cpp

# Please do not modify the following two lines. Required for deployment.
include(qmlapplicationviewer/qmlapplicationviewer.pri)
qtcAddDeployment()

win32:DEFINES -= UNICODE
win32:DEFINES += __WINDOWS_ASIO__
#For MINGW, use this:
win32:LIBS += -lkernel32 -luser32 -lgdi32 -lwinspool -lcomdlg32 -ladvapi32 -lshell32 -lole32 -loleaut32 -luuid -lodbc32 -lodbccp32
#For a MSVC compiler, uncomment the following line:
#win32:LIBS += kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib

HEADERS += \
    hardware.h \
    RtError.h \
    RtAudio.h \
    include/soundcard.h \
    include/iasiothiscallresolver.h \
    include/iasiodrv.h \
    include/ginclude.h \
    include/dsound.h \
    include/asiosys.h \
    include/asiolist.h \
    include/asiodrvr.h \
    include/asiodrivers.h \
    include/asio.h \
    audioconfigurationdialog.h \
    qmlbridge.h \
    DSP/endpoint.h \
    DSP/devicetuple.h \
    DSP/device.h \
    DSP/audiointerface.h \
    DSP/delay.h

FORMS += \
    audioconfigurationdialog.ui
