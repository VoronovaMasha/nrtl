#include "MeshAlgorithm.h"
#include "nrtlmanager.h"
#include <QMatrix4x4>

QString MeshCutter::_error_string = QString("");
QMatrix4x4 MeshCutter::ViewMatrix2= QMatrix4x4();
QVector<QVector3D> MeshCutter::cutVertexes=QVector<QVector3D>();


float isLeft(float _startX, float _startY, float _endX, float _endY, float _pointX, float _pointY)
{
    return ((_endX - _startX) * (_pointY - _startY) - (_pointX - _startX) * (_endY - _startY));
}
bool checkBelonging(float _pointX, float _pointY)
{
    int windingNumber = 0;
    float startX = 0;
    float startY = 0;
    float endX = 0;
    float endY = 0;
    int count = MeshCutter::CutVertexes::get().size();
    for (int i = 1; i <= count; i++)
    {
        startX = MeshCutter::CutVertexes::get()[i-1].x();
        startY = MeshCutter::CutVertexes::get()[i-1].y();
        if (i == count)
        {
            endX = MeshCutter::CutVertexes::get()[0].x();
            endY = MeshCutter::CutVertexes::get()[0].y();
        }
        else
        {
            endX = MeshCutter::CutVertexes::get()[i].x();
            endY = MeshCutter::CutVertexes::get()[i].y();
        }
        if (startY <= _pointY)
        {
            if (endY > _pointY)
            {
                if (isLeft(startX, startY, endX, endY, _pointX, _pointY) > 0)
                {
                    ++windingNumber;
                }
            }
        }
        else
        {
            if (endY <= _pointY)
            {
                if (isLeft(startX, startY, endX, endY, _pointX, _pointY) < 0)
                {
                    --windingNumber;
                }
            }
        }
    }
    return (windingNumber != 0);
}

bool checkDistance(QVector3D a, QVector3D b, QVector3D c)
{
    float s=0.0;
    for(int i=0;i<MeshCutter::CutVertexes::get().size();i++)
    {
        for(int j=0;j<MeshCutter::CutVertexes::get().size();j++)
        {
            QVector3D tmp=MeshCutter::CutVertexes::get()[i]-MeshCutter::CutVertexes::get()[j];
            if(s<tmp.length())
                s=tmp.length();
        }
    }
    for(int i=0;i<MeshCutter::CutVertexes::get().size();i++)
    {
        if((MeshCutter::CutVertexes::get()[i]-a).length()>s || (MeshCutter::CutVertexes::get()[i]-b).length()>s || (MeshCutter::CutVertexes::get()[i]-c).length()>s)
            return false;
    }
    return true;
}

