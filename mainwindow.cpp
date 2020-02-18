#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QString>
#include <QFile>
#include <QFileDialog>
#include <QDir>
#include <QTextStream>
#include <QMessageBox>
#include <QDebug>
#include "MeshAlgorithm.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    model = nullptr;

    /** Create **/
    view = new QGraphicsView();
    scene = new QGraphicsScene();
    brush = new QBrush(QColor(0., 0., 0.));

    glWgt = new OGLAreaWidget();

    dock_Outliner = new QDockWidget("Outliner");

    dock_TractEditor = new QDockWidget("Tract Editor");

    menu = new QMenu("Menu");
    menu_Edit = new QMenu("Edit");
    menu_Tools = new QMenu("Tools");
    menu_Mode = new QMenu("Mode");
    menu_Window = new QMenu("Window");
    menu_About = new QMenu("About");

    act_Align = new QAction("Align");
    act_SelectSection = new QAction("Select Section");
    act_DrawPl = new QAction("Draw Polyline");
    act_ConnectBorders = new QAction("Connect Borders");
    act_Outliner = new QAction("Outiner");
    act_TractEd = new QAction("Tract Editor");
    act_NewDoc = new QAction("New");
    act_OpenDoc = new QAction("Open");
    act_SaveDoc = new QAction("Save");
    act_SaveDocAs = new QAction("Save As");
    act_ResetDoc = new QAction("Reset");
    act_Undo = new QAction("Undo");
    act_Redo = new QAction("Redo");
    act_ResMan = new QAction("Resource Manager");

    outlinerWgt = new OutlinerWidget();
    dummy_2 = new QWidget();

    res_man_wnd = new ResourceManager(this);
    res_man_wnd->close();

    /** Setup **/
    this->setWindowTitle("Nerve Tracts Lab");

    scene->setBackgroundBrush(*brush);
    act_Align->setIcon(QIcon(":/img/icons/icon_align.png"));
    act_SelectSection->setIcon(QIcon(":/img/icons/icon_cut_cube.png"));
    act_DrawPl->setIcon(QIcon(":/img/icons/icon_create_polyline.png"));
    act_ConnectBorders->setIcon(QIcon(":/img/icons/icon_connect_borders.png"));

    act_NewDoc->setIcon(QIcon(":/img/icons/icon_new_document.png"));
    act_OpenDoc->setIcon(QIcon(":/img/icons/icon_open.png"));
    act_SaveDoc->setIcon(QIcon(":/img/icons/icon_save.png"));
    act_SaveDocAs->setIcon(QIcon(":/img/icons/icon_save_as.png"));
    act_ResetDoc->setIcon(QIcon(":/img/icons/icon_reset.png"));
    act_Undo->setIcon(QIcon(":/img/icons/icon_undo.png"));
    act_Redo->setIcon(QIcon(":/img/icons/icon_redo.png"));

