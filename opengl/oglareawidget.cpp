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
