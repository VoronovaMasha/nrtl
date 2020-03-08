#include "MeshAlgorithm.h"
#include "cmath"

double findDistance(QVector<QVector3D> a, QVector<QVector3D> b,QMatrix4x4 matr)
{
    double distance=0.0;
    for(int i=0;i<a.size();i++)
    {
        distance+=(a[i]-matr*b[i]).length();
    }
    return distance;
}

QMatrix4x4 AlignMesh::getAlignMatrix(QVector<QVector3D> a, QVector<QVector3D> b)
{
    QVector<QVector3D> vec_a,vec_b;
    vec_a=a;
    vec_b=b;
    QMatrix4x4 matr,res_mat;
    QVector3D a_center, b_center;
    for(int i=0;i<a.size();i++)
    {
        a_center+=a[i]/a.size();
        b_center+=b[i]/b.size();
    }
    float x=a_center.x(),y=a_center.y(),z=a_center.z();
    for(int i=0;i<a.size();i++)
    {
        a[i].setX(a[i].x()-x);
        b[i].setX(b[i].x()-x);
        a[i].setY(a[i].y()-y);
        b[i].setY(b[i].y()-y);
        a[i].setZ(a[i].z()-z);
        b[i].setZ(b[i].z()-z);
    }

    matr.setToIdentity();
    matr.translate(a_center-b_center);

    res_mat=matr;
    double min=findDistance(vec_a,vec_b,matr);

    for(int i=0;i<180;i++)
    {
        for(int j=0;j<180;j++)
        {
            for(int k=0;k<180;k++)
            {
                matr.setToIdentity();
                matr.translate(a_center);
                matr.rotate(2*k,QVector3D(0,0,1));
                matr.rotate(2*j,QVector3D(1,0,0));
                matr.rotate(2*i,QVector3D(0,0,1));
                matr.translate(-a_center);
                matr.translate(a_center-b_center);
                double distance=findDistance(vec_a,vec_b,matr);
                if(distance<min)
                {
                    min=distance;
                    res_mat=matr;
                }
            }
        }
    }

    return res_mat;
}

