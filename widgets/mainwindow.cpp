#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QString>
#include <QFile>
#include <QFileDialog>
#include <QDir>
#include <QTextStream>
#include <QMatrix4x4>
#include <QMessageBox>
#include <QDebug>
#include <QLabel>
#include "MeshAlgorithm.h"
#include "allignwindow.h"


#define MULTITHREADING 1


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    model = nullptr;

    /** Create **/

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
    act_SaveSection = new QAction("Save Section");
    act_DrawPl = new QAction("Draw Polyline");
    act_ConnectBorders = new QAction("Connect Borders");
    act_Outliner = new QAction("Outiner");
    act_TractEd = new QAction("Tract Viewer");
    act_NewDoc = new QAction("New");
    act_LoadObj = new QAction("Load");
    act_LoadDoc = new QAction("Load Doc");
    act_SaveDocAs = new QAction("Save As");
    act_ResetDoc = new QAction("Reset");
    act_Undo = new QAction("Undo");
    act_Redo = new QAction("Redo");
    act_ResMan = new QAction("Resource Manager");

    outlinerWgt = new OutlinerWidget();
    tractWgt = new TractWidget();

    res_man_wnd = new ResourceManager(this);
    res_man_wnd->close();

    settings = new QSettings("Higher School of Economics", "Nerve Tracts Lab", this);

    tLoader = new QThread();
    tListener = new QThread();

    statusWgt = new QWidget(this);
    statusLbl = new QLabel("Loading...");
    progressBar = new QProgressBar();
    QVBoxLayout* lout = new QVBoxLayout();

    grWgt = new GroupEditorWidget(this);


    /** Setup **/
    this->setWindowTitle("Nerve Tracts Lab");

    act_Align->setIcon(QIcon(":/img/icons/icon_align.png"));
