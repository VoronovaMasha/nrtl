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
    glClearColor(0.0f,0.0f,0.0f,1.0f);//очистка экрана
    glEnable(GL_DEPTH_TEST);//включить буфер глубины
    glEnable(GL_CULL_FACE);//задние грани не будут рисоваться
    initShaders();//загрузка исх кодов шейдеров и компиляция их
}
void OGLAreaWidget::resizeGL(int w,int h)
{
    float aspect=(float)w/(float)h;
    projectionMatrix.setToIdentity();//сделали единичной
    projectionMatrix.perspective(45,aspect,0.01f,50.0f);//2 числа это плоскости отсечения
}
void OGLAreaWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);//очищаем буферцвета и глубины
    //нужно загрузить матрицу проекций  и модельно видовую матрицу в shader
    ViewMatrix1.setToIdentity();
    ViewMatrix2.setToIdentity();
    ViewMatrix1.translate(x,y,z);//отодвигаем матрицу на 5
    ViewMatrix2.rotate(rotation);
    ViewMatrix=ViewMatrix1*ViewMatrix2;
    program.bind();
    program.setUniformValue("u_projectionMatrix",projectionMatrix);
    program.setUniformValue("u_viewMatrix",ViewMatrix);
    program.setUniformValue("u_lightPosition",QVector4D(0.0,0.0,0.0,1.0));
    program.setUniformValue("u_lightPower",6.0f);

    for(auto model : MeshData::get())
    {
        model->draw(&program, context()->functions());
    }

/*    glEnable(GL_ALPHA_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    if(tr_model!=100)
    {
        program.setUniformValue("u_alpha",tr_model/100.0f);
        for(int i=0;i<objects.size();i++)
        {
            objects[i]->draw(&program,context()->functions());
        }
    }
    if(tr_section!=100)
    {
        program.setUniformValue("u_alpha",tr_section/100.0f);
        for(int i=0;i<sections.size();i++)
        {
            sections[i]->draw(&program,context()->functions());
        }
    }
    if(tr_tract!=100)
    {
        program.setUniformValue("u_alpha",tr_tract/100.0f);
        for(int i=0;i<tracks.size();i++)
        {
            tracks[i]->draw(&program,context()->functions());
        }
    }
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    if(tr_model==100)
    {
        program.setUniformValue("u_alpha",1);
        for(int i=0;i<objects.size();i++)
        {
            objects[i]->draw(&program,context()->functions());
        }
    }
    if(tr_section==100)
    {
        program.setUniformValue("u_alpha",1);
        for(int i=0;i<sections.size();i++)
        {
            sections[i]->draw(&program,context()->functions());
        }
    }
    if(tr_tract==100)
    {
        program.setUniformValue("u_alpha",1);
        for(int i=0;i<tracks.size();i++)
        {
            tracks[i]->draw(&program,context()->functions());
        }
    }*/
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
    if(!program.addShaderFromSourceFile(QOpenGLShader::Vertex,":/ogl/shaders/vshader.vsh"))//здесь + компиляция
        close();//закрыть приложение
    if(!program.addShaderFromSourceFile(QOpenGLShader::Fragment,":/ogl/shaders/fshader.fsh"))
        close();
    if(!program.link())//объединение шейдеров в один и прокидывает переменные varying
        close();
}


