#ifndef EXPORTAUDIODIALOG_H
#define EXPORTAUDIODIALOG_H

#include <QDialog>
#include <QString>

namespace Ui {
class ExportAudioDialog;
}

class ExportAudioDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit ExportAudioDialog(QWidget *parent = 0);
    ~ExportAudioDialog();
    QString filename();
    bool isSong();//otherwise loop
    int fromBar();
    int toBar();
    int loopCount();

public slots:
    void browseFile();
    
private:
    Ui::ExportAudioDialog *ui;
};

#endif // EXPORTAUDIODIALOG_H