//    act_Align->setDisabled(true);
    act_SelectSection->setDisabled(true);
    act_DrawPl->setDisabled(true);
    act_ConnectBorders->setDisabled(false);

    ui->toolBar->setMovable(false);
    ui->toolBar->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonIconOnly);

    dock_Outliner->setWidget(outlinerWgt);
    dock_TractEditor->setWidget(dummy_2);

    dock_Outliner->setWindowFlags( dock_Outliner->windowFlags() | Qt::WindowStaysOnTopHint | Qt::SubWindow );
    dock_TractEditor->setWindowFlags( dock_TractEditor->windowFlags() | Qt::WindowStaysOnTopHint | Qt::SubWindow );

    dock_Outliner->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    dock_TractEditor->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    dock_Outliner->setMinimumWidth(300);
    dock_TractEditor->setMinimumWidth(300);

    dock_Outliner->setMaximumWidth(350);
    dock_TractEditor->setMaximumWidth(350);

    /** Connect **/

    connect(act_OpenDoc, &QAction::triggered, this, &MainWindow::on_OpenAction_clicked);
    connect(outlinerWgt, SIGNAL(need_update()), glWgt, SLOT(update()));
    connect(dock_Outliner,SIGNAL(close()),this,SLOT(hideOutliner()));
    connect(act_Align, &QAction::triggered, this, &MainWindow::on_AlignAction_clicked);

    connect(act_ResMan, &QAction::triggered, res_man_wnd, &ResourceManager::show);

    /** Place **/
    menu->addAction(act_NewDoc);
    menu->addAction(act_OpenDoc);
    menu->addAction(act_SaveDoc);
    menu->addAction(act_SaveDocAs);
    menu->addAction(act_ResetDoc);

    menu_Edit->addAction(act_Undo);
    menu_Edit->addAction(act_Redo);

    menu_Tools->addAction(act_Align);
    menu_Tools->addAction(act_SelectSection);
    menu_Tools->addAction(act_DrawPl);
    menu_Tools->addSeparator();
    menu_Tools->addAction(act_ConnectBorders);

    menu_Mode->addAction(act_Outliner);
    menu_Mode->addAction(act_TractEd);

    menu_Window->addAction(act_ResMan);

    ui->menubar->addMenu(menu);
    ui->menubar->addMenu(menu_Edit);
    ui->menubar->addMenu(menu_Tools);
    ui->menubar->addMenu(menu_Mode);
    ui->menubar->addMenu(menu_Window);
    ui->menubar->addMenu(menu_About);

    ui->toolBar->addAction(act_NewDoc);
    ui->toolBar->addAction(act_OpenDoc);
    ui->toolBar->addAction(act_SaveDoc);
    ui->toolBar->addAction(act_SaveDocAs);
    ui->toolBar->addAction(act_ResetDoc);
    ui->toolBar->addAction(act_Undo);
    ui->toolBar->addAction(act_Redo);

    ui->toolBar->addSeparator();

    ui->toolBar->addAction(act_Align);
    ui->toolBar->addAction(act_SelectSection);
    ui->toolBar->addAction(act_DrawPl);

    ui->toolBar->addSeparator();

    ui->toolBar->addAction(act_ConnectBorders);

    view->setScene(scene);
    this->setCentralWidget(glWgt);
    this->addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, dock_Outliner);
    this->addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, dock_TractEditor);
}

MainWindow::~MainWindow()
{
    if(model != nullptr)
        delete model;
    delete ui;
}

void MainWindow::setModel(NrtlModel* md)
{
    if(model != nullptr)
        delete model;
    model = md;
}


void MainWindow::on_OpenAction_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this,
                       tr("Open Mesh"), QDir::currentPath(),
                       tr("Model File(*.obj)"));
    if(!filename.isEmpty())
    {
        MeshModel* md = MeshModelLoader::OBJ::loadMesh(filename);
        if(md != nullptr)
        {
            QFileInfo info(filename);
            NrtlManager::createTransaction(NrtlManager::SYNC);
            if(ROutlinerData::WorkingStep::get()==NONE)
                outlinerWgt->addMainModel(md,info.fileName());
            else
                outlinerWgt->addCut(md,info.fileName());
            NrtlManager::commitTransaction();
        }
        else
        {
            QMessageBox::warning(this, "Warning", MeshModelLoader::errorString());
        }
    }
}

void MainWindow::on_AlignAction_clicked()
{

}


/*void MainWindow::on_OpenAction_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this,
                       tr("Open Mesh"), QDir::currentPath(),
                       tr("Model File(*.obj)"));
    if(!filename.isEmpty())
    {
        MeshModel* md = MeshModelLoader::OBJ::loadMesh(filename);
        if(md != nullptr)
        {
            NrtlManager::createTransaction(NrtlManager::SYNC);
            DataId id = RMeshModel::create(md);
            NrtlManager::commitTransaction();
            glWgt->update();
            Q_UNUSED(id)
        }
        else
        {
            QMessageBox::warning(this, "Warning", MeshModelLoader::errorString());
        }
    }
}*/
