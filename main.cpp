#include <QtGui/QApplication>
#include <QGLFormat>
#include "qmlapplicationviewer.h"
#include "musebox.h"

using namespace QGL;

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QScopedPointer<QApplication> app(createApplication(argc, argv));
    app->setGraphicsSystem("opengl");
    QCoreApplication::setApplicationName("MuseBox");
    QCoreApplication::setOrganizationName("Yadli studio");
    QCoreApplication::setOrganizationDomain("yaldi.net");

    MuseBox museBox;
    museBox.show();

    return app->exec();
}
