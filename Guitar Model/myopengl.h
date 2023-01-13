#ifndef MYOPENGL_H
#define MYOPENGL_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QWheelEvent>
#include <QOpenGLFramebufferObject>
#include "camera.h"
#include "object.h"

class MyOpenGL : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    struct Options {
        QVector3D scale;
        int projection;
        bool drawInvisible;
        bool drawCoordinates;
    };

    struct Light {
        int type;
        QVector3D position;
        QVector3D ambient;
        QVector3D diffuse;
        QVector3D specular;
    };

    MyOpenGL(QWidget *parent);
    ~MyOpenGL();

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private:
    void initShaders();
    void initScene();
    void initObject();
    void initCoord();
    void initWall();
    void initLamp();
    void drawScene();

    QOpenGLShaderProgram objectShaderProgram;
    QOpenGLShaderProgram depthShaderProgram;
    QOpenGLShaderProgram lampShaderProgram;

    QMatrix4x4 lightMatrix;
    QMatrix4x4 shadowLightMatrix;
    float shadowPointCloudFilteringQuality;
    QVector2D mousePosition;

    float lightRotateX;
    float lightRotateY;

    Camera camera;
    Options options;
    Light light;
    Object coordinates;
    Object wall;
    Object lamp;
    QVector<Object*> guitarParts;

    QOpenGLFramebufferObject* depthBuffer;
    quint32 fbHeight;
    quint32 fbWidth;

public slots:
    void changeDisplayOptions(int index, bool invisible, bool coordinates);
    void changeObjectOptions(QVector3D& scaleVec, QVector3D& translateVec);
    void changeObjectMaterialOprions(QVector3D ambient, QVector3D diffuse, QVector3D specular, float shininess);
    void changeLightOptions(int type, QVector3D position, QVector3D ambient, QVector3D diffuse);
};

#endif // MYOPENGL_H