//    act_SelectSection->setIcon(QIcon(":/img/icons/icon_cut_cube.png"));
    act_SelectSection->setIcon(QIcon(":/img/icons/icon_start_cutting.png"));
    act_SaveSection->setIcon(QIcon(":/img/icons/icon_save_section.png"));
    act_DrawPl->setIcon(QIcon(":/img/icons/icon_create_polyline.png"));
    act_ConnectBorders->setIcon(QIcon(":/img/icons/icon_connect_borders.png"));

    act_NewDoc->setIcon(QIcon(":/img/icons/icon_new_document.png"));
    act_LoadObj->setIcon(QIcon(":/img/icons/icon_open.png"));
    act_LoadDoc->setIcon(QIcon(":/img/icons/icon_save.png"));
    act_SaveDocAs->setIcon(QIcon(":/img/icons/icon_save_as.png"));
    act_ResetDoc->setIcon(QIcon(":/img/icons/icon_reset.png"));
    act_Undo->setIcon(QIcon(":/img/icons/icon_undo.png"));
    act_Redo->setIcon(QIcon(":/img/icons/icon_redo.png"));

    act_DrawPl->setDisabled(true);
    act_ConnectBorders->setDisabled(false);

    ui->toolBar->setMovable(false);
    ui->toolBar->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonIconOnly);

    dock_Outliner->setWidget(outlinerWgt);
    dock_TractEditor->setWidget(tractWgt);

    dock_Outliner->setWindowFlags( dock_Outliner->windowFlags() | Qt::WindowStaysOnTopHint | Qt::SubWindow );
    dock_TractEditor->setWindowFlags( dock_TractEditor->windowFlags() | Qt::WindowStaysOnTopHint | Qt::SubWindow );

    dock_Outliner->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    dock_TractEditor->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    dock_Outliner->setMinimumWidth(300);
    dock_TractEditor->setMinimumWidth(300);

    dock_Outliner->setMaximumWidth(350);
    dock_TractEditor->setMaximumWidth(350);

    outlinerWgt->setObjLoaderAction(act_LoadObj);

    statusWgt->setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::Dialog );
    statusWgt->setWindowModality(Qt::WindowModal);
    statusWgt->setWindowTitle("Loading...");
    statusWgt->resize(200, 100);
    progressBar->setMinimum(0);
    progressBar->setMaximum(100);

    grWgt->setWindowFlags(Qt::Window |
                          Qt::CustomizeWindowHint |
                          Qt::WindowTitleHint |
                          Qt::WindowCloseButtonHint |
                          Qt::WindowStaysOnTopHint |
                          Qt::Tool);
    grWgt->setGeometry(150, 200, 230, 400);

    /** Connect **/

    connect(act_LoadObj, &QAction::triggered, this, &MainWindow::on_OpenAction_clicked);
    connect(outlinerWgt, SIGNAL(need_update()), glWgt, SLOT(update()));
    connect(dock_Outliner,SIGNAL(close()),this,SLOT(hideOutliner()));
    connect(act_Align, &QAction::triggered, this, &MainWindow::on_AlignAction_clicked);
    connect(act_ResMan, &QAction::triggered, res_man_wnd, &ResourceManager::show);
    connect(act_SelectSection,SIGNAL(triggered()),this,SLOT(SelectSection_clicked()));
    connect(act_SaveSection,SIGNAL(triggered()),this,SLOT(SaveSection_clicked()));
    connect(act_SaveDocAs,SIGNAL(triggered()),this,SLOT(SaveDocAs_clicked()));
    connect(act_LoadDoc,SIGNAL(triggered()),this,SLOT(LoadDoc_clicked()));

    connect(tListener, &QThread::finished, [](){ qDebug() << "Listener Thread finished "; });
    connect(tLoader, &QThread::finished, [](){ qDebug() << "Loader Thread finished "; });

    connect(grWgt, &GroupEditorWidget::update_group_tree, outlinerWgt, &OutlinerWidget::update);
    connect(tractWgt, &TractWidget::update_tract_tree, outlinerWgt, &OutlinerWidget::update);
    connect(tractWgt, &TractWidget::update_tract_tree, glWgt, &OGLAreaWidget::updater);

    /** Place **/
    menu->addAction(act_NewDoc);
    menu->addAction(act_LoadObj);
    menu->addAction(act_LoadDoc);
    menu->addAction(act_SaveDocAs);
    menu->addAction(act_ResetDoc);

    menu_Edit->addAction(act_Undo);
    menu_Edit->addAction(act_Redo);

    menu_Tools->addAction(act_Align);
    menu_Tools->addAction(act_SelectSection);
    menu_Tools->addAction(act_SaveSection);
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
    ui->toolBar->addAction(act_LoadObj);
    ui->toolBar->addAction(act_LoadDoc);
    ui->toolBar->addAction(act_SaveDocAs);
    ui->toolBar->addAction(act_ResetDoc);
    ui->toolBar->addAction(act_Undo);
    ui->toolBar->addAction(act_Redo);

    ui->toolBar->addSeparator();

    ui->toolBar->addAction(act_Align);
    ui->toolBar->addAction(act_SelectSection);
    ui->toolBar->addAction(act_SaveSection);
    ui->toolBar->addAction(act_DrawPl);

    ui->toolBar->addSeparator();

    ui->toolBar->addAction(act_ConnectBorders);

    lout->addWidget(statusLbl, Qt::AlignCenter);
    lout->addWidget(progressBar);
    statusWgt->setLayout(lout);

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
    QString prev_dir_path = settings->value(NrtlSettings::PREV_DIR_VAR, QDir::homePath()).toString();
    QString filename = QFileDialog::getOpenFileName(this,
                       tr("Open Mesh"), prev_dir_path,
                       tr("Model File(*.obj)"));

    if(!filename.isEmpty())
    {
        QDir prev_dir;
        settings->setValue(NrtlSettings::PREV_DIR_VAR, prev_dir.absoluteFilePath(filename));
    }
    if(!filename.isEmpty())
    {
        MeshModelLoader::setPath(filename);
#if MULTITHREADING==1
        loader = new MeshLoader();
        listener = new StatusListener(tLoader, MeshModelLoader::getProgress);

        loader->moveToThread(tLoader);
        listener->moveToThread(tListener);

        connect(tLoader, &QThread::finished,
                loader, &QObject::deleteLater);
        connect(tListener, &QThread::finished,
                listener, &QObject::deleteLater);

        connect(this, &MainWindow::startMeshLoading,
                loader, &MeshLoader::loadMesh);
        connect(loader, &MeshLoader::loadingFinished,
                this, &MainWindow::loadMeshSlot);

        connect(listener, &StatusListener::sendStatus,
                this, &MainWindow::updateProgressBar);

        statusWgt->show();
        tLoader->start();
        tListener->start();
        emit startMeshLoading();


#else
        MeshModelLoader::OBJ::loadMesh();

        MeshModel* md = MeshModelLoader::getMesh();

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
#endif
    }
}

void MainWindow::updateProgressBar(int val)
{
    progressBar->setValue(val);
}

void MainWindow::loadMeshSlot()
{
    tLoader->quit();
    tLoader->wait();

    tListener->quit();
    tListener->wait();

    statusWgt->close();

    MeshModel* md = MeshModelLoader::getMesh();

    if(md != nullptr)
    {
        QFileInfo info(MeshModelLoader::getPath());
        NrtlManager::createTransaction(NrtlManager::SYNC);
        if(ROutlinerData::WorkingStep::get()==NONE)
            outlinerWgt->addMainModel(md,info.fileName());
        else
            outlinerWgt->addCut(md,info.fileName());
        NrtlManager::commitTransaction();

        outlinerWgt->update();
    }
    else
    {
        QMessageBox::warning(this, "Warning", MeshModelLoader::errorString());
    }
}

