#include "outlinerwidget.h"
#include "outliner.h"
#include <QDockWidget>
#include <QtDebug>
#include <QAction>
#include <QMenu>
#include <QFile>
#include <QFileDialog>
#include <QDir>
#include <QTextStream>
#include <QMessageBox>
#include <vector>
#include "renamestepdialog.h"
#include "NrtlType.h"
#include "MeshAlgorithm.h"
#include "transparencydialog.h"

OutlinerWidget::OutlinerWidget()
{
    lout = new QVBoxLayout(this);
    tree = new QTreeWidget();
    addStepBtn = new QPushButton("Add step");
//    tree->header()->hide();
    tree->setColumnCount(4);
    tree->setHeaderLabels({"", "Mesh", "Group", "Color"});
    tree->header()->setStretchLastSection(false);
    tree->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    tree->setContextMenuPolicy(Qt::CustomContextMenu);
    tree->clear();

    connect(addStepBtn, &QPushButton::clicked,
            this, &OutlinerWidget::add_step);

    connect(tree, &QTreeWidget::customContextMenuRequested,
            this, &OutlinerWidget::slotCustomMenuRequested);

    connect(tree, &QTreeWidget::itemDoubleClicked,
            this, &OutlinerWidget::itemDoubleClickedSlot);

    lout->addWidget(tree);
    lout->addWidget(addStepBtn);

    this->update();
}

void OutlinerWidget::addMainModel(MeshModel *mesh,QString name)
{
    DataId old_id=ROutlinerData::MainMesh::get();
    if(old_id!=NONE)
        RMeshModel::deleteMesh(old_id);
    DataId id=RMeshModel::create(mesh);
    RMeshModel::Name::set(id,name);
    ROutlinerData::MainMesh::set(id);
    ROutlinerData::WorkingStep::set(NONE);
    update();
}
void OutlinerWidget::addCut(MeshModel *mesh,QString name)
{
    DataId old_id=RStep::MeshCut::get(ROutlinerData::WorkingStep::get());
    if(old_id!=NONE)
        RMeshModel::deleteMesh(old_id);
    DataId id=RMeshModel::create(mesh);
    RMeshModel::Name::set(id,name);
    RStep::MeshCut::set(ROutlinerData::WorkingStep::get(),id);
    update();
}

void OutlinerWidget::update()
{
    int col = 1;
    tree->clear();
    MainMeshItem* mainMesh= new MainMeshItem();

    DataId mainMeshId=ROutlinerData::MainMesh::get();
    QString name;

    if(mainMeshId!=NONE)
        name=RMeshModel::Name::get(mainMeshId);
    else
        name="No main mesh";
    mainMesh->setText(col, name);
    mainMesh->id = mainMeshId;
    tree->addTopLevelItem(mainMesh);

    DataId current_step=ROutlinerData::WorkingStep::get();
    RStepList stepList=ROutlinerData::StepList::get();
    for(DataId step_id : ROutlinerData::StepList::get())
    {
        StepItem *st = new StepItem();
        QString name_step = RStep::Name::get(step_id);
        if(step_id == current_step) name_step+=QString(" (current)");
        st->id = step_id;
        st->setText(col, name_step);
        CutItem* cti = new CutItem();
        cti->id = RStep::MeshCut::get(step_id);
        if(cti->id == NONE)
        {
            cti->setText(col, "No cut");
        }
        else
        {
            cti->setText(col, RMeshModel::Name::get(cti->id));
        }
        st->addChild(cti);

        NrtlLabel* nlbl = new NrtlLabel();
        nlbl->setText(col, "Sections");
        st->addChild(nlbl);

        ResourceList sectionList=RStep::SectionList::get(step_id);

        SectionItem* sni = nullptr;

        if(sectionList.size() == 0)
        {
            sni = new SectionItem();
            sni->setText(col, "    No Sections");
            sni->id = NONE;
            st->addChild(sni);
        }
        else
        {
            for(DataId sec_id : sectionList)
            {
                sni = new SectionItem();
                sni->id = sec_id;
                QString section_name=QString("    ") +
                        RMeshModel::Name::get(sec_id);
                sni->setText(col, section_name);

                IGroupId gid = RSectionModel::GroupId::get(sec_id);
                QString group_name = gid == NONE ? "no group" : gid._name;
                sni->setText(col+1, group_name);
                if(gid._id != NONE)
                {
                    sni->setIcon(col+2, makeIcon(gid._color));
                }

                nlbl->addChild(sni);
            }
        }

        tree->addTopLevelItem(st);
        if(step_id == current_step)
        {
            tree->expandItem(st);
            st->setExpanded(true);
            nlbl->setExpanded(true);
        }
    }
    emit need_update();
}

