#ifndef OUTLINERCLASS_H
#define OUTLINERCLASS_H

#include <QPushButton>
#include <QTreeWidget>
#include <QWidget>
#include <QVBoxLayout>
#include <QVector>
#include<QHeaderView>
#include"nrtlmanager.h"

class Section : public QTreeWidgetItem
{
public:
    unsigned int id;
    Section():QTreeWidgetItem(){}
};



class Step : public QTreeWidgetItem
{
private:
public:
     unsigned int id;
     int how_many_section = 1;
     QTreeWidgetItem *cut;
     QTreeWidgetItem *sections;
     Section *sect_1;
     QVector<Section*> v_section_in_sect;
     Step()
     {
        cut = new QTreeWidgetItem();
        sections = new QTreeWidgetItem();
        sect_1 = new Section();
        cut->setText(0,"No cut");
        sections->setText(0,"Sections");
        sect_1->setText(0,"No sections");
        v_section_in_sect.push_back(sect_1);
        sections->addChild(sect_1);
        this->addChild(cut);
        this->addChild(sections);
     }
};

class OutlinerWidget : public QWidget
{
Q_OBJECT
private:
    QVBoxLayout *lout;

    QTreeWidget *tree;
    QTreeWidgetItem *currentIt;
    QTreeWidgetItem *mainMesh;

    QPushButton *addStepBtn;

    QVector<Step*> v_steps;

    int how_many_step = 1;

signals:
    void need_update();
public:
    OutlinerWidget();
    void addMainModel(MeshModel* mesh,QString name);
    void addCut(MeshModel* mesh,QString name);
private slots:
    void update();
    void add_step();
    void slotCustomMenuRequested(QPoint);
    void showContextMenu(QTreeWidgetItem* item, const QPoint& globalPos);
    void add_new_section();
    void rename();
    void deleteMesh();
    void loadMainMesh();
    void change(QString);
    void make_step_current();
    void makeMainMeshVisible();

    void on_treeItem_doubleClicked(QTreeWidgetItem* itm, int column);
};

#endif // OUTLINERCLASS_H
