#include "newprojectdialog.h"
#include "ui_newprojectdialog.h"

NewProjectDialog::NewProjectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewProjectDialog)
{
    ui->setupUi(this);
    beatCount = unitCount = 4;
    bpm = 120;
}

NewProjectDialog::~NewProjectDialog()
{
    delete ui;
}

void NewProjectDialog::setBeat(int _)
{
    beatCount = _;
}
void NewProjectDialog::setUnit(int _)
{
    unitCount = _;
}
void NewProjectDialog::setBPM(int _)
{
    bpm = _;
}
