#include "outlinerwidget.h"
#include "outliner.h"
#include<QDockWidget>
#include<QtDebug>
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
    currentIt = nullptr;
    lout = new QVBoxLayout(this);
    tree = new QTreeWidget();
    tree->header()->hide();
    tree->clear();
    but_1 = new QPushButton("Add step");
    lout->addWidget(tree);
    lout->addWidget(but_1);
    connect(but_1,SIGNAL(clicked()),this,SLOT(add_step()));
    tree->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(tree, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotCustomMenuRequested(QPoint)));
    QString name_step = "No main mesh";
    mainMesh=new QTreeWidgetItem();
    mainMesh->setText(0,name_step);
    tree->addTopLevelItem(mainMesh);
}

void OutlinerWidget::addMainModel(MeshModel *mesh,QString name)
{
    DataId old_id=ROutlinerData::MainMesh::get();
    if(old_id!=NONE)
        RMeshModel::deleteMesh(old_id);
    DataId id=RMeshModel::create(mesh);
    RMeshModel::Name::set(id,name);
    ROutlinerData::MainMesh::set(id);
    RMeshModel::Visibility::makeVisibleOnlyOne(id);
    ROutlinerData::WorkingStep::set(NONE);
    update();
    emit need_update();
}

void OutlinerWidget::addCut(MeshModel *mesh,QString name)
{
    DataId old_id=RStep::MeshCut::get(ROutlinerData::WorkingStep::get());
    if(old_id!=NONE)
        RMeshModel::deleteMesh(old_id);
    DataId id=RMeshModel::create(mesh);
    RMeshModel::Name::set(id,name);
    RStep::MeshCut::set(ROutlinerData::WorkingStep::get(),id);
    RMeshModel::Visibility::makeVisibleOnlyOne(id);
    update();
    emit need_update();
}

void OutlinerWidget::update()
{
    tree->clear();
    v_steps.clear();
    how_many_step = 1;
    currentIt=nullptr;
    mainMesh=nullptr;
    DataId mainMeshId=ROutlinerData::MainMesh::get();
    QString name;
    DataId current_step=ROutlinerData::WorkingStep::get();
    if(mainMeshId!=NONE)
        name=RMeshModel::Name::get(mainMeshId);
    else
        name="No main mesh";
    mainMesh=new QTreeWidgetItem();
    mainMesh->setText(0,name);
    tree->addTopLevelItem(mainMesh);
    RStepList stepList=ROutlinerData::StepList::get();
    for(unsigned int i=0;i<stepList.size();i++)
    {
        QString name_step=RStep::Name::get(stepList[i]);
        if(stepList[i]==current_step)
            name_step+=QString(" (current)");
        Step *st = new Step();
        st->id=stepList[i];
        st->setText(0,name_step);
        DataId cut_id=RStep::MeshCut::get(stepList[i]);
        QString name_cut;
        if(cut_id!=NONE)
        {
            name_cut=RMeshModel::Name::get(cut_id);
            st->cut->setText(0,name_cut);
        }
        RSectionList sectionList=RStep::SectionList::get(stepList[i]);
        if(sectionList.size())
        {
            QString section_name=RMeshModel::Name::get(sectionList[0]);
            st->sections->child(0)->setText(0,section_name);
            st->v_section_in_sect[0]->id=sectionList[0];
            for(unsigned int j=1;j<sectionList.size();j++)
            {
                section_name=RMeshModel::Name::get(sectionList[j]);
                Section *child_for_section = new Section();
                child_for_section->setText(0,section_name);
                child_for_section->id=sectionList[j];
                st->sections->addChild(child_for_section);
                st->v_section_in_sect.push_back(child_for_section);
                st->how_many_section++;
            }
        }
        how_many_step++;
        v_steps.push_back(st);
        tree->addTopLevelItem(st);
    }
}

void OutlinerWidget :: add_step()
{
    DataId id=RStep::create("Step " + QString::number(how_many_step));
    ROutlinerData::StepList::add(id);
    ROutlinerData::WorkingStep::set(id);
    RMeshModel::Visibility::makeAllUnvisible();
    update();
    emit need_update();
}

void OutlinerWidget::slotCustomMenuRequested(QPoint pos)
{
    QTreeWidgetItem *itm = tree->currentItem();
    currentIt=itm;
    showContextMenu(currentIt,pos);
}

