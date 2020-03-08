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


class NrtlItem : public QTreeWidgetItem
{
public:
    DataId id;
    bool show;
};

class MainMeshItem : public NrtlItem
{};

class CutItem : public NrtlItem
{};

class SectionItem : public NrtlItem
{};

class StepItem : public NrtlItem
{};

class NrtlLabel : public QTreeWidgetItem
{};



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
    void makeSectionVisible();
    void makeMeshVisible();
    void changeTransparency();
    void setTransparency(int value,DataId id);
    void itemDoubleClickedSlot();
private:
    QVBoxLayout *lout;
    QTreeWidget *tree;
    QPushButton *addStepBtn;
    QAction* act_loadObj;
};




#endif // OUTLINERCLASS_H
