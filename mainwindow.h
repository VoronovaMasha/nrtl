#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include<QMainWindow>
#include<QGraphicsView>
#include<QDockWidget>
#include<QMenu>
#include<QMenuBar>
#include<QToolBar>
#include<QAction>
#include<QBrush>
#include<QOpenGLWidget>
#include<oglareawidget.h>

#include"outlinerwidget.h"
#include"resourcemanager.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setModel(NrtlModel*);

private slots:
    void on_OpenAction_clicked();
    void on_AlignAction_clicked();
    void alligning(QVector<QVector3D> a,QVector<QVector3D> b, DataId mesh_id1, DataId mesh_id2);

private:
    Ui::MainWindow *ui;

    QGraphicsView* view;
    QGraphicsScene* scene;
    QBrush* brush;

    OGLAreaWidget* glWgt;

    QDockWidget* dock_Outliner;
    QDockWidget* dock_TractEditor;

    QMenu* menu;
    QMenu* menu_Edit;
    QMenu* menu_Tools;
    QMenu* menu_Mode;
    QMenu* menu_Window;
    QMenu* menu_About;

    QAction* act_Align;
    QAction* act_SelectSection;
    QAction* act_DrawPl;
    QAction* act_ConnectBorders;
    QAction* act_Outliner;
    QAction* act_TractEd;
    QAction* act_NewDoc;
    QAction* act_LoadObj;
    QAction* act_SaveDoc;
    QAction* act_SaveDocAs;
    QAction* act_ResetDoc;
    QAction* act_Undo;
    QAction* act_Redo;
    QAction* act_ResMan;

    OutlinerWidget* outlinerWgt;
    QWidget* dummy_2;

    NrtlModel* model;

    ResourceManager* res_man_wnd;

};
#endif // MAINWINDOW_H