void OutlinerWidget :: showContextMenu(QTreeWidgetItem* item, const QPoint& globalPos)
{
    QMenu * menu = new QMenu(this);
    currentIt = item;
    if(item==mainMesh)
    {
        QAction * loadMainMesh = new QAction("Load", this);
        menu->addAction(loadMainMesh);
        if(item->text(0)!="No main mesh")
        {
            QAction * addDevice = new QAction("Rename", this);
            QAction * makeVisible = new QAction("Make Visible", this);
            QAction * deleteMainMesh = new QAction("Delete", this);
            QAction * act_changeTransparency = new QAction("Change Transparency", this);
            menu->addAction(addDevice);
            menu->addAction(makeVisible);
            menu->addAction(deleteMainMesh);
            menu->addAction(act_changeTransparency);
            connect(addDevice, SIGNAL(triggered()), this, SLOT(rename()));
            connect(makeVisible, SIGNAL(triggered()), this, SLOT(makeMainMeshVisible()));
            connect(deleteMainMesh, SIGNAL(triggered()), this, SLOT(deleteMesh()));
            connect(act_changeTransparency, SIGNAL(triggered()), this, SLOT(changeTransparency()));
        }
        connect(loadMainMesh, SIGNAL(triggered()), this, SLOT(loadMainMesh()));
        menu->popup(tree->viewport()->mapToGlobal(globalPos));
    }
    for(auto i = 0; i < v_steps.size(); i++)
    {
        Step *q = v_steps[i];
        if (item == q)
        {
            QAction * addDevice = new QAction("Rename step", this);
            QAction * makeCurrent = new QAction("Choose as a current", this);
            menu->addAction(addDevice);
            menu->addAction(makeCurrent);
            menu->popup(tree->viewport()->mapToGlobal(globalPos));
            connect(addDevice, SIGNAL(triggered()), this, SLOT(rename()));
            connect(makeCurrent, SIGNAL(triggered()), this, SLOT(make_step_current()));
            break;
        }
        else if(item == q->cut)
        {
            if(item->text(0)!="No cut")
            {
                QAction * renameCutDevice = new QAction("Rename", this);
                QAction * deleteCutDevice = new QAction("Delete", this);
                QAction * act_changeTransparency = new QAction("Change Transparency", this);
                menu->addAction(renameCutDevice);
                menu->addAction(deleteCutDevice);
                menu->addAction(act_changeTransparency);
                menu->popup(tree->viewport()->mapToGlobal(globalPos));
                connect(renameCutDevice, SIGNAL(triggered()), this, SLOT(rename()));
                connect(deleteCutDevice, SIGNAL(triggered()), this, SLOT(deleteMesh()));
                connect(act_changeTransparency, SIGNAL(triggered()), this, SLOT(changeTransparency()));
            }
            break;
        }
        else{
            for(auto j = 0; j<q->v_section_in_sect.size();j++)
            {
                if(item == q->v_section_in_sect[j])
                {
                    if(item->text(0)!="No sections")
                    {
                        QAction * renameSectionDevice = new QAction("Rename", this);
                        QAction * makeVisible = new QAction("Make Visible", this);
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
                        connect(makeVisible, SIGNAL(triggered()), this, SLOT(makeSectionVisible()));
                    }
                    break;
                }
            }
        }
    }
}

void OutlinerWidget :: addNewSection(MeshModel* mesh)
{
    if(ROutlinerData::WorkingStep::get()==NONE)
    {
        QMessageBox::warning(this, "Warning", "You have not chosen current step.");
        return;
    }
    DataId id=RMeshModel::create(mesh);
    RMeshModel::Name::set(id,QString("Section"));
    RStep::SectionList::add(ROutlinerData::WorkingStep::get(),id);
    RMeshModel::Visibility::makeVisibleOnlyOne(id);
    ROutlinerData::WorkingStep::set(NONE);
    update();
    emit need_update();
}

