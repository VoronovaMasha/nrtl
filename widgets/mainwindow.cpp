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

    scene->setBackgroundBrush(*brush);
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
    RMeshModel::align(mesh_id2,mesh_id1,b,a);
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
    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::warning(this, "Warning", "Error.");
        return;
    }
    QDataStream stream(&file);
    std::vector<DataId> modelIds=MeshData::getMeshIdList();
    stream<<modelIds.size();
    for(unsigned int i=0;i<modelIds.size();i++)
    {
        stream<<modelIds[i];
        stream<<RMeshModel::Visibility::get(modelIds[i]);
        stream<<RMeshModel::Name::get(modelIds[i]);
        stream<<MeshData::get().getElement(modelIds[i])->r;
        stream<<MeshData::get().getElement(modelIds[i])->g;
        stream<<MeshData::get().getElement(modelIds[i])->b;
        stream<<RMeshModel::Transperancy::get(modelIds[i]);
        stream<<RMeshModel::Step::get(modelIds[i]);
        stream<<RMeshModel::ModelMatrix::get(modelIds[i]);
        stream<<MeshData::get().getElement(modelIds[i])->polygons;
        stream<<MeshData::get().getElement(modelIds[i])->texpolygons;
        stream<<MeshData::get().getElement(modelIds[i])->coords;
        stream<<MeshData::get().getElement(modelIds[i])->texcoords;
        stream<<MeshData::get().getElement(modelIds[i])->image;
    }
    std::vector<IGroupId> groupLst;
    groupLst=ROutlinerData::GroupList::get();
    stream<<groupLst.size();
    for(unsigned int i=0;i<groupLst.size();i++)
    {
        stream<<groupLst[i]._id;
        stream<<GroupId::Name::get(groupLst[i]);
        stream<<GroupId::Color::get(groupLst[i]);
    }
    RStepList stepLst=ROutlinerData::StepList::get();
    stream<<stepLst.size();
    for(unsigned int i=0;i<stepLst.size();i++)
    {
        stream<<stepLst[i];
        stream<<RStep::Name::get(stepLst[i]);
        stream<<RStep::MeshCut::get(stepLst[i]);
        stream<<RStep::SectionList::get(stepLst[i]).size();
        for(unsigned int j=0;j<RStep::SectionList::get(stepLst[i]).size();j++)
            stream<<RStep::SectionList::get(stepLst[i])[j];
        std::unordered_map<DataId, IGroupId, DataHash> map1=NrtlManager::getStep(stepLst[i])->section_group_map;
        stream<<map1.size();
        for(auto it=map1.begin();it!=map1.end();it++)
        {
            stream<<it->first;
            stream<<(it->second)._id;
            stream<<(it->second)._name;
            stream<<(it->second)._color;
        }
        std::unordered_map<IGroupId, DataId, GroupHash> map2=NrtlManager::getStep(stepLst[i])->group_section_map;
        stream<<map2.size();
        for(auto it=map2.begin();it!=map2.end();it++)
        {
            stream<<(it->first)._id;
            stream<<(it->first)._name;
            stream<<(it->first)._color;
            stream<<it->second;
        }
    }
    stream<<ROutlinerData::MainMesh::get();
    stream<<ROutlinerData::WorkingStep::get();
    file.close();
}