/*
void OGLAreaWidget::loadObj(QString path)
{
    try
    {
        QFile objFile(path);
        QVector<QVector3D> coords;
        QVector<QVector2D> texcoords;
        QVector<VertexData> vertexes;
        QVector<GLuint> indexes;
        QVector<QVector<unsigned int>> polygons;
        if(!objFile.exists())
        {
            QMessageBox::warning(this,"Warning","Did not found \".obj\" file.");
            return;
        }
        objFile.open(QIODevice::ReadOnly);
        QTextStream input(&objFile);
        QString picture;
        bool is_picture=false;
        while(!input.atEnd())
        {
            QString str=input.readLine();
            QStringList list=str.split(" ");
            if(list[0]=="mtllib")
            {
                if(list.size()!=2)
                {
                    QMessageBox::warning(this,"Warning","Bad \".obj\" file (problems in string \"mtllib ...\").");
                    return;
                }
                QFileInfo info(path);
                if(list[1].startsWith("./"))
                    list[1].remove(0,2);
                QFile mtlFile(info.absolutePath()+"/"+list[1]);
                if(!mtlFile.exists())
                {
                    QMessageBox::warning(this,"Warning","Did not found \".mtl\" file.");
                    return;
                }
                mtlFile.open(QIODevice::ReadOnly);
                QTextStream input2(&mtlFile);
                while(!input2.atEnd())
                {
                    QString str2=input2.readLine();
                    QStringList list2=str2.split(" ");
                    if(list2[0]=="map_Kd")
                    {
                        if(list2.size()!=2)
                        {
                            QMessageBox::warning(this,"Warning","Bad \".mtl\" file (problems in string \"map_Kd ...\").");
                            return;
                        }
                        if(list2[1].startsWith("./"))
                            list2[1].remove(0,2);
                        picture=info.absolutePath()+"/"+list2[1];
                        is_picture=true;
                        mtlFile.close();
                        break;
                    }
                }
                continue;
            }
            if(list[0]=="v")
            {
                if(list.size()!=4 && list.size()!=7)
                {
                    QMessageBox::warning(this,"Warning","Bad \".obj\" file (problems in string \"v ...\").");
                    return;
                }
                coords.append(QVector3D((list[1].toFloat()/50.f),(list[2].toFloat()/50.f),(list[3].toFloat()/50.f)));
                continue;
            }
            if(list[0]=="vt")
            {
                if(list.size()!=3)
                {
                    QMessageBox::warning(this,"Warning","Bad \".obj\" file (problems in string \"vt ...\").");
                    return;
                }
                texcoords.append(QVector2D(list[1].toFloat(),list[2].toFloat()));
                continue;
            }
            if(list[0]=="f")
            {
                if(list.size()!=4)
                {
                    QMessageBox::warning(this,"Warning","Bad \".obj\" file (problems in string \"f ...\").");
                    return;
                }
                QStringList vert1=list[1].split("/");
                QStringList vert2=list[2].split("/");
                QStringList vert3=list[3].split("/");
                QVector<unsigned int> tmp,ttmp;
                tmp.push_back(vert1[0].toUInt());
                tmp.push_back(vert2[0].toUInt());
                tmp.push_back(vert3[0].toUInt());
                polygons.push_back(tmp);
                ttmp.push_back(vert1[1].toUInt());
                ttmp.push_back(vert2[1].toUInt());
                ttmp.push_back(vert3[1].toUInt());
                if(texcoords.size()==0 || vert1.size()==1 || vert2.size()==1 || vert3.size()==1)
                {
                    is_picture=false;
                    vertexes.append(VertexData(coords[tmp[0]-1],QVector2D(0.0,1.0),QVector3D(1.0,0.0,0.0)));
                    vertexes.append(VertexData(coords[tmp[1]-1],QVector2D(0.0,0.0),QVector3D(1.0,0.0,0.0)));
                    vertexes.append(VertexData(coords[tmp[2]-1],QVector2D(1.0,1.0),QVector3D(1.0,0.0,0.0)));
                }
                if(texcoords.size()!=0 && vert1.size()>1 && vert2.size()>1 && vert3.size()>1)
                {
                    vertexes.append(VertexData(coords[tmp[0]-1],texcoords[ttmp[0]-1],QVector3D(1.0,0.0,0.0)));
                    vertexes.append(VertexData(coords[tmp[1]-1],texcoords[ttmp[1]-1],QVector3D(1.0,0.0,0.0)));
                    vertexes.append(VertexData(coords[tmp[2]-1],texcoords[ttmp[2]-1],QVector3D(1.0,0.0,0.0)));
                }
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
                if(texcoords.size()==0 || vert1.size()==1 || vert2.size()==1 || vert3.size()==1)
                {
                    is_picture=false;
                    vertexes.append(VertexData(coords[tmp[2]-1],QVector2D(0.0,1.0),QVector3D(1.0,0.0,0.0)));
                    vertexes.append(VertexData(coords[tmp[1]-1],QVector2D(0.0,0.0),QVector3D(1.0,0.0,0.0)));
                    vertexes.append(VertexData(coords[tmp[0]-1],QVector2D(1.0,1.0),QVector3D(1.0,0.0,0.0)));
                }
                if(texcoords.size()!=0 && vert1.size()>1 && vert2.size()>1 && vert3.size()>1)
                {
                    vertexes.append(VertexData(coords[tmp[2]-1],texcoords[ttmp[2]-1],QVector3D(1.0,0.0,0.0)));
                    vertexes.append(VertexData(coords[tmp[1]-1],texcoords[ttmp[1]-1],QVector3D(1.0,0.0,0.0)));
                    vertexes.append(VertexData(coords[tmp[0]-1],texcoords[ttmp[0]-1],QVector3D(1.0,0.0,0.0)));
                }
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
                continue;
            }
        }
        objFile.close();
        MeshModel* obj;
        if(is_picture)
            obj=new MeshModel(vertexes,indexes,QImage(picture));
        else
            obj=new MeshModel(vertexes,indexes,QImage(":/cube.png"));
        obj->polygons=polygons;
        obj->coords=coords;
        objects.append(obj);
        centerizeObj(objects.size()-1);
        update();
    }
    catch(...)
    {
        QMessageBox::warning(this,"Warning","There was an error during loading the model. Maybe the reason is the limits of your RAM.");
        return;
    }
}

void OGLAreaWidget::centerizeObj(int d)
{
    if(d<0 || d>objects.size()-1)
    {
        QMessageBox::warning(this,"Warning","Invalid parameter in function \"centerizeObj\".");
        return;
    }
    QVector<QVector3D> coords=objects[d]->coords;
    double x=0.f,y=0.f,z=0.f;
    for(int i=0;i<coords.size();i++)
    {
        x+=coords[i].x()/coords.size();
        y+=coords[i].y()/coords.size();
        z+=coords[i].z()/coords.size();
    }
    objects[d]->translate(QVector3D(-x,-y,-z));
}

/*bool checkNormal(QVector3D a, QVector3D b, QVector3D c)
{
    QVector3D n1=QVector3D(c.x()-a.x(),c.y()-a.y(),c.z()-a.z());
    QVector3D n2=QVector3D(b.x()-a.x(),b.y()-a.y(),b.z()-a.z());
    QVector3D n=QVector3D(n1.y()*n2.z()-n1.z()*n2.y(),n1.z()*n2.x()-n1.x()*n2.z(),n1.x()*n2.y()-n1.y()*n2.x());
    QVector3D norm=QVector3D(-n.x()/n.length(),-n.y()/n.length(),-n.z()/n.length());
    float s=QVector3D::dotProduct(norm,QVector3D(0.0,0.0,1.0));
    if(s>0.0f)
        return true;
    else
        return false;
}*/

