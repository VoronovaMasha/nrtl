#include "transparencydialog.h"
#include "ui_transparencydialog.h"
TransparencyDialog::TransparencyDialog(int value,DataId id,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TransparencyDialog)
{
    ui->setupUi(this);
    ui->progressBar->setValue(value);
    ui->horizontalSlider->setValue(value);
    this->id=id;
}

TransparencyDialog::~TransparencyDialog()
{
    delete ui;
}

void TransparencyDialog::on_horizontalSlider_valueChanged(int value)
{
    ui->progressBar->setValue(value);
}

void TransparencyDialog::on_pushButton_clicked()
{
    emit new_value(ui->horizontalSlider->value(),id);
    close();
}
