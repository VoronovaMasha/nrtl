#include "MeshAlgorithm.h"
#include "nrtlmanager.h"

class Data
{
public:
    QVector<QVector<unsigned int>> polygons;
    float distance;
    bool inverted;
    QVector<QVector3D> coords;
};


QString MeshTopology::_error_string = QString("");

PolygonMatrix MeshTopology::makePolygonMatrix(MeshModel* mesh)
{
    PolygonMatrix polygonMatrix;
    for(int i=0;i<mesh->polygons.size();i++)
    {
        QVector<int> tmp;
        tmp.push_back(-1);
        tmp.push_back(-1);
        tmp.push_back(-1);
        polygonMatrix.push_back(tmp);
    }
    for(int i=0;i<mesh->polygons.size()-1;i++)
    {
        for(int j=i+1;j<mesh->polygons.size();j++)
        {
            if(mesh->polygons[j].contains(mesh->polygons[i][0]) && mesh->polygons[j].contains(mesh->polygons[i][1]))
            {
                polygonMatrix[i][0]=j;
                if((mesh->polygons[j][0]==mesh->polygons[i][0] && mesh->polygons[j][1]==mesh->polygons[i][1])||
                        (mesh->polygons[j][0]==mesh->polygons[i][1] && mesh->polygons[j][1]==mesh->polygons[i][0]))
                {
                    polygonMatrix[j][0]=i;
                }
                if((mesh->polygons[j][1]==mesh->polygons[i][0] && mesh->polygons[j][2]==mesh->polygons[i][1])||
                        (mesh->polygons[j][1]==mesh->polygons[i][1] && mesh->polygons[j][2]==mesh->polygons[i][0]))
                {
                    polygonMatrix[j][1]=i;
                }
                if((mesh->polygons[j][2]==mesh->polygons[i][0] && mesh->polygons[j][0]==mesh->polygons[i][1])||
                        (mesh->polygons[j][2]==mesh->polygons[i][1] && mesh->polygons[j][0]==mesh->polygons[i][0]))
                {
                    polygonMatrix[j][2]=i;
                }
            }
            if(mesh->polygons[j].contains(mesh->polygons[i][1]) && mesh->polygons[j].contains(mesh->polygons[i][2]))
            {
                polygonMatrix[i][1]=j;
                if((mesh->polygons[j][0]==mesh->polygons[i][1] && mesh->polygons[j][1]==mesh->polygons[i][2])||
                        (mesh->polygons[j][0]==mesh->polygons[i][2] && mesh->polygons[j][1]==mesh->polygons[i][1]))
                {
                    polygonMatrix[j][0]=i;
                }
                if((mesh->polygons[j][1]==mesh->polygons[i][1] && mesh->polygons[j][2]==mesh->polygons[i][2])||
                        (mesh->polygons[j][1]==mesh->polygons[i][2] && mesh->polygons[j][2]==mesh->polygons[i][1]))
                {
                    polygonMatrix[j][1]=i;
                }
                if((mesh->polygons[j][2]==mesh->polygons[i][1] && mesh->polygons[j][0]==mesh->polygons[i][2])||
                        (mesh->polygons[j][2]==mesh->polygons[i][2] && mesh->polygons[j][0]==mesh->polygons[i][1]))
                {
                    polygonMatrix[j][2]=i;
                }
            }
            if(mesh->polygons[j].contains(mesh->polygons[i][2]) && mesh->polygons[j].contains(mesh->polygons[i][0]))
            {
                polygonMatrix[i][2]=j;
                if((mesh->polygons[j][0]==mesh->polygons[i][2] && mesh->polygons[j][1]==mesh->polygons[i][0])||
                        (mesh->polygons[j][0]==mesh->polygons[i][0] && mesh->polygons[j][1]==mesh->polygons[i][2]))
                {
                    polygonMatrix[j][0]=i;
                }
                if((mesh->polygons[j][1]==mesh->polygons[i][2] && mesh->polygons[j][2]==mesh->polygons[i][0])||
                        (mesh->polygons[j][1]==mesh->polygons[i][0] && mesh->polygons[j][2]==mesh->polygons[i][2]))
                {
                    polygonMatrix[j][1]=i;
                }
                if((mesh->polygons[j][2]==mesh->polygons[i][2] && mesh->polygons[j][0]==mesh->polygons[i][0])||
                        (mesh->polygons[j][2]==mesh->polygons[i][0] && mesh->polygons[j][0]==mesh->polygons[i][2]))
                {
                    polygonMatrix[j][2]=i;
                }
            }
        }
    }
    return polygonMatrix;
}

