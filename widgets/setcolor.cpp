#include "setcolor.h"
#include "ui_setcolor.h"
#include<QColorDialog>
#include <QMessageBox>

SetColor::SetColor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetColor)
{
    ui->setupUi(this);
}

SetColor::~SetColor()
{
    delete ui;
}


//кнопка close
void SetColor::on_pushButton_3_clicked()
{
    this->close();
}


//кнопка choose a color
void SetColor::on_pushButton_2_clicked()
{
    clr = QColorDialog::getColor();
}


//кнопка OK
void SetColor::on_pushButton_clicked()
{//    if(ui->labelR->text() == "-"){
    if(!clr.isValid())
    {
        QMessageBox::information(this,"Warning","You dont choose a color");
    }
    else if(ui->lineEdit->text() == ""){
        QMessageBox::information(this,"Warning","You dont enter a name");
    }
    else{
        this->close();
        emit info_about_all(clr, ui->lineEdit->text());
    }
}
