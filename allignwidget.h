#ifndef ALLIGNWIDGET_H
#define ALLIGNWIDGET_H
#include "oglareawidget.h"
#include "meshmodel.h"
class AllignWidget: public OGLAreaWidget
{
private:
    MeshModel* mesh;
public:
    AllignWidget(QWidget *parent = nullptr);
    void push(MeshModel* a)
    {
        mesh=a;
        update();
    }
    void paintGL();
    void mouseDoubleClickEvent ( QMouseEvent * e );
};

#endif // ALLIGNWIDGET_H
