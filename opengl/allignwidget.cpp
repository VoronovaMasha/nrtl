#include "allignwidget.h"
#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <QOpenGLExtraFunctions>
#include <QMessageBox>
AllignWidget::AllignWidget(QWidget *parent): OGLAreaWidget(parent)
{
}

void AllignWidget::paintGL()
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
    program.setUniformValue("u_lightPower",3*abs(z));
    mesh->draw(&program,context()->functions(),true);
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
                cutVertexes.append(mesh->getModelMatrix().inverted()*ViewMatrix2.inverted()*result);
            if(i==1)
                tr_cutVertexes.push_front(mesh->getModelMatrix().inverted()*ViewMatrix2.inverted()*result);
        }
        mouseclick.clear();
        mFBO->release();
        isCutting=false;
    }
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    for(int i=0;i<cutVertexes.size();i++)
    {
        glPointSize(6);
        glBegin(GL_POINTS);
        glVertex3d((mesh->getModelMatrix()*cutVertexes[i]).x(),(mesh->getModelMatrix()*cutVertexes[i]).y(),(mesh->getModelMatrix()*cutVertexes[i]).z());
        glEnd();
        for(int j=1;j<i+2;j++)
        {
            glLineWidth(2);
            glBegin(GL_LINE_LOOP);
            glVertex3d((ViewMatrix2.inverted()*((ViewMatrix2*mesh->getModelMatrix()*cutVertexes[i])+QVector3D(j*0.02,0.04,0))).x(),(ViewMatrix2.inverted()*((ViewMatrix2*mesh->getModelMatrix()*cutVertexes[i])+QVector3D(j*0.02,0.04,0))).y(),(ViewMatrix2.inverted()*((ViewMatrix2*mesh->getModelMatrix()*cutVertexes[i])+QVector3D(j*0.02,0.04,0))).z());
            glVertex3d((ViewMatrix2.inverted()*((ViewMatrix2*mesh->getModelMatrix()*cutVertexes[i])+QVector3D(j*0.02,-0.04,0))).x(),(ViewMatrix2.inverted()*((ViewMatrix2*mesh->getModelMatrix()*cutVertexes[i])+QVector3D(j*0.02,-0.04,0))).y(),(ViewMatrix2.inverted()*((ViewMatrix2*mesh->getModelMatrix()*cutVertexes[i])+QVector3D(j*0.02,-0.04,0))).z());
            glEnd();
        }
    }
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    program.release();
}

void AllignWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(event->buttons()==Qt::LeftButton)
    {
        mouseclick.push_back(QVector2D(event->localPos()));
        isCutting=true;
        z+=0.000001f;
        update();
    }
}
