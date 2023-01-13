#include "object.h"

Object::Object() :
    vertexBuffer(QOpenGLBuffer::VertexBuffer)
{
    countVertices = 0;
    scaleVec = { 1.0f, 1.0f, 1.0f };
}

Object::~Object()
{
    if (vertexBuffer.isCreated()) {
        vertexBuffer.destroy();
    }
    if (vao.isCreated()) {
        vao.destroy();
    }
}

void Object::init(const QVector<VertexData>& vertices, QOpenGLFunctions* functions)
{
    vao.create();
    QOpenGLVertexArrayObject::Binder vaoBinder(&vao);
    vertexBuffer.create();
    vertexBuffer.bind();
    vertexBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    vertexBuffer.allocate(vertices.constData(), vertices.size() * sizeof(VertexData));

    functions->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*) nullptr);
    functions->glEnableVertexAttribArray(0);

    functions->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    functions->glEnableVertexAttribArray(1);

    functions->glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    functions->glEnableVertexAttribArray(2);

    countVertices = vertices.size();
}

void Object::draw(QOpenGLShaderProgram* program, int primitive)
{
    QMatrix4x4 modelMatrix;
    modelMatrix.setToIdentity();
    modelMatrix.translate(translateVec);
    modelMatrix.rotate(rotation);
    modelMatrix.scale(scaleVec);
    program->setUniformValue("modelMatrix", modelMatrix);
    program->setUniformValue("material.ambient", material.ambient);
    program->setUniformValue("material.diffuse", material.diffuse);
    program->setUniformValue("material.specular", material.specular);
    program->setUniformValue("material.shininess", material.shininess);
    QOpenGLVertexArrayObject::Binder vaoBinder(&vao);
    glDrawArrays(primitive, 0, countVertices);
    vertexBuffer.release();
}

void Object::scale(QVector3D& v)
{
    scaleVec = v;
}

void Object::rotate(QQuaternion& q)
{
    rotation = q * rotation;
}

void Object::translate(QVector3D v)
{
    translateVec += v;
}

void Object::setPosition(QVector3D v)
{
    translateVec = v;
}

void Object::changeMaterialProperties(Material m)
{
    material = m;
}
