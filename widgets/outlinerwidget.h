#ifndef OUTLINERCLASS_H
#define OUTLINERCLASS_H

#include <QPushButton>
#include <QTreeWidget>
#include <QWidget>
#include <QVBoxLayout>
#include <QVector>
#include <QPainter>
#include<QHeaderView>
#include"nrtlmanager.h"

class SectionItem : public QTreeWidgetItem
{
public:
    DataId id;
    SectionItem():QTreeWidgetItem(){}
    bool show;
};

class StepItem : public QTreeWidgetItem
{
private:
public:
     DataId id;
     int how_many_section = 1;
     QTreeWidgetItem *cut;
     QTreeWidgetItem *sections;
     SectionItem *sect_1;
     QVector<SectionItem*> v_section_in_sect;
     StepItem()
     {
        cut = new QTreeWidgetItem();
        sections = new QTreeWidgetItem();
        sect_1 = new SectionItem();
        cut->setText(1,"No cut");
        sections->setText(1,"Sections");
        sect_1->setText(1,"    No sections");
        v_section_in_sect.push_back(sect_1);
        sections->addChild(sect_1);
        this->addChild(cut);
        this->addChild(sections);
     }
};

class OutlinerWidget : public QWidget
{
Q_OBJECT

signals:
    void need_update();
public:
    OutlinerWidget();
    void addMainModel(MeshModel* mesh,QString name);
    void addCut(MeshModel* mesh,QString name);
    void setObjLoaderAction(QAction* obj_loader) { act_loadObj = obj_loader; }
    void addNewSection(MeshModel* mesh, IGroupId gid);
    void update();
    QIcon makeIcon(QColor clr)
    {
        QImage image(16, 16, QImage::Format_ARGB32_Premultiplied);
        QPainter painter(&image);
        painter.fillRect(0, 0, 16, 16, clr);
        QRgb rgb = clr.rgb();
        if(rgb == QColor(Qt::white).rgb()){
            QPen pen(Qt::black);
            pen.setWidth(20);
            painter.setPen(pen);
            painter.drawRect(0,0,16,16);
        }
        QPixmap pixmap;
        pixmap.convertFromImage(image);
        QIcon icon(pixmap);
        return icon;
    }

private slots:
    void add_step();
    void slotCustomMenuRequested(QPoint);
    void showContextMenu(QTreeWidgetItem* item, const QPoint& globalPos);
    void rename();
    void deleteMesh();
    void change(QString);
    void make_step_current();
    void makeSectionVisible();
    void makeMainMeshVisible();
    void changeTransparency();
    void setTransparency(int value,DataId id);
private:
    QVBoxLayout *lout;
    QTreeWidget *tree;
    QTreeWidgetItem *currentIt;
    QTreeWidgetItem *mainMesh;
    QPushButton *addStepBtn;
    QVector<StepItem*> v_steps;
    QAction* act_loadObj;
    int how_many_step = 1;
};




#endif // OUTLINERCLASS_H