void OutlinerWidget::add_step()
{
    DataId id = RStep::create("Step " +
                              QString::number(ROutlinerData::StepList::get().size()));
    ROutlinerData::StepList::add(id);
    ROutlinerData::WorkingStep::set(id);
    emit act_loadObj->triggered();
    update();
}

void OutlinerWidget::slotCustomMenuRequested(QPoint pos)
{
    showContextMenu(tree->currentItem(), pos);
}

void OutlinerWidget::showContextMenu(QTreeWidgetItem* item, const QPoint& globalPos)
{
    QMenu * menu = new QMenu(this);

    MainMeshItem* mmi = nullptr;
    CutItem* cti = nullptr;
    SectionItem* sni = nullptr;
    StepItem* spi = nullptr;

    mmi = dynamic_cast<MainMeshItem*>(item);

    if(mmi != nullptr)
    {
        if(act_loadObj == nullptr)
        {
            act_loadObj = new QAction("Load", this);
            act_loadObj->setDisabled(true);
        }
//        menu->addAction(act_loadObj);
        if(mmi->id != NONE)
        {
            QAction * addDevice = new QAction("Rename", this);
            QString vis = "Make ";
            vis += RMeshModel::Visibility::get(mmi->id) ? "invisible" : "visible";
            QAction * makeVisible = new QAction(vis, this);
            QAction * deleteMainMesh = new QAction("Delete", this);
            QAction * act_changeTransparency = new QAction("Change Transparency", this);
            menu->addAction(addDevice);
            menu->addAction(makeVisible);
            menu->addAction(deleteMainMesh);
            menu->addAction(act_changeTransparency);
            connect(addDevice, SIGNAL(triggered()), this, SLOT(rename()));
            connect(makeVisible, SIGNAL(triggered()), this, SLOT(makeMeshVisible()));
            connect(deleteMainMesh, SIGNAL(triggered()), this, SLOT(deleteMesh()));
            connect(act_changeTransparency, SIGNAL(triggered()), this, SLOT(changeTransparency()));
        }
        menu->popup(tree->viewport()->mapToGlobal(globalPos));

        return;
    }

    spi = dynamic_cast<StepItem*>(item);

    if(spi != nullptr)
    {
        QAction * addDevice = new QAction("Rename step", this);
        QAction * makeCurrent = new QAction("Choose as a current", this);
        menu->addAction(addDevice);
        menu->addAction(makeCurrent);
        menu->popup(tree->viewport()->mapToGlobal(globalPos));
        connect(addDevice, SIGNAL(triggered()), this, SLOT(rename()));
        connect(makeCurrent, &QAction::triggered, [spi, this](){
            ROutlinerData::WorkingStep::set(spi->id);
            this->update();
        });

        return;
    }

    cti = dynamic_cast<CutItem*>(item);

    if(cti != nullptr)
    {
        if(cti->id != NONE)
        {
            QAction * renameCutDevice = new QAction("Rename", this);
            QString vis = "Make ";
            vis += RMeshModel::Visibility::get(cti->id) ? "invisible" : "visible";
            QAction * makeVisible = new QAction(vis, this);
            QAction * deleteCutDevice = new QAction("Delete", this);
            QAction * act_changeTransparency = new QAction("Change Transparency", this);
            menu->addAction(renameCutDevice);
            menu->addAction(makeVisible);
            menu->addAction(deleteCutDevice);
            menu->addAction(act_changeTransparency);
            menu->popup(tree->viewport()->mapToGlobal(globalPos));
            connect(renameCutDevice, SIGNAL(triggered()), this, SLOT(rename()));
            connect(makeVisible, SIGNAL(triggered()), this, SLOT(makeMeshVisible()));
            connect(deleteCutDevice, SIGNAL(triggered()), this, SLOT(deleteMesh()));
            connect(act_changeTransparency, SIGNAL(triggered()), this, SLOT(changeTransparency()));
        }

        return;
    }

    sni = dynamic_cast<SectionItem*>(item);

    if(sni != nullptr)
    {
        if(sni->id != NONE)
        {
            QAction * renameSectionDevice = new QAction("Rename", this);
            QString vis = "Make ";
            vis += RMeshModel::Visibility::get(sni->id) ? "invisible" : "visible";
            QAction* makeVisible = new QAction(vis, this);
            QAction * deleteSectionDevice = new QAction("Delete", this);
            QAction * act_changeTransparency = new QAction("Change Transparency", this);
            menu->addAction(renameSectionDevice);
            menu->addAction(makeVisible);
            menu->addAction(deleteSectionDevice);
            menu->addAction(act_changeTransparency);
            menu->popup(tree->viewport()->mapToGlobal(globalPos));
            connect(renameSectionDevice, SIGNAL(triggered()), this, SLOT(rename()));
            connect(deleteSectionDevice, SIGNAL(triggered()), this, SLOT(deleteMesh()));
            connect(act_changeTransparency, SIGNAL(triggered()), this, SLOT(changeTransparency()));
            connect(makeVisible, SIGNAL(triggered()), this, SLOT(makeMeshVisible()));
        }
    }
}

