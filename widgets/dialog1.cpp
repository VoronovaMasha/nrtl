#include "dialog1.h"
#include "ui_dialog1.h"
#include<QMessageBox>
Dialog1::Dialog1(QString s) :
    ui(new Ui::Dialog1)
{
    ui->setupUi(this);
    ui->lineEdit->setText(s);
}

Dialog1::~Dialog1()
{
    delete ui;
}

void Dialog1::on_pushButton_clicked()
{
    QString s = ui->lineEdit->text();
    if(s == ""){
        QMessageBox::critical(this,"Error","You entered an empty string, please try again");
    }
    else{
        emit send_name(s);
        close();
    }
}
