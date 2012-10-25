#include "exportaudiodialog.h"
#include "ui_exportaudiodialog.h"
#include <QFileDialog>

ExportAudioDialog::ExportAudioDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExportAudioDialog)
{
    ui->setupUi(this);
}

ExportAudioDialog::~ExportAudioDialog()
{
    delete ui;
}

QString ExportAudioDialog::filename()
{
    return ui->lineEdit->text();
}
bool ExportAudioDialog::isSong()
{
    return ui->songButton->isChecked();
}
int ExportAudioDialog::fromBar()
{
    return ui->songFrom->value();
}
int ExportAudioDialog::toBar()
{
    return ui->songTo->value();
}
int ExportAudioDialog::loopCount()
{
    return ui->loopCount->value();
}

void ExportAudioDialog::browseFile()
{
    ui->lineEdit->setText(QFileDialog::getSaveFileName(this,"Export to...","","*.wav"));
}
