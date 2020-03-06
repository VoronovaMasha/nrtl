#include "MeshAlgorithm.h"
#include "cmath"

QMatrix4x4 AlignMesh::getAlignMatrix(QVector<QVector3D> a, QVector<QVector3D> b)
{
    QVector<QVector3D> vec_a,vec_b;
    vec_a=a;
    vec_b=b;
    QVector3D tmp1,tmp2,axis,axis1;
    QMatrix4x4 matr,matr2;
    float angle,angle1;
    float x=a[0].x(),y=a[0].y(),z=a[0].z();
    for(int i=0;i<a.size();i++)
    {
        a[i].setX(a[i].x()-x);
        b[i].setX(b[i].x()-x);
        a[i].setY(a[i].y()-y);
        b[i].setY(b[i].y()-y);
        a[i].setZ(a[i].z()-z);
        b[i].setZ(b[i].z()-z);
    }
    QVector3D translater=a[0]-b[0];
    for(int i=0;i<a.size();i++)
    {
        b[i]=b[i]+translater;
    }
    tmp1=a[1]-a[0];
    tmp2=b[1]-a[0];
    angle=acos(QVector3D::dotProduct(tmp1,tmp2)/(tmp1.length()*tmp2.length()));
    axis=QVector3D(tmp1.y()*tmp2.z()-tmp1.z()*tmp2.y(),-tmp1.x()*tmp2.z()+tmp1.z()*tmp2.x(),tmp1.x()*tmp2.y()-tmp1.y()*tmp2.x());

    matr2.setToIdentity();
    matr2.rotate(angle*180.0/3.14,-axis);
    for(int i=0;i<b.size();i++)
    {
        b[i]=matr2*b[i];
    }
    tmp1=a[2]-a[0];
    tmp2=b[2]-a[0];
    angle1=acos(QVector3D::dotProduct(tmp1,tmp2)/(tmp1.length()*tmp2.length()));
    axis1=QVector3D(tmp1.y()*tmp2.z()-tmp1.z()*tmp2.y(),-tmp1.x()*tmp2.z()+tmp1.z()*tmp2.x(),tmp1.x()*tmp2.y()-tmp1.y()*tmp2.x());

    QMatrix4x4 res_mat;
    matr.setToIdentity();
    matr.translate(vec_a[0]);
    matr.rotate(angle1*180.0/3.14,-axis1);
    matr.rotate(angle*180.0/3.14,-axis);
    matr.translate(-vec_a[0]);
    matr.translate(vec_a[0]-vec_b[0]);

    res_mat=matr;

    float min=(vec_a[0]-matr*vec_b[0]).length()+(vec_a[1]-matr*vec_b[1]).length()+(vec_a[2]-matr*vec_b[2]).length();

    for(int i=0;i<720;i++)
    {
        matr.setToIdentity();
        matr.translate(vec_a[0]);
        matr.rotate(0.5*i,a[1]-a[0]);
        matr.rotate(angle*180.0/3.14,-axis);
        matr.translate(-vec_a[0]);
        matr.translate(vec_a[0]-vec_b[0]);
        if(((vec_a[0]-matr*vec_b[0]).length()+(vec_a[1]-matr*vec_b[1]).length()+(vec_a[2]-matr*vec_b[2]).length())<min)
        {
            min=(vec_a[0]-matr*vec_b[0]).length()+(vec_a[1]-matr*vec_b[1]).length()+(vec_a[2]-matr*vec_b[2]).length();
            res_mat=matr;
        }
    }

    return res_mat;
}