void MainWindow::on_AlignAction_clicked()
{
    DataId id=ROutlinerData::MainMesh::get();
    if(id==NONE)
    {
        QMessageBox::warning(this, "Warning", "Before alligning load main mesh.");
        return;
    }
    id=ROutlinerData::WorkingStep::get();
    if(id==NONE)
    {
        QMessageBox::warning(this, "Warning", "Before alligning choose current step.");
        return;
    }
    id=RStep::MeshCut::get(ROutlinerData::WorkingStep::get());
    if(id==NONE)
    {
        QMessageBox::warning(this, "Warning", "Before alligning load cut to current step.");
        return;
    }
    AllignWindow *dialog=new AllignWindow(RMeshModel::getMeshData(ROutlinerData::MainMesh::get()),
                                          RMeshModel::getMeshData(RStep::MeshCut::get(ROutlinerData::WorkingStep::get())),
                                          ROutlinerData::MainMesh::get(),
                                          RStep::MeshCut::get(ROutlinerData::WorkingStep::get()));
    connect (dialog, SIGNAL (ok(QVector<QVector3D>,QVector<QVector3D>, DataId, DataId)),
             this, SLOT(alligning(QVector<QVector3D>,QVector<QVector3D>, DataId, DataId)));
    dialog->showMaximized();
}

void MainWindow::alligning(QVector<QVector3D> a, QVector<QVector3D> b, DataId mesh_id1, DataId mesh_id2)
{
    RMeshModel::align(a,b,mesh_id1,mesh_id2);
}

void MainWindow::SelectSection_clicked()
{
    DataId id=ROutlinerData::WorkingStep::get();
    if(id==NONE)
    {
        QMessageBox::warning(this, "Warning", "Before cutting choose current step.");
        return;
    }
    id=RStep::MeshCut::get(id);
    if(id==NONE)
    {
        QMessageBox::warning(this, "Warning", "Before cutting load cut to current step.");
        return;
    }
    if(RMeshModel::Transperancy::get(id)!=100)
    {
        QMessageBox::warning(this, "Warning", "Before cutting switch off transparency.");
        return;
    }

    /** Show group Widget here **/
    grWgt->show();
    glWgt->cutVertexes.clear();
    glWgt->tr_cutVertexes.clear();
    glWgt->cutFlag=1;
}

void MainWindow::SaveSection_clicked()
{
    grWgt->close();
    if(glWgt->cutFlag!=0)
    {
        glWgt->cutFlag=0;
        if(ROutlinerData::WorkingStep::get()!=NONE && RStep::MeshCut::get(ROutlinerData::WorkingStep::get())!=NONE)
        {
            MeshCutter::ViewMatrix::set(glWgt->getViewMatrix());
            MeshCutter::CutVertexes::set(glWgt->cutVertexes,glWgt->tr_cutVertexes);
            MeshModel* section=MeshCutter::cutFromMesh(RStep::MeshCut::get(ROutlinerData::WorkingStep::get()));
            if(section!=nullptr)
            {
                NrtlManager::createTransaction(NrtlManager::SYNC);
                outlinerWgt->addNewSection(section, grWgt->getCurrentId());
                NrtlManager::commitTransaction();
                glWgt->update();
            }
            else
            {
                QMessageBox::warning(this, "Warning", MeshCutter::errorString());
            }
            glWgt->cutVertexes.clear();
            glWgt->tr_cutVertexes.clear();
        }
    }
}

void MainWindow::SaveDocAs_clicked()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("Save File"),
                               "untitled.nrtl",
                               tr("NRTL-Project (*.nrtl)"));
    ProjectOptions::Saver::saveProject(filename);
}

void MainWindow::LoadDoc_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this,
                       tr("Open NRTL-Project"), "/",
                       tr("NRTL-File(*.nrtl)"));
    try
    {
        if(ProjectOptions::Loader::loadProject(filename))
        {
            outlinerWgt->update();
            grWgt->rewrite();
            glWgt->update();
            tractWgt->update();
        }
    }
    catch (...)
    {
        NrtlManager::restart();
        QMessageBox::warning(this, "Warning", "Error. Please, choose correct file.");
    }
}
/*void RTractM::LtSurface::create()
{
    if(RSectionModel::Border::get(2).size()==0)
    {
        PolygonMatrix m=MeshTopology::makePolygonMatrix(MeshData::get().getElement(2));
        RSectionModel::MatrixOfPolygons::set(2,m);
        MeshBorder b=MeshTopology::makeBorder(MeshData::get().getElement(2));
        if(b.size()==0)
        {
            QMessageBox::warning(this, "Warning", MeshTopology::errorString());
        }
        else
            RSectionModel::Border::set(2,b);
    }
    if(RSectionModel::Border::get(3).size()==0)
    {
        PolygonMatrix m=MeshTopology::makePolygonMatrix(MeshData::get().getElement(3));
        RSectionModel::MatrixOfPolygons::set(3,m);
        MeshBorder b=MeshTopology::makeBorder(MeshData::get().getElement(3));
        if(b.size()==0)
        {
            QMessageBox::warning(this, "Warning", MeshTopology::errorString());
        }
        else
            RSectionModel::Border::set(3,b);
    }
    MeshModel* mesh=MeshTopology::makeSurface(MeshData::get().getElement(2),MeshData::get().getElement(3));
    if(mesh!=nullptr)
    {
        NrtlManager::createTransaction(NrtlManager::SYNC);
        outlinerWgt->addMainModel(mesh,"fsdc");
        NrtlManager::commitTransaction();
    }
}*/
