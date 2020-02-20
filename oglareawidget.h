#ifndef WIDGET_H
#define WIDGET_H

#include <QOpenGLWidget>
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLBuffer>
#include <QMouseEvent>
#include <QOpenGLContext>
#include <QOpenGLFramebufferObject>
#include <QOpenGLFunctions>

#include "nrtlmanager.h"
#include "meshmodel.h"

class Data
{
public:
    QVector<QVector<unsigned int>> polygons;
    float distance;
    bool inverted;
    QVector<QVector3D> coords;
};

class OGLAreaWidget : public QOpenGLWidget, public QOpenGLFunctions
{
    Q_OBJECT
public slots:
    void updater()
    {
        update();
    }
public:
    OGLAreaWidget(QWidget *parent = nullptr);
    ~OGLAreaWidget();
    int cutFlag=0;
    bool isCutting;
    QVector<QVector3D> cutVertexes;
    QVector<QVector3D> tr_cutVertexes;
public slots:
    QMatrix4x4 getViewMatrix()
    {
        return ViewMatrix2;
    }
    void finder();
    void connecting(unsigned int a,unsigned int b);
    void makePolygonMatrix(int a);
    int makeBorder(int a);
    Data makeSidesOfTract(int a, int b, int numer, bool inverted);
protected:
    void initializeGL();
    void resizeGL(int w,int h);
    void paintGL();
    void initShaders();

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent* event);
protected:
    QMatrix4x4 projectionMatrix;
    QMatrix4x4 ViewMatrix1;
    QMatrix4x4 ViewMatrix2;
    QMatrix4x4 ViewMatrix;
    QOpenGLShaderProgram program;

    QVector2D mousePosition;
    QQuaternion rotation;


    QVector<MeshModel*> objects;
    QVector<MeshModel*> sections;
    QVector<MeshModel*> tracks;

    float z;
    float x;
    float y;

    int tr_section;
    int tr_model;
    int tr_tract;

    QOpenGLFramebufferObject *mFBO=0;

    QVector<QVector2D> mouseclick;
};
#endif // WIDGET_H