MeshModel* MeshCutter::cutFromMesh(DataId mesh_id)
{
    if(_error_string!=QString("No error"))
        return nullptr;
    QVector<QVector3D> coords;
    QVector<VertexData> vertexes;
    QVector<GLuint> indexes;
    QVector<QVector<unsigned int>> polygons;
    for(int j=0;j<MeshData::get().getElement(mesh_id)->polygons.size();j++)
    {
        QVector<unsigned int> tmp=MeshData::get().getElement(mesh_id)->polygons[j];
        QVector3D tmp1=ViewMatrix2*MeshData::get().getElement(mesh_id)->getModelMatrix()*MeshData::get().getElement(mesh_id)->coords[tmp[0]-1];
        QVector3D tmp2=ViewMatrix2*MeshData::get().getElement(mesh_id)->getModelMatrix()*MeshData::get().getElement(mesh_id)->coords[tmp[1]-1];
        QVector3D tmp3=ViewMatrix2*MeshData::get().getElement(mesh_id)->getModelMatrix()*MeshData::get().getElement(mesh_id)->coords[tmp[2]-1];
        if(checkBelonging(tmp1.x(),tmp1.y())&&
                checkBelonging(tmp2.x(),tmp2.y())&&
                checkBelonging(tmp3.x(),tmp3.y())&&
                checkDistance(tmp1,tmp2,tmp3))
        {
            polygons.append(tmp);
            vertexes.append(VertexData(QVector3D((MeshData::get().getElement(mesh_id)->getModelMatrix()*MeshData::get().getElement(mesh_id)->coords[tmp[0]-1]).x(),(MeshData::get().getElement(mesh_id)->getModelMatrix()*MeshData::get().getElement(mesh_id)->coords[tmp[0]-1]).y(),(MeshData::get().getElement(mesh_id)->getModelMatrix()*MeshData::get().getElement(mesh_id)->coords[tmp[0]-1]).z()),QVector2D(0.0,1.0),QVector3D(1.0,0.0,0.0)));
            vertexes.append(VertexData(QVector3D((MeshData::get().getElement(mesh_id)->getModelMatrix()*MeshData::get().getElement(mesh_id)->coords[tmp[1]-1]).x(),(MeshData::get().getElement(mesh_id)->getModelMatrix()*MeshData::get().getElement(mesh_id)->coords[tmp[1]-1]).y(),(MeshData::get().getElement(mesh_id)->getModelMatrix()*MeshData::get().getElement(mesh_id)->coords[tmp[1]-1]).z()),QVector2D(0.0,0.0),QVector3D(1.0,0.0,0.0)));
            vertexes.append(VertexData(QVector3D((MeshData::get().getElement(mesh_id)->getModelMatrix()*MeshData::get().getElement(mesh_id)->coords[tmp[2]-1]).x(),(MeshData::get().getElement(mesh_id)->getModelMatrix()*MeshData::get().getElement(mesh_id)->coords[tmp[2]-1]).y(),(MeshData::get().getElement(mesh_id)->getModelMatrix()*MeshData::get().getElement(mesh_id)->coords[tmp[2]-1]).z()),QVector2D(1.0,1.0),QVector3D(1.0,0.0,0.0)));
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
            vertexes.append(VertexData(QVector3D((MeshData::get().getElement(mesh_id)->getModelMatrix()*MeshData::get().getElement(mesh_id)->coords[tmp[2]-1]).x(),(MeshData::get().getElement(mesh_id)->getModelMatrix()*MeshData::get().getElement(mesh_id)->coords[tmp[2]-1]).y(),(MeshData::get().getElement(mesh_id)->getModelMatrix()*MeshData::get().getElement(mesh_id)->coords[tmp[2]-1]).z()),QVector2D(1.0,1.0),QVector3D(1.0,0.0,0.0)));
            vertexes.append(VertexData(QVector3D((MeshData::get().getElement(mesh_id)->getModelMatrix()*MeshData::get().getElement(mesh_id)->coords[tmp[1]-1]).x(),(MeshData::get().getElement(mesh_id)->getModelMatrix()*MeshData::get().getElement(mesh_id)->coords[tmp[1]-1]).y(),(MeshData::get().getElement(mesh_id)->getModelMatrix()*MeshData::get().getElement(mesh_id)->coords[tmp[1]-1]).z()),QVector2D(0.0,0.0),QVector3D(1.0,0.0,0.0)));
            vertexes.append(VertexData(QVector3D((MeshData::get().getElement(mesh_id)->getModelMatrix()*MeshData::get().getElement(mesh_id)->coords[tmp[0]-1]).x(),(MeshData::get().getElement(mesh_id)->getModelMatrix()*MeshData::get().getElement(mesh_id)->coords[tmp[0]-1]).y(),(MeshData::get().getElement(mesh_id)->getModelMatrix()*MeshData::get().getElement(mesh_id)->coords[tmp[0]-1]).z()),QVector2D(0.0,1.0),QVector3D(1.0,0.0,0.0)));
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
    }
    if(polygons.size()!=0)
    {
        coords=MeshData::get().getElement(mesh_id)->coords;
        for(int j=0;j<coords.size();j++)
        {
            coords[j]=MeshData::get().getElement(mesh_id)->getModelMatrix()*coords[j];
        }
        MeshModel* obj=new MeshModel(vertexes,indexes,QImage(":/img/textures/cube2.png"));
        obj->polygons=polygons;
        obj->coords=coords;
        return obj;
    }
    _error_string=QString("Nothing to cut.");
    return nullptr;
}
