#ifndef ALLIGNWINDOW_H
#define ALLIGNWINDOW_H

#include <QMainWindow>
#include <QComboBox>
#include "meshmodel.h"
#include "NrtlType.h"
namespace Ui {
class AllignWindow;
}

class AllignWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit AllignWindow(MeshModel* a,MeshModel* b, DataId c,DataId d,QWidget *parent = nullptr);
    ~AllignWindow();
private slots:
    void on_pushButton_clicked();
signals:
    void ok(QVector<QVector3D>,QVector<QVector3D>,DataId c,DataId d);
private:
    Ui::AllignWindow *ui;
    DataId c,d;
};

#endif // ALLIGNWINDOW_H
