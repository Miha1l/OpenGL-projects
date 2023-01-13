#ifndef OBJECT_H
#define OBJECT_H

#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QVector3D>

struct VertexData {
    VertexData() {
    }

    VertexData(QVector3D p, QVector3D c, QVector3D n) :
        position(p), color(c), normal(n) {
    }

    QVector3D position;
    QVector3D color;
    QVector3D normal;
};

class Object
{
public:
    struct Material {
        QVector3D ambient;
        QVector3D diffuse;
        QVector3D specular;
        float shininess;
    };

    Object();
    ~Object();

    void init(const QVector<VertexData>& vertices, QOpenGLFunctions* functions);
    void draw(QOpenGLShaderProgram* program, int primitive);
    void scale(QVector3D& v);
    void rotate(QQuaternion& q);
    void translate(QVector3D v);
    void setPosition(QVector3D v);
    void changeMaterialProperties(Material m);

private:
    QOpenGLBuffer vertexBuffer;
    QOpenGLVertexArrayObject vao;

    QQuaternion rotation;
    QVector3D translateVec;
    QVector3D scaleVec;

    Material material;
    int countVertices;
};

#endif // OBJECT_H
