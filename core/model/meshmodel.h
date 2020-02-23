#ifndef SIMPLEOBJECT3D_H
#define SIMPLEOBJECT3D_H
# include <QOpenGLBuffer>
# include <QMatrix4x4>
# include <QOpenGLTexture>
# include <QVector2D>
# include <QOpenGLFunctions>
# include <QOpenGLShaderProgram>
# include "NrtlType.h"
struct VertexData
{
    VertexData(){}
    VertexData(QVector3D p,QVector2D t,QVector3D n)
    {
        position=p;
        texCoord=t;
        normal=n;
    }
    QVector3D position;
    QVector2D texCoord;//содержит текстурные координаты
    QVector3D normal;
};

class MeshModel
{
public:
    MeshModel();
    MeshModel(const QVector<VertexData> &vertData, const QVector<GLuint> &indexes, const QImage &texture);
    ~MeshModel();
    void init(const QVector<VertexData> &vertData, const QVector<GLuint> &indexes, const QImage &texture);
    void draw(QOpenGLShaderProgram *program, QOpenGLFunctions *functions,bool is_allign=false);
    void translate(const QVector3D &translateVector);
    void rotate(QQuaternion rotation);
    QMatrix4x4 getModelMatrix();
    void setModelMatrix(QMatrix4x4 a);
    void setModelMatrixToIdentity();
    void setPolygonMatrix(QVector<QVector<int>> a);
    QVector<QVector<unsigned int>> polygons;
    QVector<QVector<unsigned int>> texpolygons;
    QVector<QVector3D> coords;
    QVector<QVector2D> texcoords;
    QVector<QVector3D> border;
    QVector<QVector<int>> polygonMatrix;
    QImage image;
    QString name() { return _name; }
    void setName(const QString& mesh_name) { _name = mesh_name; }
    uint8_t transperancy() { return _transperancy; }
    void setTransperancy(uint8_t val) { _transperancy = val; }
    /*! \todo: Использовать наследование SectionModel::MeshModel */
    DataId step() { return _step_id; }
    void setStep(DataId id) { _step_id = id; }
    void setColor(QColor clr) { r = clr.red()/256.0; g = clr.green()/256.0; b = clr.blue()/256.0; }
    float r,g,b;
private:
    QOpenGLBuffer vertexBuffer, indexBuffer;
    QMatrix4x4 modelMatrix;
    QOpenGLTexture *mytexture;
    QString _name;
    uint8_t _transperancy;
    /*! \todo: Использовать наследование SectionModel::MeshModel */
    DataId _step_id;
};
#endif // SIMPLEOBJECT3D_H
