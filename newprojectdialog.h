#ifndef NEWPROJECTDIALOG_H
#define NEWPROJECTDIALOG_H

#include <QDialog>

namespace Ui {
class NewProjectDialog;
}

class NewProjectDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit NewProjectDialog(QWidget *parent = 0);
    ~NewProjectDialog();

    int beatCount;
    int unitCount;
    int bpm;

public slots:
    void setBeat(int);
    void setUnit(int);
    void setBPM(int);
private:
    Ui::NewProjectDialog *ui;
};

#endif // NEWPROJECTDIALOG_H
