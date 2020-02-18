#include "MeshAlgorithm.h"

QMatrix4x4 AlignMesh::getAlignMatrix(QVector<QVector3D> dst, QVector<QVector3D> src)
{
    float x=dst[0].x(),y=dst[0].y(),z=dst[0].z();
    float x1=src[0].x(),y1=src[0].y(),z1=src[0].z();
    QVector3D tmp1,tmp2,axis;
    float angle;
    QQuaternion r1,r2;
    QMatrix4x4 matr,matr2;
    for(int i=0;i<dst.size();i++)
    {
        dst[i].setX(dst[i].x()-x);
        src[i].setX(src[i].x()-x);
        dst[i].setY(dst[i].y()-y);
        src[i].setY(src[i].y()-y);
        dst[i].setZ(dst[i].z()-z);
        src[i].setZ(src[i].z()-z);
    }
    QVector3D translater=dst[0]-src[0];
    for(int i=0;i<dst.size();i++)
    {
        src[i]=src[i]+translater;
    }
    tmp1=dst[1]-dst[0];
    tmp2=src[1]-dst[0];
    angle=acos(QVector3D::dotProduct(tmp1,tmp2)/(tmp1.length()*tmp2.length()));
    axis=QVector3D(tmp1.y()*tmp2.z()-tmp1.z()*tmp2.y(),-tmp1.x()*tmp2.z()+tmp1.z()*tmp2.x(),tmp1.x()*tmp2.y()-tmp1.y()*tmp2.x());
    r1=QQuaternion::fromAxisAndAngle(axis,-angle*180.0/3.14);
    matr.setToIdentity();
    matr.rotate(r1);
    for(int i=0;i<dst.size();i++)
    {
        src[i]=matr*src[i];
    }
    tmp1=dst[2]-dst[1];
    tmp2=src[2]-dst[1];
    angle=acos(QVector3D::dotProduct(tmp1,tmp2)/(tmp1.length()*tmp2.length()));
    axis=QVector3D(tmp1.y()*tmp2.z()-tmp1.z()*tmp2.y(),-tmp1.x()*tmp2.z()+tmp1.z()*tmp2.x(),tmp1.x()*tmp2.y()-tmp1.y()*tmp2.x());
    r2=QQuaternion::fromAxisAndAngle(axis,-angle*180.0/3.14);
    matr.setToIdentity();
    matr.translate(QVector3D(x,y,z));
    matr.rotate(r2*r1);
    matr.translate(QVector3D(-x,-y,-z));
    matr.translate(QVector3D(translater.x(),translater.y(),translater.z()));

    return matr;
}

