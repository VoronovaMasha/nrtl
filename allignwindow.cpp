#include "allignwindow.h"
#include "ui_allignwindow.h"
#include <QMessageBox>
AllignWindow::AllignWindow(MeshModel *a, MeshModel *b,DataId c,DataId d,QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::AllignWindow)
{
    ui->setupUi(this);
    ui->openGLWidget->push(a);
    ui->openGLWidget_2->push(b);
    this->c=c;
    this->d=d;
}

AllignWindow::~AllignWindow()
{
    delete ui;
}

void AllignWindow::on_pushButton_clicked()
{
    if(ui->openGLWidget->cutVertexes.size()!=3 || ui->openGLWidget_2->cutVertexes.size()!=3)
    {
        QMessageBox::warning(this,"Warning","You must choose 3 vertexes in each model.");
        return;
    }
    emit ok(ui->openGLWidget->cutVertexes,ui->openGLWidget_2->cutVertexes,c,d);
    close();
}
