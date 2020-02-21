#include "meshmodel.h"
#include <QOpenGLTexture>
#include <GL/gl.h>
#include <GL/glu.h>

MeshModel::MeshModel():indexBuffer(QOpenGLBuffer::IndexBuffer),mytexture(0)
{
    _step_id = NONE;
    _transperancy=100;
    r=0;
    g=0;
    b=0;
}
MeshModel::MeshModel(const QVector<VertexData> &vertData, const QVector<GLuint> &indexes, const QImage &texture):indexBuffer(QOpenGLBuffer::IndexBuffer),mytexture(0)
{
    _step_id = NONE;
    _transperancy=100;
    r=0;
    g=0;
    b=0;
    image=texture;
    init(vertData,indexes,texture);
}

MeshModel::~MeshModel()
{
    if(vertexBuffer.isCreated())
        vertexBuffer.destroy();
    if(indexBuffer.isCreated())
        indexBuffer.destroy();
    if(mytexture!=0)
    {
        if(mytexture->isCreated())
            mytexture->destroy();
    }
}

void MeshModel::init(const QVector<VertexData> &vertData, const QVector<GLuint> &indexes, const QImage &texture)
{
    if(vertexBuffer.isCreated())//если вызывается неск раз то нужно удалять предыдущ буфер
        vertexBuffer.destroy();
    if(indexBuffer.isCreated())
        indexBuffer.destroy();
    if(mytexture!=0)
    {
        if(mytexture->isCreated())
        {
            delete mytexture;
            mytexture=0;
        }
    }
    vertexBuffer.create();
    vertexBuffer.bind();
    vertexBuffer.allocate(vertData.constData(),vertData.size()*sizeof(VertexData));
    vertexBuffer.release();

    indexBuffer.create();
    indexBuffer.bind();
    indexBuffer.allocate(indexes.constData(),indexes.size()*sizeof(GLuint));
    indexBuffer.release();

    mytexture = new QOpenGLTexture(texture.mirrored());

    mytexture->setMinificationFilter(QOpenGLTexture::Nearest);
    mytexture->setMagnificationFilter(QOpenGLTexture::Linear);
    mytexture->setWrapMode(QOpenGLTexture::Repeat);

    modelMatrix.setToIdentity();
}

void MeshModel::draw(QOpenGLShaderProgram *program, QOpenGLFunctions *functions,bool is_allign)
{
    if(_transperancy!=100 && !is_allign)
    {
        glEnable(GL_ALPHA_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);
    }
    if(!vertexBuffer.isCreated()|| !indexBuffer.isCreated())
        return;
    mytexture->bind(0);
    program->setUniformValue("u_texture",0); //0-номер текстуры которая будет отрисовываться
    program->setUniformValue("u_modelMatrix", modelMatrix);
    program->setUniformValue("u_r", r);
    program->setUniformValue("u_g", g);
    program->setUniformValue("u_b", b);
    if(_transperancy!=100 && !is_allign )
        program->setUniformValue("u_alpha", _transperancy/100.0f);
    else
        program->setUniformValue("u_alpha", 1);
    vertexBuffer.bind();
    int offset=0;

    int vertLoc=program->attributeLocation("a_position");//получаем расположение необходимого атрибута
    program->enableAttributeArray(vertLoc);//включаем использование атрибута
    program->setAttributeBuffer(vertLoc,GL_FLOAT,offset,3,sizeof(VertexData));

    offset+=sizeof(QVector3D);

    int texLoc=program->attributeLocation("a_texcoord");//получаем расположение необходимого атрибута
    program->enableAttributeArray(texLoc);//включаем использование атрибута
    program->setAttributeBuffer(texLoc,GL_FLOAT,offset,2,sizeof(VertexData));

    offset+=sizeof(QVector2D);

    int normLoc=program->attributeLocation("a_normal");//получаем расположение необходимого атрибута
    program->enableAttributeArray(normLoc);//включаем использование атрибута
    program->setAttributeBuffer(normLoc,GL_FLOAT,offset,3,sizeof(VertexData));

    indexBuffer.bind();
    functions->glDrawElements(GL_TRIANGLES,indexBuffer.size(),GL_UNSIGNED_INT,nullptr);
    vertexBuffer.release();
    indexBuffer.release();
    mytexture->release();
    if(_transperancy!=100 && !is_allign )
    {
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
    }
}

void MeshModel::translate(const QVector3D &translateVector)
{
    modelMatrix.translate(translateVector);
}

void MeshModel::rotate(QQuaternion rotation)
{
    modelMatrix.rotate(rotation);
}

QMatrix4x4 MeshModel::getModelMatrix()
{
    return modelMatrix;
}

void MeshModel::setModelMatrix(QMatrix4x4 a)
{
    modelMatrix=a;
}

void MeshModel::setModelMatrixToIdentity()
{
    modelMatrix.setToIdentity();
}

void MeshModel::setPolygonMatrix(QVector<QVector<int> > a)
{
    polygonMatrix=a;
}
\