MeshBorder MeshTopology::makeBorder(MeshModel* mesh)
{
    MeshBorder border;
    int q=-1;
    for(int i=0;i<mesh->polygonMatrix.size();i++)
    {
        if(mesh->polygonMatrix[i].contains(-1))
        {
            q=i;
            break;
        }
    }
    if(q==-1)
    {
        _error_string=QString("Section '"+RMeshModel::Name::get(MeshData::get().getId(mesh))+"' is bad for making tract. Choose another section.");
        border.clear();
        return border;
    }
    QVector<unsigned int> visited;
    if(mesh->polygonMatrix[q].count(-1)==3)
    {
        _error_string=QString("Section '"+RMeshModel::Name::get(MeshData::get().getId(mesh))+"' is bad for making tract. Choose another section.");
        border.clear();
        return border;
    }
    else if(mesh->polygonMatrix[q].count(-1)==2)
    {
        if(mesh->polygonMatrix[q][0]==-1 && mesh->polygonMatrix[q][1]==-1)
            border.push_back(mesh->coords[mesh->polygons[q][0]-1]);
        if(mesh->polygonMatrix[q][1]==-1 && mesh->polygonMatrix[q][2]==-1)
            border.push_back(mesh->coords[mesh->polygons[q][1]-1]);
        if(mesh->polygonMatrix[q][0]==-1 && mesh->polygonMatrix[q][2]==-1)
            border.push_back(mesh->coords[mesh->polygons[q][2]-1]);
    }
    else
    {
        if(mesh->polygonMatrix[q][0]==-1)
            border.push_back(mesh->coords[mesh->polygons[q][0]-1]);
        if(mesh->polygonMatrix[q][1]==-1)
            border.push_back(mesh->coords[mesh->polygons[q][1]-1]);
        if(mesh->polygonMatrix[q][2]==-1)
            border.push_back(mesh->coords[mesh->polygons[q][2]-1]);
    }
    while(1)
    {
        visited.push_back(q);
        if(mesh->polygonMatrix[q].count(-1)==3)
        {
            _error_string=QString("Section '"+RMeshModel::Name::get(MeshData::get().getId(mesh))+"' is bad for making tract. Choose another section.");
            border.clear();
            return border;
        }
        else if(mesh->polygonMatrix[q].count(-1)==2)
        {
            if(mesh->polygonMatrix[q][0]==-1 && mesh->polygonMatrix[q][1]==-1)
            {
                border.push_back(mesh->coords[mesh->polygons[q][1]-1]);
                if(border.count(mesh->coords[mesh->polygons[q][0]-1])==0)
                    border.push_back(mesh->coords[mesh->polygons[q][0]-1]);
                if(border.count(mesh->coords[mesh->polygons[q][2]-1])==0)
                    border.push_back(mesh->coords[mesh->polygons[q][2]-1]);
            }
            if(mesh->polygonMatrix[q][1]==-1 && mesh->polygonMatrix[q][2]==-1)
            {
                border.push_back(mesh->coords[mesh->polygons[q][2]-1]);
                if(border.count(mesh->coords[mesh->polygons[q][0]-1])==0)
                    border.push_back(mesh->coords[mesh->polygons[q][0]-1]);
                if(border.count(mesh->coords[mesh->polygons[q][1]-1])==0)
                    border.push_back(mesh->coords[mesh->polygons[q][1]-1]);
            }
            if(mesh->polygonMatrix[q][0]==-1 && mesh->polygonMatrix[q][2]==-1)
            {
                border.push_back(mesh->coords[mesh->polygons[q][0]-1]);
                if(border.count(mesh->coords[mesh->polygons[q][1]-1])==0)
                    border.push_back(mesh->coords[mesh->polygons[q][1]-1]);
                if(border.count(mesh->coords[mesh->polygons[q][2]-1])==0)
                    border.push_back(mesh->coords[mesh->polygons[q][2]-1]);
            }
        }
        else
        {
            if(mesh->polygonMatrix[q][0]==-1)
            {
                if(border.count(mesh->coords[mesh->polygons[q][0]-1])==0)
                    border.push_back(mesh->coords[mesh->polygons[q][0]-1]);
                if(border.count(mesh->coords[mesh->polygons[q][1]-1])==0)
                    border.push_back(mesh->coords[mesh->polygons[q][1]-1]);
            }
            if(mesh->polygonMatrix[q][1]==-1)
            {
                if(border.count(mesh->coords[mesh->polygons[q][1]-1])==0)
                    border.push_back(mesh->coords[mesh->polygons[q][1]-1]);
                if(border.count(mesh->coords[mesh->polygons[q][2]-1])==0)
                    border.push_back(mesh->coords[mesh->polygons[q][2]-1]);
            }
            if(mesh->polygonMatrix[q][2]==-1)
            {
                if(border.count(mesh->coords[mesh->polygons[q][0]-1])==0)
                    border.push_back(mesh->coords[mesh->polygons[q][0]-1]);
                if(border.count(mesh->coords[mesh->polygons[q][2]-1])==0)
                    border.push_back(mesh->coords[mesh->polygons[q][2]-1]);
            }
        }
        q=-1;
        for(int i=0;i<mesh->polygonMatrix.size();i++)
        {
            if(visited.count(i)==0 && mesh->polygonMatrix[i].count(-1)!=0)
            {
                if((mesh->polygonMatrix[i][0]==-1 && (mesh->coords[mesh->polygons[i][0]-1]==border[border.size()-1] ||
                                                              mesh->coords[mesh->polygons[i][1]-1]==border[border.size()-1])) ||
                   (mesh->polygonMatrix[i][1]==-1 && (mesh->coords[mesh->polygons[i][1]-1]==border[border.size()-1] ||
                                                              mesh->coords[mesh->polygons[i][2]-1]==border[border.size()-1])) ||
                   (mesh->polygonMatrix[i][2]==-1 && (mesh->coords[mesh->polygons[i][0]-1]==border[border.size()-1] ||
                                                              mesh->coords[mesh->polygons[i][2]-1]==border[border.size()-1])))
                {
                    q=i;
                    break;
                }
            }
        }
        if(q==-1)
            break;
    }
    for(int i=0;i<mesh->polygonMatrix.size();i++)
    {
        if(mesh->polygonMatrix[i].contains(-1) && visited.count(i)==0)
        {
            _error_string=QString("Section '"+RMeshModel::Name::get(MeshData::get().getId(mesh))+"' is bad for making tract. Choose another section.");
            border.clear();
            return border;
        }
    }
    return border;
}

