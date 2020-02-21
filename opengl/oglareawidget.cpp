#include "oglareawidget.h"
#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <QOpenGLExtraFunctions>
#include <QMessageBox>
#include <QFileInfo>

OGLAreaWidget::OGLAreaWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{
    z=-5.0f;
    x=0.0f;
    y=0.0f;
    isCutting=false;
    tr_section=100;
    tr_model=100;
    tr_tract=100;
}

OGLAreaWidget::~OGLAreaWidget()
{
}

void OGLAreaWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0.0f,0.0f,0.0f,1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    initShaders();
}
void OGLAreaWidget::resizeGL(int w,int h)
{
    float aspect=(float)w/(float)h;
    projectionMatrix.setToIdentity();
    projectionMatrix.perspective(45,aspect,0.01f,50.0f);
}
void OGLAreaWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    ViewMatrix1.setToIdentity();
    ViewMatrix2.setToIdentity();
    ViewMatrix1.translate(x,y,z);
    ViewMatrix2.rotate(rotation);
    ViewMatrix=ViewMatrix1*ViewMatrix2;
    program.bind();
    program.setUniformValue("u_projectionMatrix",projectionMatrix);
    program.setUniformValue("u_viewMatrix",ViewMatrix);
    program.setUniformValue("u_lightPosition",QVector4D(0.0,0.0,0.0,1.0));
    program.setUniformValue("u_lightPower",4*abs(z));

    for(auto model : MeshData::get())
    {
        model->draw(&program, context()->functions());
    }

    QMatrix4x4 matr;
    matr.setToIdentity();
    program.setUniformValue("u_modelMatrix", matr);
    if(isCutting)
    {
        QOpenGLContext *ctx=QOpenGLContext::currentContext();
        if(mFBO)
            delete mFBO;
        QOpenGLFramebufferObjectFormat format;
        format.setSamples(0);
        format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
        mFBO=new QOpenGLFramebufferObject(size(),format);
        glBindFramebuffer(GL_READ_FRAMEBUFFER,defaultFramebufferObject());
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER,mFBO->handle());
        ctx->extraFunctions()->glBlitFramebuffer(0,0,width(),height(),0,0,mFBO->width(),mFBO->height(),GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT,GL_NEAREST);
        mFBO->bind();
        float depth;
        bool error=false;
        if((mouseclick.size()==1 && tr_cutVertexes.size()>0) ||
                (mouseclick.size()==2 && tr_cutVertexes.size()==0 && cutVertexes.size()>0))
            error=true;
        for(int i=0;i<mouseclick.size();i++)
        {
            if(error)
                break;
            glReadPixels(mouseclick[i].x(),height()-mouseclick[i].y(),1,1,GL_DEPTH_COMPONENT,GL_FLOAT,&depth);
            if(depth>=1)
                return;
            float zNorm=2*depth-1;
            float zView=2*0.01*50/((50-0.01)*zNorm-0.01-50);
            depth=zView-z;
            QVector4D tmp(2.0f*mouseclick[i].x()/width()-1.0f,-2.0f*mouseclick[i].y()/height()+1.0f,-1.0f,1.0f);
            QVector4D tmp2((projectionMatrix.inverted()*tmp).toVector2D(),-1.0f,0.0f);//вектор направления клика мышью
            QVector3D direction((ViewMatrix1.inverted()*tmp2).toVector3D().normalized());//вектор направления клика мышью
                                                                                            //в мировой СК
            QVector3D camPos((ViewMatrix1.inverted()*QVector4D(0.0f,0.0f,0.0f,1.0f)).toVector3D());
            QVector3D N(0.0f,0.0f,1.0f);
            float t=QVector3D::dotProduct(QVector3D(camPos.x(),camPos.y(),depth-camPos.z()),N)/QVector3D::dotProduct(direction,N);
            QVector3D result=camPos+direction*t;
            if(i==0)
                cutVertexes.append(result);
            if(i==1)
                tr_cutVertexes.push_front(result);
        }
        mouseclick.clear();
        mFBO->release();
        isCutting=false;
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);
        if(tr_cutVertexes.size()!=0)
        {
            glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
            glBegin(GL_QUADS);
            for(int i=0;i<cutVertexes.size()-1;i++)
            {
                glVertex3d((ViewMatrix2.inverted()*cutVertexes[i]).x(),(ViewMatrix2.inverted()*cutVertexes[i]).y(),(ViewMatrix2.inverted()*cutVertexes[i]).z());
                glVertex3d((ViewMatrix2.inverted()*tr_cutVertexes[tr_cutVertexes.size()-i-1]).x(),(ViewMatrix2.inverted()*tr_cutVertexes[tr_cutVertexes.size()-i-1]).y(),(ViewMatrix2.inverted()*tr_cutVertexes[tr_cutVertexes.size()-i-1]).z());
                glVertex3d((ViewMatrix2.inverted()*tr_cutVertexes[tr_cutVertexes.size()-i-2]).x(),(ViewMatrix2.inverted()*tr_cutVertexes[tr_cutVertexes.size()-i-2]).y(),(ViewMatrix2.inverted()*tr_cutVertexes[tr_cutVertexes.size()-i-2]).z());
                glVertex3d((ViewMatrix2.inverted()*cutVertexes[i+1]).x(),(ViewMatrix2.inverted()*cutVertexes[i+1]).y(),(ViewMatrix2.inverted()*cutVertexes[i+1]).z());
                glVertex3d((ViewMatrix2.inverted()*cutVertexes[i+1]).x(),(ViewMatrix2.inverted()*cutVertexes[i+1]).y(),(ViewMatrix2.inverted()*cutVertexes[i+1]).z());
                glVertex3d((ViewMatrix2.inverted()*tr_cutVertexes[tr_cutVertexes.size()-i-2]).x(),(ViewMatrix2.inverted()*tr_cutVertexes[tr_cutVertexes.size()-i-2]).y(),(ViewMatrix2.inverted()*tr_cutVertexes[tr_cutVertexes.size()-i-2]).z());
                glVertex3d((ViewMatrix2.inverted()*tr_cutVertexes[tr_cutVertexes.size()-i-1]).x(),(ViewMatrix2.inverted()*tr_cutVertexes[tr_cutVertexes.size()-i-1]).y(),(ViewMatrix2.inverted()*tr_cutVertexes[tr_cutVertexes.size()-i-1]).z());
                glVertex3d((ViewMatrix2.inverted()*cutVertexes[i]).x(),(ViewMatrix2.inverted()*cutVertexes[i]).y(),(ViewMatrix2.inverted()*cutVertexes[i]).z());
            }
            glEnd();
        }
        else
        {
            glLineWidth(2);
            glBegin(GL_LINE_LOOP);
            for(int i=0;i<cutVertexes.size();i++)
            {
                glVertex3d((ViewMatrix2.inverted()*cutVertexes[i]).x(),(ViewMatrix2.inverted()*cutVertexes[i]).y(),(ViewMatrix2.inverted()*cutVertexes[i]).z());
            }
            glEnd();
        }
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
    }
    program.release();
}
void OGLAreaWidget::initShaders()
{
    if(!program.addShaderFromSourceFile(QOpenGLShader::Vertex,":/ogl/shaders/vshader.vsh"))
        close();
    if(!program.addShaderFromSourceFile(QOpenGLShader::Fragment,":/ogl/shaders/fshader.fsh"))
        close();
    if(!program.link())
        close();
}

