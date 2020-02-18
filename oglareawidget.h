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
//    void loadObj(QString path);
//    void centerizeObj(int i);
    void cutter();
    bool checkBelonging(float _pointX, float _pointY);
    bool checkDistance(QVector3D a, QVector3D b, QVector3D c);
    void finder();
    void connecting(unsigned int a,unsigned int b);
    void alligning(QVector<QVector3D>,QVector<QVector3D>, int c, int d);
    void transparent(int what);
    QVector<MeshModel*> getAllModels();
    int getObjectsSize();
    int getSectionsSize();
    bool canCut();
    void makePolygonMatrix(int a);
    int makeBorder(int a);
    Data makeSidesOfTract(int a, int b, int numer, bool inverted);
protected:
    void initializeGL();
    void resizeGL(int w,int h);//вызывается при изменении размера окна
    void paintGL();//вызывается каждый раз при перерисовке содержимого окна
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

    float z;//сдвиг по оси z камеры
    float x;//сдвиг по x
    float y;//сдвиг по y

    int tr_section;
    int tr_model;
    int tr_tract;

    QOpenGLFramebufferObject *mFBO=0;

    //вспомогательные для расчёта z-буфера в paintGL
    QVector<QVector2D> mouseclick;
};
#endif // WIDGET_H
