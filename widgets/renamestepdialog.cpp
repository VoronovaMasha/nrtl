#include "renamestepdialog.h"
#include "ui_dialog.h"
#include<QMessageBox>
#include<QContextMenuEvent>

RenameStepDialog::RenameStepDialog(QString s) :
    ui(new Ui::RenameStepDialog)
{
    ui->setupUi(this);
    ui->lineEdit->setText(s);
}

RenameStepDialog::~RenameStepDialog()
{
    delete ui;
}

void RenameStepDialog::on_pushButton_clicked()
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
