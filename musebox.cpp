#include "musebox.h"
#include <QDeclarativeContext>
#include <QtDeclarative>
#include "qmlapplicationviewer.h"
#include "hardware.h"
#include "trackarrangementbackground.h"
#include "cursorruler.h"
#include "DataModel/patternmodel.h"
#include "patterndisplay.h"
#include "patternnotedisplay.h"
#include "DataModel/notemodel.h"
#include <QGLWidget>

using namespace QGL;

MuseBox::MuseBox(QWidget *parent) :
    QMainWindow(parent)
{
    //TODO add a splash screen here? would be totally fun!
    qmlRegisterType<TrackArrangementBackground>("TrackArrangement",1,0,"TrackArrangementBackground");
    qmlRegisterType<CursorRuler>("TrackArrangement",1,0,"CursorRuler");
    qmlRegisterType<PatternModel>("DataModel",1,0,"PatternModel");
    qmlRegisterType<NoteModel>("DataModel",1,0,"NoteModel");
    qmlRegisterType<PatternDisplay>("TrackArrangement",1,0,"PatternDisplay");
    qmlRegisterType<PatternNoteDisplay>("TrackArrangement",1,0,"PatternNoteDisplay");
    QmlApplicationViewer *view = new QmlApplicationViewer();
    view->rootContext()->setContextProperty("musebox", this);
    view->rootContext()->setContextProperty("trackModel", &this->trackModel);
    view->setSource(QUrl::fromLocalFile("qml/MuseBox/main.qml"));
    view->setResizeMode(QDeclarativeView::SizeRootObjectToView);


    QGLFormat format = QGLFormat::defaultFormat();
    format.setSampleBuffers(false);
    QGLWidget *glWidget = new QGLWidget(format);
    glWidget->setAutoFillBackground(false);
    view->setViewport(glWidget);

    setCentralWidget(view);
    Hardware::Init();
    view->rootContext()->setContextProperty("transposeMachine", Hardware::TransposeMachine);
    Hardware::StartAudio();
}