void OutlinerWidget::addNewSection(MeshModel* mesh, IGroupId gid)
{
    if(ROutlinerData::WorkingStep::get()==NONE)
    {
        QMessageBox::warning(this, "Warning", "You have not chosen current step.");
        return;
    }
    DataId id=RSectionModel::create(mesh);
    ResourceList secLst=RStep::SectionList::get(ROutlinerData::WorkingStep::get());
    bool flag=false;
    DataId prev_id=NONE;
    for(unsigned int i=0;i<secLst.size();i++)
    {
        if(RSectionModel::GroupId::get(secLst[i])==gid)
        {
            flag=true;
            prev_id=secLst[i];
        }
    }
    if(flag && prev_id!=NONE )
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Question",
                                      "Do you want to converge sections? (Yes - to converge, No - to replace previous section)",
                                        QMessageBox::Yes|QMessageBox::No);
          if (reply == QMessageBox::Yes)
          {
              MeshModel* obj=RSectionModel::Converge::convergesections(prev_id,id);
              DataId new_id=RSectionModel::create(obj);
              RSectionModel::Name::set(new_id,RSectionModel::Name::get(prev_id));
              RStep::SectionList::remove(ROutlinerData::WorkingStep::get(),prev_id);
              RMeshModel::deleteMesh(prev_id);
              RMeshModel::deleteMesh(id);
              RStep::SectionList::add(ROutlinerData::WorkingStep::get(),new_id);
              RSectionModel::GroupId::set(new_id, gid);
              RMeshModel::Visibility::set(new_id, true);
          }
          else
          {
              RStep::SectionList::remove(ROutlinerData::WorkingStep::get(),prev_id);
              RMeshModel::deleteMesh(prev_id);
              RStep::SectionList::add(ROutlinerData::WorkingStep::get(),id);
              RSectionModel::Name::set(id,QString("Section"));
              RSectionModel::GroupId::set(id, gid);
              RMeshModel::Visibility::set(id, true);
          }
    }
    else
    {
        RSectionModel::Name::set(id,QString("Section"));
        RStep::SectionList::add(ROutlinerData::WorkingStep::get(),id);
        RSectionModel::GroupId::set(id, gid);
        RMeshModel::Visibility::set(id, true);
    }
    update();
}

void OutlinerWidget::rename()
{    
    QString s=tree->currentItem()->text(0);
    if(s.endsWith(QString("(current)")))
        s.remove(s.length()-10,10);
    RenameStepDialog *d1 = new RenameStepDialog(s);
    connect (d1, SIGNAL (send_name(QString)),
             this, SLOT(change(QString)));
    d1->exec();
}