void OGLAreaWidget::finder()
{
    for(int i=0;i<objects.size();i++)
    {
        qDebug()<<"_________";
        for(int j=0;j<objects[i]->coords.size();j++)
        {
            qDebug()<<objects[i]->coords[j];
        }
    }
}

void OGLAreaWidget::makePolygonMatrix(int a)
{
    if(a<1||a>sections.size())
        return;
    QVector<QVector<int>> polygonMatrix;
    for(int i=0;i<sections[a-1]->polygons.size();i++)
    {
        QVector<int> tmp;
        tmp.push_back(-1);
        tmp.push_back(-1);
        tmp.push_back(-1);
        polygonMatrix.push_back(tmp);
    }
    for(int i=0;i<sections[a-1]->polygons.size()-1;i++)
    {
        for(int j=i+1;j<sections[a-1]->polygons.size();j++)
        {
            if(sections[a-1]->polygons[j].contains(sections[a-1]->polygons[i][0]) && sections[a-1]->polygons[j].contains(sections[a-1]->polygons[i][1]))
            {
                polygonMatrix[i][0]=j;
                if((sections[a-1]->polygons[j][0]==sections[a-1]->polygons[i][0] && sections[a-1]->polygons[j][1]==sections[a-1]->polygons[i][1])||
                        (sections[a-1]->polygons[j][0]==sections[a-1]->polygons[i][1] && sections[a-1]->polygons[j][1]==sections[a-1]->polygons[i][0]))
                {
                    polygonMatrix[j][0]=i;
                }
                if((sections[a-1]->polygons[j][1]==sections[a-1]->polygons[i][0] && sections[a-1]->polygons[j][2]==sections[a-1]->polygons[i][1])||
                        (sections[a-1]->polygons[j][1]==sections[a-1]->polygons[i][1] && sections[a-1]->polygons[j][2]==sections[a-1]->polygons[i][0]))
                {
                    polygonMatrix[j][1]=i;
                }
                if((sections[a-1]->polygons[j][2]==sections[a-1]->polygons[i][0] && sections[a-1]->polygons[j][0]==sections[a-1]->polygons[i][1])||
                        (sections[a-1]->polygons[j][2]==sections[a-1]->polygons[i][1] && sections[a-1]->polygons[j][0]==sections[a-1]->polygons[i][0]))
                {
                    polygonMatrix[j][2]=i;
                }
            }
            if(sections[a-1]->polygons[j].contains(sections[a-1]->polygons[i][1]) && sections[a-1]->polygons[j].contains(sections[a-1]->polygons[i][2]))
            {
                polygonMatrix[i][1]=j;
                if((sections[a-1]->polygons[j][0]==sections[a-1]->polygons[i][1] && sections[a-1]->polygons[j][1]==sections[a-1]->polygons[i][2])||
                        (sections[a-1]->polygons[j][0]==sections[a-1]->polygons[i][2] && sections[a-1]->polygons[j][1]==sections[a-1]->polygons[i][1]))
                {
                    polygonMatrix[j][0]=i;
                }
                if((sections[a-1]->polygons[j][1]==sections[a-1]->polygons[i][1] && sections[a-1]->polygons[j][2]==sections[a-1]->polygons[i][2])||
                        (sections[a-1]->polygons[j][1]==sections[a-1]->polygons[i][2] && sections[a-1]->polygons[j][2]==sections[a-1]->polygons[i][1]))
                {
                    polygonMatrix[j][1]=i;
                }
                if((sections[a-1]->polygons[j][2]==sections[a-1]->polygons[i][1] && sections[a-1]->polygons[j][0]==sections[a-1]->polygons[i][2])||
                        (sections[a-1]->polygons[j][2]==sections[a-1]->polygons[i][2] && sections[a-1]->polygons[j][0]==sections[a-1]->polygons[i][1]))
                {
                    polygonMatrix[j][2]=i;
                }
            }
            if(sections[a-1]->polygons[j].contains(sections[a-1]->polygons[i][2]) && sections[a-1]->polygons[j].contains(sections[a-1]->polygons[i][0]))
            {
                polygonMatrix[i][2]=j;
                if((sections[a-1]->polygons[j][0]==sections[a-1]->polygons[i][2] && sections[a-1]->polygons[j][1]==sections[a-1]->polygons[i][0])||
                        (sections[a-1]->polygons[j][0]==sections[a-1]->polygons[i][0] && sections[a-1]->polygons[j][1]==sections[a-1]->polygons[i][2]))
                {
                    polygonMatrix[j][0]=i;
                }
                if((sections[a-1]->polygons[j][1]==sections[a-1]->polygons[i][2] && sections[a-1]->polygons[j][2]==sections[a-1]->polygons[i][0])||
                        (sections[a-1]->polygons[j][1]==sections[a-1]->polygons[i][0] && sections[a-1]->polygons[j][2]==sections[a-1]->polygons[i][2]))
                {
                    polygonMatrix[j][1]=i;
                }
                if((sections[a-1]->polygons[j][2]==sections[a-1]->polygons[i][2] && sections[a-1]->polygons[j][0]==sections[a-1]->polygons[i][0])||
                        (sections[a-1]->polygons[j][2]==sections[a-1]->polygons[i][0] && sections[a-1]->polygons[j][0]==sections[a-1]->polygons[i][2]))
                {
                    polygonMatrix[j][2]=i;
                }
            }
        }
    }
    sections[a-1]->setPolygonMatrix(polygonMatrix);
}

