#ifndef QMLBRIDGE_H
#define QMLBRIDGE_H

#include <QObject>
#include <QDebug>
#include "audioconfigurationdialog.h"
#include "hardware.h"

class QMLBridge : public QObject
{
    Q_OBJECT
public:
    explicit QMLBridge(QObject *parent = 0);
    Q_INVOKABLE void OpenConfigurationDialog() {
        qDebug()<<"Opening dialog...";
        AudioConfigurationDialog* dialog = new AudioConfigurationDialog();
        dialog->show();
    }
    Q_INVOKABLE void OnExit()
    {
        Hardware::DeInit();
    }

public slots:
    void cppSlot(int number) {
        qDebug() << "Called the C++ slot with" << number;
    }
};

#endif // QMLBRIDGE_H