void OGLAreaWidget::cutter()
{
    if(tr_cutVertexes.size()>0)
    {
        cutVertexes=cutVertexes+tr_cutVertexes;
        tr_cutVertexes.clear();
    }
    if(cutVertexes.size()<3)
    {
        QMessageBox::warning(this,"Warning","You must choose at least 3 vertexes.");
        return;
    }
    int Q=objects.size();
    for(int i=0;i<Q;i++)
    {
        QVector<QVector3D> coords;
        QVector<VertexData> vertexes;
        QVector<GLuint> indexes;
        QVector<QVector<unsigned int>> polygons;
        for(int j=0;j<objects[i]->polygons.size();j++)
        {
            QVector<unsigned int> tmp=objects[i]->polygons[j];
            QVector3D tmp1=ViewMatrix2*objects[i]->getModelMatrix()*objects[i]->coords[tmp[0]-1];
            QVector3D tmp2=ViewMatrix2*objects[i]->getModelMatrix()*objects[i]->coords[tmp[1]-1];
            QVector3D tmp3=ViewMatrix2*objects[i]->getModelMatrix()*objects[i]->coords[tmp[2]-1];
            if(checkBelonging(tmp1.x(),tmp1.y())&&
                    checkBelonging(tmp2.x(),tmp2.y())&&
                    checkBelonging(tmp3.x(),tmp3.y())&&
                    checkDistance(tmp1,tmp2,tmp3))
            {
                polygons.append(tmp);
                vertexes.append(VertexData(QVector3D((objects[i]->getModelMatrix()*objects[i]->coords[tmp[0]-1]).x(),(objects[i]->getModelMatrix()*objects[i]->coords[tmp[0]-1]).y(),(objects[i]->getModelMatrix()*objects[i]->coords[tmp[0]-1]).z()),QVector2D(0.0,1.0),QVector3D(1.0,0.0,0.0)));
                vertexes.append(VertexData(QVector3D((objects[i]->getModelMatrix()*objects[i]->coords[tmp[1]-1]).x(),(objects[i]->getModelMatrix()*objects[i]->coords[tmp[1]-1]).y(),(objects[i]->getModelMatrix()*objects[i]->coords[tmp[1]-1]).z()),QVector2D(0.0,0.0),QVector3D(1.0,0.0,0.0)));
                vertexes.append(VertexData(QVector3D((objects[i]->getModelMatrix()*objects[i]->coords[tmp[2]-1]).x(),(objects[i]->getModelMatrix()*objects[i]->coords[tmp[2]-1]).y(),(objects[i]->getModelMatrix()*objects[i]->coords[tmp[2]-1]).z()),QVector2D(1.0,1.0),QVector3D(1.0,0.0,0.0)));
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
                vertexes.append(VertexData(QVector3D((objects[i]->getModelMatrix()*objects[i]->coords[tmp[2]-1]).x(),(objects[i]->getModelMatrix()*objects[i]->coords[tmp[2]-1]).y(),(objects[i]->getModelMatrix()*objects[i]->coords[tmp[2]-1]).z()),QVector2D(1.0,1.0),QVector3D(1.0,0.0,0.0)));
                vertexes.append(VertexData(QVector3D((objects[i]->getModelMatrix()*objects[i]->coords[tmp[1]-1]).x(),(objects[i]->getModelMatrix()*objects[i]->coords[tmp[1]-1]).y(),(objects[i]->getModelMatrix()*objects[i]->coords[tmp[1]-1]).z()),QVector2D(0.0,0.0),QVector3D(1.0,0.0,0.0)));
                vertexes.append(VertexData(QVector3D((objects[i]->getModelMatrix()*objects[i]->coords[tmp[0]-1]).x(),(objects[i]->getModelMatrix()*objects[i]->coords[tmp[0]-1]).y(),(objects[i]->getModelMatrix()*objects[i]->coords[tmp[0]-1]).z()),QVector2D(0.0,1.0),QVector3D(1.0,0.0,0.0)));
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
            coords=objects[i]->coords;
            for(int j=0;j<coords.size();j++)
            {
                coords[j]=objects[i]->getModelMatrix()*coords[j];
            }
            MeshModel* obj=new MeshModel(vertexes,indexes,QImage(":/cube2.png"));
            obj->polygons=polygons;
            obj->coords=coords;
            sections.append(obj);
        }
    }
    cutVertexes.clear();
    update();
}
float isLeft(float _startX, float _startY, float _endX, float _endY, float _pointX, float _pointY)
{
    return ((_endX - _startX) * (_pointY - _startY) - (_pointX - _startX) * (_endY - _startY));
}
bool OGLAreaWidget::checkBelonging(float _pointX, float _pointY)
{
    int windingNumber = 0;
    float startX = 0;
    float startY = 0;
    float endX = 0;
    float endY = 0;
    int count = cutVertexes.size();
    for (int i = 1; i <= count; i++)
    {
        startX = cutVertexes[i-1].x();
        startY = cutVertexes[i-1].y();
        if (i == count)
        {
            endX = cutVertexes[0].x();
            endY = cutVertexes[0].y();
        }
        else
        {
            endX = cutVertexes[i].x();
            endY = cutVertexes[i].y();
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

bool OGLAreaWidget::checkDistance(QVector3D a, QVector3D b, QVector3D c)
{
    float s=0.0;
    for(int i=0;i<cutVertexes.size();i++)
    {
        for(int j=0;j<cutVertexes.size();j++)
        {
            QVector3D tmp=cutVertexes[i]-cutVertexes[j];
            if(s<tmp.length())
                s=tmp.length();
        }
    }
    for(int i=0;i<cutVertexes.size();i++)
    {
        if((cutVertexes[i]-a).length()>s || (cutVertexes[i]-b).length()>s || (cutVertexes[i]-c).length()>s)
            return false;
    }
    return true;
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

void OGLAreaWidget::alligning(QVector<QVector3D> dst, QVector<QVector3D> src,int c, int d)
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
    matr=objects[c]->getModelMatrix()*matr;
    objects[d]->setModelMatrix(matr);
    update();
}

void OGLAreaWidget::transparent(int what)
{
    if(what==1)
    {
        if(tr_model==0)
            tr_model=100;
        else
            tr_model-=10;
    }
    if(what==2)
    {
        if(tr_section==0)
            tr_section=100;
        else
            tr_section-=10;
    }
    if(what==3)
    {
        if(tr_tract==0)
            tr_tract=100;
        else
            tr_tract-=10;
    }
    update();
}

QVector<MeshModel *> OGLAreaWidget::getAllModels()
{
    return objects;
}

int OGLAreaWidget::getSectionsSize()
{
    return sections.size();
}

int OGLAreaWidget::getObjectsSize()
{
    return objects.size();
}

bool OGLAreaWidget::canCut()
{
    if(tr_model!=100)
        return false;
    else
        return true;
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
            float angle=diff.length()/2.0;//длина перемещения мыши/2 (/2 чтобы поворот не был слишком быстрым)
            QVector3D axis=QVector3D(diff.y(),diff.x(),0.0);//ось вращения(перпендик линии перемещения мыши)
                                    //по правилам надо поменять коорд местами и у одной знак поменять но в opengl др ск
                                    //поэтому не надо знак менять
            rotation=QQuaternion::fromAxisAndAngle(axis,angle)*rotation;
            update();// обновить текущее изображение
        }
        if(event->buttons()==Qt::RightButton)
        {
            QVector2D diff=QVector2D(event->localPos())-mousePosition;
            mousePosition=QVector2D(event->localPos());
            x+=diff.x()/50;
            y-=diff.y()/50;
            update();// обновить текущее изображение
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

/*b--;
a--;
if(tracks.size()>=a)
{
    QVector<QVector3D> coords=tracks[a]->coords;
    QVector<QString> faces=tracks[a]->faces;
    QVector<QVector3D> scoords=sections[b]->coords;
    QVector<QString> sfaces=sections[b]->faces;
    QVector<QVector3D> sborder=sections[b]->border;
    QVector<VertexData> vertexes;
    QVector<GLuint> indexes;
    int s=coords.size();
    for(int i=0;i<scoords.size();i++)
    {
        coords.push_back(QVector3D(scoords[i].x(),scoords[i].y(),scoords[i].z()-0.25f));
    }
    for(int i=0;i<sfaces.size();i++)
    {
        QString str=sfaces[i];
        QStringList list=str.split(" ");
        QStringList vert1=list[1].split("/");
        QStringList vert2=list[2].split("/");
        QStringList vert3=list[3].split("/");
        long int q=vert1[0].toLong()+s;
        long int w=vert2[0].toLong()+s;
        long int e=vert3[0].toLong()+s;
        QString res="f "+QString::number(q)+" "+QString::number(w)+" "+QString::number(e);
        faces.push_back(res);
    }
    for(int j=0;j<faces.size();j++)
    {
        QString str=faces[j];
        QStringList list=str.split(" ");
        QStringList vert1=list[1].split("/");
        QStringList vert2=list[2].split("/");
        QStringList vert3=list[3].split("/");
        vertexes.append(VertexData(coords[vert1[0].toLong()-1],QVector2D(0.0,1.0),QVector3D(1.0,0.0,0.0)));
        vertexes.append(VertexData(coords[vert2[0].toLong()-1],QVector2D(0.0,0.0),QVector3D(1.0,0.0,0.0)));
        vertexes.append(VertexData(coords[vert3[0].toLong()-1],QVector2D(1.0,1.0),QVector3D(1.0,0.0,0.0)));
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
        vertexes.append(VertexData(coords[vert3[0].toLong()-1],QVector2D(0.0,1.0),QVector3D(1.0,0.0,0.0)));
        vertexes.append(VertexData(coords[vert2[0].toLong()-1],QVector2D(0.0,0.0),QVector3D(1.0,0.0,0.0)));
        vertexes.append(VertexData(coords[vert1[0].toLong()-1],QVector2D(1.0,1.0),QVector3D(1.0,0.0,0.0)));
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
    SimpleObject3D* obj;
    obj=new SimpleObject3D(vertexes,indexes,QImage(":/cube3.png"));
    obj->faces=faces;
    obj->coords=coords;
    obj->border=sborder;
    tracks.append(obj);
}
else
{
    QVector<QVector3D> coords=sections[b]->coords;
    QVector<VertexData> vertexes;
    QVector<GLuint> indexes;
    QVector<QString> faces=sections[b]->faces;
    QVector<QVector3D> border=sections[b]->border;
    for(int j=0;j<coords.size();j++)
    {
        coords[j].setZ(coords[j].z()-0.25f);
    }
    for(int j=0;j<border.size();j++)
    {
        border[j].setZ(border[j].z()-0.25f);
    }
    for(int j=0;j<faces.size();j++)
    {
        QString str=faces[j];
        QStringList list=str.split(" ");
        QStringList vert1=list[1].split("/");
        QStringList vert2=list[2].split("/");
        QStringList vert3=list[3].split("/");
        vertexes.append(VertexData(coords[vert1[0].toLong()-1],QVector2D(0.0,1.0),QVector3D(1.0,0.0,0.0)));
        vertexes.append(VertexData(coords[vert2[0].toLong()-1],QVector2D(0.0,0.0),QVector3D(1.0,0.0,0.0)));
        vertexes.append(VertexData(coords[vert3[0].toLong()-1],QVector2D(1.0,1.0),QVector3D(1.0,0.0,0.0)));
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
        vertexes.append(VertexData(coords[vert3[0].toLong()-1],QVector2D(0.0,1.0),QVector3D(1.0,0.0,0.0)));
        vertexes.append(VertexData(coords[vert2[0].toLong()-1],QVector2D(0.0,0.0),QVector3D(1.0,0.0,0.0)));
        vertexes.append(VertexData(coords[vert1[0].toLong()-1],QVector2D(1.0,1.0),QVector3D(1.0,0.0,0.0)));
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
    SimpleObject3D* obj;
    obj=new SimpleObject3D(vertexes,indexes,QImage(":/cube3.png"));
    obj->faces=faces;
    obj->coords=coords;
    obj->border=border;
    tracks.append(obj);
}*/