int OGLAreaWidget::makeBorder(int a)
{
    if(a<1||a>sections.size())
        return -1;
    if(sections[a-1]->polygonMatrix.size()==0)
        return -1;
    if(sections[a-1]->border.size()!=0)
        return 1;
    int q=-1;
    for(int i=0;i<sections[a-1]->polygonMatrix.size();i++)
    {
        if(sections[a-1]->polygonMatrix[i].contains(-1))
        {
            q=i;
            break;
        }
    }
    if(q==-1)
        return -1;
    QVector<QVector3D> border;
    QVector<unsigned int> visited;
    if(sections[a-1]->polygonMatrix[q].count(-1)==3)
        return -1;
    else if(sections[a-1]->polygonMatrix[q].count(-1)==2)
    {
        if(sections[a-1]->polygonMatrix[q][0]==-1 && sections[a-1]->polygonMatrix[q][1]==-1)
            border.push_back(sections[a-1]->coords[sections[a-1]->polygons[q][0]-1]);
        if(sections[a-1]->polygonMatrix[q][1]==-1 && sections[a-1]->polygonMatrix[q][2]==-1)
            border.push_back(sections[a-1]->coords[sections[a-1]->polygons[q][1]-1]);
        if(sections[a-1]->polygonMatrix[q][0]==-1 && sections[a-1]->polygonMatrix[q][2]==-1)
            border.push_back(sections[a-1]->coords[sections[a-1]->polygons[q][2]-1]);
    }
    else
    {
        if(sections[a-1]->polygonMatrix[q][0]==-1)
            border.push_back(sections[a-1]->coords[sections[a-1]->polygons[q][0]-1]);
        if(sections[a-1]->polygonMatrix[q][1]==-1)
            border.push_back(sections[a-1]->coords[sections[a-1]->polygons[q][1]-1]);
        if(sections[a-1]->polygonMatrix[q][2]==-1)
            border.push_back(sections[a-1]->coords[sections[a-1]->polygons[q][2]-1]);
    }
    while(1)
    {
        visited.push_back(q);
        if(sections[a-1]->polygonMatrix[q].count(-1)==3)
            return -1;
        else if(sections[a-1]->polygonMatrix[q].count(-1)==2)
        {
            if(sections[a-1]->polygonMatrix[q][0]==-1 && sections[a-1]->polygonMatrix[q][1]==-1)
            {
                border.push_back(sections[a-1]->coords[sections[a-1]->polygons[q][1]-1]);
                if(border.count(sections[a-1]->coords[sections[a-1]->polygons[q][0]-1])==0)
                    border.push_back(sections[a-1]->coords[sections[a-1]->polygons[q][0]-1]);
                if(border.count(sections[a-1]->coords[sections[a-1]->polygons[q][2]-1])==0)
                    border.push_back(sections[a-1]->coords[sections[a-1]->polygons[q][2]-1]);
            }
            if(sections[a-1]->polygonMatrix[q][1]==-1 && sections[a-1]->polygonMatrix[q][2]==-1)
            {
                border.push_back(sections[a-1]->coords[sections[a-1]->polygons[q][2]-1]);
                if(border.count(sections[a-1]->coords[sections[a-1]->polygons[q][0]-1])==0)
                    border.push_back(sections[a-1]->coords[sections[a-1]->polygons[q][0]-1]);
                if(border.count(sections[a-1]->coords[sections[a-1]->polygons[q][1]-1])==0)
                    border.push_back(sections[a-1]->coords[sections[a-1]->polygons[q][1]-1]);
            }
            if(sections[a-1]->polygonMatrix[q][0]==-1 && sections[a-1]->polygonMatrix[q][2]==-1)
            {
                border.push_back(sections[a-1]->coords[sections[a-1]->polygons[q][0]-1]);
                if(border.count(sections[a-1]->coords[sections[a-1]->polygons[q][1]-1])==0)
                    border.push_back(sections[a-1]->coords[sections[a-1]->polygons[q][1]-1]);
                if(border.count(sections[a-1]->coords[sections[a-1]->polygons[q][2]-1])==0)
                    border.push_back(sections[a-1]->coords[sections[a-1]->polygons[q][2]-1]);
            }
        }
        else
        {
            if(sections[a-1]->polygonMatrix[q][0]==-1)
            {
                if(border.count(sections[a-1]->coords[sections[a-1]->polygons[q][0]-1])==0)
                    border.push_back(sections[a-1]->coords[sections[a-1]->polygons[q][0]-1]);
                if(border.count(sections[a-1]->coords[sections[a-1]->polygons[q][1]-1])==0)
                    border.push_back(sections[a-1]->coords[sections[a-1]->polygons[q][1]-1]);
            }
            if(sections[a-1]->polygonMatrix[q][1]==-1)
            {
                if(border.count(sections[a-1]->coords[sections[a-1]->polygons[q][1]-1])==0)
                    border.push_back(sections[a-1]->coords[sections[a-1]->polygons[q][1]-1]);
                if(border.count(sections[a-1]->coords[sections[a-1]->polygons[q][2]-1])==0)
                    border.push_back(sections[a-1]->coords[sections[a-1]->polygons[q][2]-1]);
            }
            if(sections[a-1]->polygonMatrix[q][2]==-1)
            {
                if(border.count(sections[a-1]->coords[sections[a-1]->polygons[q][0]-1])==0)
                    border.push_back(sections[a-1]->coords[sections[a-1]->polygons[q][0]-1]);
                if(border.count(sections[a-1]->coords[sections[a-1]->polygons[q][2]-1])==0)
                    border.push_back(sections[a-1]->coords[sections[a-1]->polygons[q][2]-1]);
            }
        }
        q=-1;
        for(int i=0;i<sections[a-1]->polygonMatrix.size();i++)
        {
            if(visited.count(i)==0 && sections[a-1]->polygonMatrix[i].count(-1)!=0)
            {
                if((sections[a-1]->polygonMatrix[i][0]==-1 && (sections[a-1]->coords[sections[a-1]->polygons[i][0]-1]==border[border.size()-1] ||
                                                              sections[a-1]->coords[sections[a-1]->polygons[i][1]-1]==border[border.size()-1])) ||
                   (sections[a-1]->polygonMatrix[i][1]==-1 && (sections[a-1]->coords[sections[a-1]->polygons[i][1]-1]==border[border.size()-1] ||
                                                              sections[a-1]->coords[sections[a-1]->polygons[i][2]-1]==border[border.size()-1])) ||
                   (sections[a-1]->polygonMatrix[i][2]==-1 && (sections[a-1]->coords[sections[a-1]->polygons[i][0]-1]==border[border.size()-1] ||
                                                              sections[a-1]->coords[sections[a-1]->polygons[i][2]-1]==border[border.size()-1])))
                {
                    q=i;
                    break;
                }
            }
        }
        if(q==-1)
            break;
    }
    for(int i=0;i<sections[a-1]->polygonMatrix.size();i++)
    {
        if(sections[a-1]->polygonMatrix[i].contains(-1) && visited.count(i)==0)
            return -1;
    }
    sections[a-1]->border=border;
    return 1;
}