void OutlinerWidget::deleteMesh()
{
    QTreeWidgetItem *item = tree->currentItem();

    MainMeshItem* mmi = nullptr;
    CutItem* cti = nullptr;
    SectionItem* sni = nullptr;

    mmi = dynamic_cast<MainMeshItem*>(item);
    if(mmi != nullptr)
    {
        RMeshModel::deleteMesh(mmi->id);
        ROutlinerData::MainMesh::set(NONE);
        update();
        return;
    }

    cti = dynamic_cast<CutItem*>(item);
    if(cti != nullptr)
    {
        RMeshModel::deleteMesh(cti->id);
        RStep::MeshCut::set(cti->id, NONE);
        update();
        return;
    }

    sni = dynamic_cast<SectionItem*>(item);
    if(sni != nullptr)
    {
        RStep::SectionList::remove(RSectionModel::Step::get(sni->id),
                                   sni->id);
        RMeshModel::deleteMesh(sni->id);
        update();
        return;
    }
    update();
}

void OutlinerWidget::change(QString s)
{
    QTreeWidgetItem *item = tree->currentItem();

    MainMeshItem* mmi = nullptr;
    CutItem* cti = nullptr;
    SectionItem* sni = nullptr;
    StepItem* spi = nullptr;

    mmi = dynamic_cast<MainMeshItem*>(item);
    if(mmi != nullptr)
    {
        RMeshModel::Name::set(mmi->id, s);
        update();
        return;
    }

    spi = dynamic_cast<StepItem*>(item);
    if(spi != nullptr)
    {
        RStep::Name::set(spi->id, s);
        update();
        return;
    }

    cti = dynamic_cast<CutItem*>(item);
    if(cti != nullptr)
    {
        RMeshModel::Name::set(cti->id, s);
        update();
        return;
    }

    sni = dynamic_cast<SectionItem*>(item);
    if(sni != nullptr)
    {
        RSectionModel::Name::set(sni->id, s);
        update();
        return;
    }
}

void OutlinerWidget::makeSectionVisible()
{
    SectionItem* sni = dynamic_cast<SectionItem*>(tree->currentItem());
    bool vis = false;
    if(sni != nullptr)
    {
        vis = RMeshModel::Visibility::get(sni->id);
        RMeshModel::Visibility::set(sni->id, !vis);
    }
    update();
}

void OutlinerWidget::makeMeshVisible()
{
    NrtlItem* nli = nullptr;

    nli = dynamic_cast<NrtlItem*>(tree->currentItem());
    bool vis = false;

    if(nli != nullptr)
    {
        vis = RMeshModel::Visibility::get(nli->id);
        RMeshModel::Visibility::set(nli->id, !vis);
    }
    update();
}

void OutlinerWidget::changeTransparency()
{
    NrtlItem* nli = nullptr;
    nli = dynamic_cast<NrtlItem*>(tree->currentItem());

    if(nli != nullptr)
    {
        TransparencyDialog* d =
                new TransparencyDialog(RMeshModel::Transperancy::get(nli->id), nli->id);
        connect(d, SIGNAL(new_value(int,DataId)), this, SLOT(setTransparency(int,DataId)));
        qDebug() << __func__;
        d->show();
    }
}

void OutlinerWidget::setTransparency(int value, DataId id)
{
    RMeshModel::Transperancy::set(id, value);
    emit need_update();
}

void OutlinerWidget::itemDoubleClickedSlot()
{
    MainMeshItem* mmi = nullptr;
    StepItem* sti = nullptr;

    mmi = dynamic_cast<MainMeshItem*>(this->tree->currentItem());
    if(mmi != nullptr)
    {
        ROutlinerData::WorkingStep::set(NONE);
        this->update();
        return;
    }
    sti = dynamic_cast<StepItem*>(this->tree->currentItem());
    if(sti != nullptr)
    {
        ROutlinerData::WorkingStep::set(sti->id);
        this->update();
        return;
    }
}
