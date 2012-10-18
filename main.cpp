#include <QtGui/QApplication>
#include <QDeclarativeContext>
#include "qmlapplicationviewer.h"
#include "hardware.h"
#include "qmlbridge.h"
#include "DSP/delay.h"

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QScopedPointer<QApplication> app(createApplication(argc, argv));
    QCoreApplication::setApplicationName("MuseBox");
    QCoreApplication::setOrganizationName("Yadli studio");
    QCoreApplication::setOrganizationDomain("yaldi.net");

    QDeclarativeView view;
    QMLBridge bridge;
    view.rootContext()->setContextProperty("bridge", &bridge);

    view.setSource(QUrl::fromLocalFile("qml/MuseBox/main.qml"));
    view.show();
    Hardware::Init();

    Delay *l = new Delay(), *r = new Delay();

    Hardware::AudioOutput->PlugInput(0,l,0);
    Hardware::AudioOutput->PlugInput(1,r,0);

    l->PlugInput(0,Hardware::AudioInput,0);
    r->PlugInput(0,Hardware::AudioInput,1);

    Hardware::StartAudio();


    return app->exec();
}