Data makeSidesOfTract(MeshModel* a, MeshModel* b, int numer, bool inverted)
{
    QVector<QVector3D> border1,border2,border;
    QVector<QVector<unsigned int>> polygons;
    if(a->border.size()>=b->border.size())
    {
        border1=b->border;
        border2=a->border;
    }
    else
    {
        border1=a->border;
        border2=b->border;
    }
    if(inverted)
    {
        for(int i=0;i<border2.size()/2;i++)
        {
            QVector3D tmp=border2[i];
            border2[i]=border2[border2.size()-1-i];
            border2[border2.size()-1-i]=tmp;
        }
    }
    float distance=0.0f;
    int s,w;
    border=border1+border2;
    for(int i=0;i<border1.size()-1;i++)
    {
        QVector<unsigned int> tmp;
        tmp.push_back(i+1);
        tmp.push_back(border1.size()+(numer+i)%border2.size()+1);
        tmp.push_back(border1.size()+(numer+1+i)%border2.size()+1);
        polygons.push_back(tmp);
        tmp.clear();
        tmp.push_back(i+1);
        tmp.push_back(border1.size()+(numer+1+i)%border2.size()+1);
        tmp.push_back(i+1+1);
        polygons.push_back(tmp);
        s=i;
        distance+=(border[i]-border[border1.size()+(numer+i)%border2.size()]).length();
        distance+=(border[i]-border[border1.size()+(numer+1+i)%border2.size()]).length();
        distance+=(border[i+1]-border[border1.size()+(numer+1+i)%border2.size()]).length();
    }
    for(int i=0;i<border2.size()-border1.size();i++)
    {
        QVector<unsigned int> tmp;
        tmp.push_back(border1.size());
        tmp.push_back(border1.size()+(numer+1+s+i)%border2.size()+1);
        tmp.push_back(border1.size()+(numer+2+s+i)%border2.size()+1);
        polygons.push_back(tmp);
        distance+=(border[border1.size()-1]-border[border1.size()+(numer+2+s+i)%border2.size()]).length();
        w=border1.size()+(numer+2+s+i)%border2.size()+1;
    }
    QVector<unsigned int> tmp;
    tmp.push_back(border1.size());
    tmp.push_back(w);
    tmp.push_back(border1.size()+numer+1);
    polygons.push_back(tmp);
    tmp.clear();
    tmp.push_back(border1.size());
    tmp.push_back(border1.size()+numer+1);
    tmp.push_back(1);
    polygons.push_back(tmp);
    distance+=(border[border1.size()-1]-border[border1.size()+numer]).length();
    Data d;
    d.distance=distance;
    d.polygons=polygons;
    d.inverted=inverted;
    d.coords=border;
    return d;
}

MeshModel* MeshTopology::makeSurface(MeshModel* a , MeshModel* b)
{
    if(a->border.size()==0 || b->border.size()==0)
        return nullptr;
    QVector<QVector<unsigned int>> polygons;
    QVector<QVector3D> coords;
    int q;
    if(a->border.size()>=b->border.size())
        q=a->border.size();
    else
        q=b->border.size();
    Data d=makeSidesOfTract(a,b,0,false);
    for(int i=0;i<q;i++)
    {
        Data e=makeSidesOfTract(a,b,i,false);
        if(e.distance<d.distance)
            d=e;
        e=makeSidesOfTract(a,b,i,true);
        if(e.distance<d.distance)
            d=e;
    }
    polygons=d.polygons;
    coords=d.coords;
    QVector<VertexData> vertexes;
    QVector<GLuint> indexes;
    for(int j=0;j<polygons.size();j++)
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
    MeshModel* obj=new MeshModel(vertexes,indexes,QImage(":/img/textures/cube3.png"));
    obj->polygons=polygons;
    obj->coords=coords;
    return obj;
}
