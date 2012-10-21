#include <QtGui/QApplication>
#include "qmlapplicationviewer.h"
#include "musebox.h"

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QScopedPointer<QApplication> app(createApplication(argc, argv));
    QCoreApplication::setApplicationName("MuseBox");
    QCoreApplication::setOrganizationName("Yadli studio");
    QCoreApplication::setOrganizationDomain("yaldi.net");

    MuseBox museBox;
    museBox.show();

    return app->exec();
}
