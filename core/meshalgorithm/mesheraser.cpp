#include "MeshAlgorithm.h"
#include "nrtlmanager.h"
#include <QMatrix4x4>

QString MeshEraser::_error_string = QString("");
QMatrix4x4 MeshEraser::ViewMatrix2= QMatrix4x4();
QVector<QVector3D> MeshEraser::cutVertexes=QVector<QVector3D>();


float isLeftToErase(float _startX, float _startY, float _endX, float _endY, float _pointX, float _pointY)
{
    return ((_endX - _startX) * (_pointY - _startY) - (_pointX - _startX) * (_endY - _startY));
}
bool checkBelongingToErase(float _pointX, float _pointY)
{
    int windingNumber = 0;
    float startX = 0;
    float startY = 0;
    float endX = 0;
    float endY = 0;
    int count = MeshEraser::CutVertexes::get().size();
    for (int i = 1; i <= count; i++)
    {
        startX = MeshEraser::CutVertexes::get()[i-1].x();
        startY = MeshEraser::CutVertexes::get()[i-1].y();
        if (i == count)
        {
            endX = MeshEraser::CutVertexes::get()[0].x();
            endY = MeshEraser::CutVertexes::get()[0].y();
        }
        else
        {
            endX = MeshEraser::CutVertexes::get()[i].x();
            endY = MeshEraser::CutVertexes::get()[i].y();
        }
        if (startY <= _pointY)
        {
            if (endY > _pointY)
            {
                if (isLeftToErase(startX, startY, endX, endY, _pointX, _pointY) > 0)
                {
                    ++windingNumber;
                }
            }
        }
        else
        {
            if (endY <= _pointY)
            {
                if (isLeftToErase(startX, startY, endX, endY, _pointX, _pointY) < 0)
                {
                    --windingNumber;
                }
            }
        }
    }
    return (windingNumber != 0);
}

bool MeshEraser::eraseFromSection(DataId section_id)
{
    if(_error_string!=QString("No error"))
        return false;
    int j=0;
    int size=MeshData::get().getElement(section_id)->polygons.size();
    while(1)
    {
        if(j>=size)
            break;
        QVector<unsigned int> tmp=MeshData::get().getElement(section_id)->polygons[j];
        QVector3D tmp1=ViewMatrix2*MeshData::get().getElement(section_id)->getModelMatrix()*MeshData::get().getElement(section_id)->coords[tmp[0]-1];
        QVector3D tmp2=ViewMatrix2*MeshData::get().getElement(section_id)->getModelMatrix()*MeshData::get().getElement(section_id)->coords[tmp[1]-1];
        QVector3D tmp3=ViewMatrix2*MeshData::get().getElement(section_id)->getModelMatrix()*MeshData::get().getElement(section_id)->coords[tmp[2]-1];
        if(checkBelongingToErase(tmp1.x(),tmp1.y())||
           checkBelongingToErase(tmp2.x(),tmp2.y())||
           checkBelongingToErase(tmp3.x(),tmp3.y()))
        {
            MeshData::get().getElement(section_id)->polygons.remove(j);
            j--;
            size--;
        }
        j++;
    }
    QVector<QVector3D> coords=MeshData::get().getElement(section_id)->coords;
    QVector<VertexData> vertexes;
    QVector<GLuint> indexes;
    QVector<QVector<unsigned int>> polygons=MeshData::get().getElement(section_id)->polygons;
    for(j=0;j<polygons.size();j++)
    {
        vertexes.append(VertexData(coords[polygons[j][0]-1],QVector2D(0.0,1.0),QVector3D(1.0,0.0,0.0)));
        vertexes.append(VertexData(coords[polygons[j][1]-1],QVector2D(0.0,0.0),QVector3D(1.0,0.0,0.0)));
        vertexes.append(VertexData(coords[polygons[j][2]-1],QVector2D(1.0,1.0),QVector3D(1.0,0.0,0.0)));
        QVector3D a=vertexes[vertexes.size()-3].position;
        QVector3D b=vertexes[vertexes.size()-2].position;
        QVector3D c=vertexes[vertexes.size()-1].position;
        QVector3D n1=QVector3D(c.x()-a.x(),c.y()-a.y(),c.z()-a.z());
        QVector3D n2=QVector3D(b.x()-a.x(),b.y()-a.y(),b.z()-a.z());
        QVector3D n=QVector3D(n1.y()*n2.z()-n1.z()*n2.y(),n1.z()*n2.x()-n1.x()*n2.z(),n1.x()*n2.y()-n1.y()*n2.x());
        QVector3D norm=QVector3D(-n.x()/n.length(),-n.y()/n.length(),-n.z()/n.length());
        vertexes[vertexes.size()-3].normal=norm;
        vertexes[vertexes.size()-2].normal=norm;
        vertexes[vertexes.size()-1].normal=norm;
        indexes.append(indexes.size());
        indexes.append(indexes.size());
        indexes.append(indexes.size());
        vertexes.append(VertexData(coords[polygons[j][2]-1],QVector2D(1.0,1.0),QVector3D(1.0,0.0,0.0)));
        vertexes.append(VertexData(coords[polygons[j][1]-1],QVector2D(0.0,0.0),QVector3D(1.0,0.0,0.0)));
        vertexes.append(VertexData(coords[polygons[j][0]-1],QVector2D(0.0,1.0),QVector3D(1.0,0.0,0.0)));
        a=vertexes[vertexes.size()-3].position;
        b=vertexes[vertexes.size()-2].position;
        c=vertexes[vertexes.size()-1].position;
        n1=QVector3D(c.x()-a.x(),c.y()-a.y(),c.z()-a.z());
        n2=QVector3D(b.x()-a.x(),b.y()-a.y(),b.z()-a.z());
        n=QVector3D(n1.y()*n2.z()-n1.z()*n2.y(),n1.z()*n2.x()-n1.x()*n2.z(),n1.x()*n2.y()-n1.y()*n2.x());
        norm=QVector3D(-n.x()/n.length(),-n.y()/n.length(),-n.z()/n.length());
        vertexes[vertexes.size()-3].normal=norm;
        vertexes[vertexes.size()-2].normal=norm;
        vertexes[vertexes.size()-1].normal=norm;
        indexes.append(indexes.size());
        indexes.append(indexes.size());
        indexes.append(indexes.size());
    }
    QMatrix4x4 modelMatrix=MeshData::get().getElement(section_id)->getModelMatrix();
    MeshData::get().getElement(section_id)->init(vertexes,indexes,MeshData::get().getElement(section_id)->image);
    MeshData::get().getElement(section_id)->setModelMatrix(modelMatrix);
    return true;
}
