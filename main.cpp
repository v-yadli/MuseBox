#include <QtGui/QApplication>
#include <QDeclarativeContext>
#include "qmlapplicationviewer.h"
#include "hardware.h"
#include "qmlbridge.h"

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

    Hardware::StartAudio();


    return app->exec();
}