void MainWindow::LoadDoc_clicked()
{
    QString prev_dir_path = settings->value(NrtlSettings::PREV_DIR_VAR, QDir::homePath()).toString();
    QString filename = QFileDialog::getOpenFileName(this,
                       tr("Open NRTL-Project"), prev_dir_path,
                       tr("NRTL-File(*.nrtl)"));
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(this, "Warning", "Can't open file.");
        return;
    }
    QDataStream stream(&file);
    unsigned int size;
    stream>>size;
    for(unsigned int i=0;i<size;i++)
    {
        DataId id;
        bool visibility;
        QString name;
        float r,g,b;
        uint8_t transparency;
        DataId step_id;
        QMatrix4x4 modelMatrix;
        QVector<QVector<unsigned int>> polygons;
        QVector<QVector<unsigned int>> texpolygons;
        QVector<QVector3D> coords;
        QVector<QVector2D> texcoords;
        QImage image;
        stream>>id;
        stream>>visibility;
        stream>>name;
        stream>>r;
        stream>>g;
        stream>>b;
        stream>>transparency;
        stream>>step_id;
        stream>>modelMatrix;
        stream>>polygons;
        stream>>texpolygons;
        stream>>coords;
        stream>>texcoords;
        stream>>image;
        QVector<VertexData> vertexes;
        QVector<GLuint> indexes;
        for(int j=0;j<polygons.size();j++)
        {
            if(texpolygons.size()!=0 && texcoords.size()!=0)
            {
                vertexes.append(VertexData(coords[polygons[j][0]-1],texcoords[texpolygons[j][0]-1],QVector3D(1.0,0.0,0.0)));
                vertexes.append(VertexData(coords[polygons[j][1]-1],texcoords[texpolygons[j][1]-1],QVector3D(1.0,0.0,0.0)));
                vertexes.append(VertexData(coords[polygons[j][2]-1],texcoords[texpolygons[j][2]-1],QVector3D(1.0,0.0,0.0)));
            }
            else
            {
                vertexes.append(VertexData(coords[polygons[j][0]-1],QVector2D(0.0,0.0),QVector3D(1.0,0.0,0.0)));
                vertexes.append(VertexData(coords[polygons[j][1]-1],QVector2D(0.0,1.0),QVector3D(1.0,0.0,0.0)));
                vertexes.append(VertexData(coords[polygons[j][2]-1],QVector2D(1.0,0.0),QVector3D(1.0,0.0,0.0)));
            }
            QVector3D a=vertexes[vertexes.size()-3].position;
            QVector3D b=vertexes[vertexes.size()-2].position;
            QVector3D c=vertexes[vertexes.size()-1].position;
            QVector3D n1=QVector3D(c.x()-a.x(),c.y()-a.y(),c.z()-a.z());
            QVector3D n2=QVector3D(b.x()-a.x(),b.y()-a.y(),b.z()-a.z());
            QVector3D n=QVector3D(n1.y()*n2.z()-n1.z()*n2.y(),n1.z()*n2.x()-n1.x()*n2.z(),n1.x()*n2.y()-n1.y()*n2.x());
            QVector3D norm=QVector3D(-n.x()/n.length(),-n.y()/n.length(),-n.z()/n.length());
            vertexes[vertexes.size()-3].normal=norm;
            vertexes[vertexes.size()-2].normal=norm;
            vertexes[vertexes.size()-1].normal=norm;
            indexes.append(indexes.size());
            indexes.append(indexes.size());
            indexes.append(indexes.size());
            if(texpolygons.size()!=0 && texcoords.size()!=0)
            {
                vertexes.append(VertexData(coords[polygons[j][2]-1],texcoords[texpolygons[j][2]-1],QVector3D(1.0,0.0,0.0)));
                vertexes.append(VertexData(coords[polygons[j][1]-1],texcoords[texpolygons[j][1]-1],QVector3D(1.0,0.0,0.0)));
                vertexes.append(VertexData(coords[polygons[j][0]-1],texcoords[texpolygons[j][0]-1],QVector3D(1.0,0.0,0.0)));
            }
            else
            {
                vertexes.append(VertexData(coords[polygons[j][2]-1],QVector2D(0.0,0.0),QVector3D(1.0,0.0,0.0)));
                vertexes.append(VertexData(coords[polygons[j][1]-1],QVector2D(0.0,1.0),QVector3D(1.0,0.0,0.0)));
                vertexes.append(VertexData(coords[polygons[j][0]-1],QVector2D(1.0,0.0),QVector3D(1.0,0.0,0.0)));
            }
            a=vertexes[vertexes.size()-3].position;
            b=vertexes[vertexes.size()-2].position;
            c=vertexes[vertexes.size()-1].position;
            n1=QVector3D(c.x()-a.x(),c.y()-a.y(),c.z()-a.z());
            n2=QVector3D(b.x()-a.x(),b.y()-a.y(),b.z()-a.z());
            n=QVector3D(n1.y()*n2.z()-n1.z()*n2.y(),n1.z()*n2.x()-n1.x()*n2.z(),n1.x()*n2.y()-n1.y()*n2.x());
            norm=QVector3D(-n.x()/n.length(),-n.y()/n.length(),-n.z()/n.length());
            vertexes[vertexes.size()-3].normal=norm;
            vertexes[vertexes.size()-2].normal=norm;
            vertexes[vertexes.size()-1].normal=norm;
            indexes.append(indexes.size());
            indexes.append(indexes.size());
            indexes.append(indexes.size());
        }
        MeshModel* obj=new MeshModel(vertexes,indexes,image);
        obj->polygons=polygons;
        obj->texpolygons=texpolygons;
        obj->coords=coords;
        obj->texcoords=texcoords;
        NrtlManager::createTransaction(NrtlManager::SYNC);
        DataId mesh_id=RMeshModel::create(obj);
        NrtlManager::commitTransaction();
        MeshData::get().changeId(mesh_id,id);
        MeshData::get().changeIdCounter(id+1);
        RMeshModel::Name::set(id,name);
        RMeshModel::Transperancy::set(id,transparency);
        RMeshModel::Visibility::set(id,visibility);
        RMeshModel::Step::set(id,step_id);
        MeshData::get().getElement(id)->setModelMatrix(modelMatrix);
        MeshData::get().getElement(id)->r=r;
        MeshData::get().getElement(id)->g=g;
        MeshData::get().getElement(id)->b=b;
    }
    stream>>size;
    std::vector<IGroupId> groupLst;
    for(unsigned int i=0;i<size;i++)
    {
        int id;
        QString name;
        QColor color;
        stream>>id;
        stream>>name;
        stream>>color;
        groupLst.push_back(IGroupId(id,name,color));
        ROutlinerData::GroupList::changeIdCounter(id+1);
    }
    ROutlinerData::GroupList::setGroupLstInManagers(groupLst);
    ROutlinerData::GroupList::setGroupLstInOutliner(groupLst);
    stream>>size;
    for(unsigned int i=0;i<size;i++)
    {
        DataId id;
        QString name;
        DataId meshCutId;
        unsigned int secSize;
        stream>>id;
        stream>>name;
        stream>>meshCutId;
        stream>>secSize;
        RSectionList meshSectionIds;
        for(unsigned int j=0;j<secSize;j++)
        {
            DataId secId;
            stream>>secId;
            meshSectionIds.push_back(secId);
        }
        std::unordered_map<DataId, IGroupId, DataHash> map1;
        unsigned int mapSize;
        stream>>mapSize;
        for(unsigned int j=0;j<mapSize;j++)
        {
            DataId mapSecId;
            int mapGroupId;
            QString mapGroupName;
            QColor mapGroupColor;
            stream>>mapSecId;
            stream>>mapGroupId;
            stream>>mapGroupName;
            stream>>mapGroupColor;
            map1[mapSecId]=IGroupId(mapGroupId,mapGroupName,mapGroupColor);
        }
        std::unordered_map<IGroupId, DataId, GroupHash> map2;
        stream>>mapSize;
        for(unsigned int j=0;j<mapSize;j++)
        {
            DataId mapSecId;
            int mapGroupId;
            QString mapGroupName;
            QColor mapGroupColor;
            stream>>mapGroupId;
            stream>>mapGroupName;
            stream>>mapGroupColor;
            stream>>mapSecId;
            map2[IGroupId(mapGroupId,mapGroupName,mapGroupColor)]=mapSecId;
        }
        DataId stepId=RStep::create(name);
        ROutlinerData::StepList::add(stepId);
        RStep::changeId(stepId,id);
        NrtlManager::getStep(id)->section_group_map=map1;
        NrtlManager::getStep(id)->group_section_map=map2;
        RStep::MeshCut::set(id,meshCutId);
        RStep::SectionList::set(id,meshSectionIds);
        RStep::changeStepIdCounter(id+1);
    }
    DataId mainMeshId;
    stream>>mainMeshId;
    ROutlinerData::MainMesh::set(mainMeshId);
    DataId workingStep;
    stream>>workingStep;
    ROutlinerData::WorkingStep::set(workingStep);
    outlinerWgt->update();
    grWgt->rewrite();
    glWgt->update();
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