void OutlinerWidget :: rename()
{
    QString s=currentIt->text(0);
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
    if(item==mainMesh)
    {
        DataId id=ROutlinerData::MainMesh::get();
        RMeshModel::deleteMesh(id);
        ROutlinerData::MainMesh::set(NONE);
    }
    for(auto i = 0; i < v_steps.size(); i++)
    {
        Step *q = v_steps[i];
        if(item == q->cut)
        {
            DataId id=RStep::MeshCut::get(q->id);
            RMeshModel::deleteMesh(id);
            RStep::MeshCut::set(q->id,NONE);
        }
        else
        {
            for(auto j = 0; j<q->v_section_in_sect.size();j++)
            {
                if(item == q->v_section_in_sect[j])
                {
                    RStep::SectionList::remove(q->id,q->v_section_in_sect[j]->id);
                    RMeshModel::deleteMesh(q->v_section_in_sect[j]->id);
                }
            }
        }
    }
    update();
    emit need_update();
}

void OutlinerWidget :: change(QString s)
{

    QTreeWidgetItem *item = tree->currentItem();
    if(item==mainMesh)
    {
        DataId id=ROutlinerData::MainMesh::get();
        RMeshModel::Name::set(id,s);
    }
    for(auto i = 0; i < v_steps.size(); i++)
    {
        Step *q = v_steps[i];
        if (item == q)
        {
            RStep::Name::set(q->id,s);
            break;
        }
        else if(item == q->cut)
        {
            DataId id=RStep::MeshCut::get(q->id);
            RMeshModel::Name::set(id,s);
        }
        else
        {
            for(auto j = 0; j<q->v_section_in_sect.size();j++)
            {
                if(item == q->v_section_in_sect[j])
                    RMeshModel::Name::set(q->v_section_in_sect[j]->id,s);
            }
        }
    }
    update();
}

void OutlinerWidget::make_step_current()
{
    for(auto i = 0; i < v_steps.size(); i++)
    {
        Step *q = v_steps[i];
        if (currentIt == q)
        {
            ROutlinerData::WorkingStep::set(q->id);
            break;
        }
    }
    DataId id=RStep::MeshCut::get(ROutlinerData::WorkingStep::get());
    if(id!=NONE)
        RMeshModel::Visibility::makeVisibleOnlyOne(id);
    else
        RMeshModel::Visibility::makeAllUnvisible();
    update();
    emit need_update();
}

void OutlinerWidget::makeSectionVisible()
{
    QTreeWidgetItem *item = tree->currentItem();
    for(auto i = 0; i < v_steps.size(); i++)
    {
        Step *q = v_steps[i];
        for(auto j = 0; j<q->v_section_in_sect.size();j++)
        {
            if(item == q->v_section_in_sect[j])
            {
                ROutlinerData::WorkingStep::set(NONE);
                RMeshModel::Visibility::makeVisibleOnlyOne(q->v_section_in_sect[j]->id);
            }
        }
    }
    update();
    emit need_update();
}

void OutlinerWidget::makeMainMeshVisible()
{
    ROutlinerData::WorkingStep::set(NONE);
    DataId id=ROutlinerData::MainMesh::get();
    if(id!=NONE)
        RMeshModel::Visibility::makeVisibleOnlyOne(id);
    else
        RMeshModel::Visibility::makeAllUnvisible();
    update();
    emit need_update();
}

void OutlinerWidget::changeTransparency()
{
    QTreeWidgetItem *item = tree->currentItem();
    DataId id=NONE;
    if(item==mainMesh)
        id=ROutlinerData::MainMesh::get();
    for(auto i = 0; i < v_steps.size(); i++)
    {
        Step *q = v_steps[i];
        if(item == q->cut)
            id=RStep::MeshCut::get(q->id);
        else
        {
            for(auto j = 0; j<q->v_section_in_sect.size();j++)
            {
                if(item == q->v_section_in_sect[j])
                    id=q->v_section_in_sect[j]->id;
            }
        }
    }
    if(id!=NONE)
    {
        TransparencyDialog* d=new TransparencyDialog(RMeshModel::Transperancy::get(id),id);
        connect(d,SIGNAL(new_value(int,DataId)),this,SLOT(setTransparency(int,DataId)));
        d->show();
    }
}

void OutlinerWidget::setTransparency(int value,DataId id)
{
    RMeshModel::Transperancy::set(id,value);
    if(value==100)
    {
        MeshData::get().makeLast(id);
    }
    else
    {
        MeshData::get().makeFirst(id);
    }
    emit need_update();
}