Data OGLAreaWidget::makeSidesOfTract(int a, int b, int numer, bool inverted)
{
    QVector<QVector3D> border1,border2,border;
    QVector<QVector<unsigned int>> polygons;
    if(sections[a-1]->border.size()>=sections[b-1]->border.size())
    {
        border1=sections[b-1]->border;
        border2=sections[a-1]->border;
    }
    else
    {
        border1=sections[a-1]->border;
        border2=sections[b-1]->border;
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

void OGLAreaWidget::connecting(unsigned int a, unsigned int b)
{
    makePolygonMatrix(a);
    int i=makeBorder(a);
    if(i!=1)
    {
        QMessageBox::warning(this,"Warning","Bad section for creating tract. Maybe it consists of several parts.");
        return;
    }
    makePolygonMatrix(b);
    i=makeBorder(b);
    if(i!=1)
    {
        QMessageBox::warning(this,"Warning","Bad section for creating tract. Maybe it consists of several parts.");
        return;
    }
    QVector<QVector<unsigned int>> polygons;
    QVector<QVector3D> coords;
    int q;
    if(sections[a-1]->border.size()>=sections[b-1]->border.size())
        q=sections[a-1]->border.size();
    else
        q=sections[b-1]->border.size();
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
    MeshModel* obj=new MeshModel(vertexes,indexes,QImage(":/cube3.png"));
    obj->polygons=polygons;
    obj->coords=coords;
    tracks.append(obj);
    update();
}

void OGLAreaWidget::mousePressEvent(QMouseEvent *event)
{
    if(!cutFlag)
    {
        if(event->buttons()==Qt::LeftButton || event->buttons()==Qt::RightButton)
            mousePosition=QVector2D(event->localPos());
        event->accept();
    }
    else
    {
        if(event->buttons()==Qt::LeftButton)
        {
            mouseclick.push_back(QVector2D(event->localPos()));
            isCutting=true;
            z+=0.000001f;
            update();
        }
        if(event->buttons()==Qt::RightButton)
        {
            mousePosition=QVector2D(event->localPos());
            event->accept();
        }
    }
}

void OGLAreaWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(!cutFlag)
    {
        if(event->buttons()==Qt::LeftButton)
        {
            QVector2D diff=QVector2D(event->localPos())-mousePosition;
            mousePosition=QVector2D(event->localPos());
            float angle;
            QVector3D axis;
            if((mousePosition.x()<width()/20.0 && event->localPos().x()<width()/20.0)||
                  (mousePosition.x()>19.0*width()/20.0 && event->localPos().x()>19.0*width()/20.0))
            {
                if(mousePosition.x()<width()/20.0 && event->localPos().x()<width()/20.0)
                    angle=diff.y()/2.0;
                else
                    angle=-diff.y()/2.0;
                axis=QVector3D(0.0,0.0,1.0);
            }
            else
            {
                angle=diff.length()/2.0;
                axis=QVector3D(diff.y(),diff.x(),0.0);
            }
            rotation=QQuaternion::fromAxisAndAngle(axis,angle)*rotation;
            update();
        }
        if(event->buttons()==Qt::RightButton)
        {
            QVector2D diff=QVector2D(event->localPos())-mousePosition;
            mousePosition=QVector2D(event->localPos());
            x+=diff.x()/50;
            y-=diff.y()/50;
            update();
        }
    }
    else
    {
        if(event->buttons()!=Qt::RightButton)
            return;
        QVector2D diff=QVector2D(event->localPos())-mousePosition;
        if(diff.length()>10)
        {
            QVector2D p;
            if(diff.x()==0.0f && diff.y()<0)
                p=QVector2D(-1.0,0.0);
            if(diff.x()==0.0f && diff.y()>0)
                p=QVector2D(1.0,0.0);
            if(diff.y()==0.0f && diff.x()<0)
                p=QVector2D(0.0,-1.0);
            if(diff.y()==0.0f && diff.x()>0)
                p=QVector2D(0.0,1.0);
            else
            {
                p=QVector2D(1.0,-diff.x()/diff.y());
                p/=p.length();
                if(diff.x()*p.y()-diff.y()*p.x()<0)
                    p=p*(-1);
            }
            mousePosition=QVector2D(event->localPos());
            mouseclick.push_back(QVector2D(QVector2D(int(event->localPos().x()+5.0f*p.x()),int(event->localPos().y()+5.0f*p.y()))));
            mouseclick.push_back(QVector2D(QVector2D(int(event->localPos().x()-5.0f*p.x()),int(event->localPos().y()-5.0f*p.y()))));
            isCutting=true;
            z+=0.000001f;
            update();
        }
    }
}

void OGLAreaWidget::wheelEvent(QWheelEvent *event)
{
    if(!cutFlag)
    {
        if(event->delta()>0)
        {
            z+=0.25f;
        }
        else if(event->delta()<0)
        {
            z-=0.25f;
        }
        update();
    }
}
