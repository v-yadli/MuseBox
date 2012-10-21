#include <QtGui/QApplication>
#include <QDeclarativeContext>
#include "qmlapplicationviewer.h"
#include "hardware.h"
#include "musebox.h"

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QScopedPointer<QApplication> app(createApplication(argc, argv));
    QCoreApplication::setApplicationName("MuseBox");
    QCoreApplication::setOrganizationName("Yadli studio");
    QCoreApplication::setOrganizationDomain("yaldi.net");

    QmlApplicationViewer view;
    MuseBox museBox;
    view.rootContext()->setContextProperty("musebox", &museBox);

    view.setSource(QUrl::fromLocalFile("qml/MuseBox/main.qml"));
    view.setResizeMode(QDeclarativeView::SizeRootObjectToView);
    museBox.setCentralWidget(&view);
    museBox.show();
    Hardware::Init();
    Hardware::StartAudio();


    return app->exec();
}
