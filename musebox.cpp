#include "musebox.h"
#include <QDeclarativeContext>
#include "qmlapplicationviewer.h"
#include "hardware.h"

MuseBox::MuseBox(QWidget *parent) :
    QMainWindow(parent)
{
    //TODO add a splash screen here? would be totally fun!
    QmlApplicationViewer *view = new QmlApplicationViewer();
    view->rootContext()->setContextProperty("musebox", this);
    view->rootContext()->setContextProperty("trackModel", &this->trackModel);
    view->setSource(QUrl::fromLocalFile("qml/MuseBox/main.qml"));
    view->setResizeMode(QDeclarativeView::SizeRootObjectToView);
    setCentralWidget(view);
    Hardware::Init();
//    view->rootContext()->setContextProperty("transposeMachine",Hardware::TransposeMachine);
    Hardware::StartAudio();
}
