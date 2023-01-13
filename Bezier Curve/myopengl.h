#ifndef MYOPENGL_H
#define MYOPENGL_H

#include <QMouseEvent>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <GL/gl.h>
#include <GL/glu.h>

struct Point {
    float x;
    float y;
};

class MyOpenGL : public QOpenGLWidget, private QOpenGLFunctions {
    Q_OBJECT
public:
    MyOpenGL(QWidget* parent = nullptr);
protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
private:
    std::vector<Point> points;
    Point bezier(float t);
    void drawPoints();
    void drawSpline();
    int pressedPoint;
};

#endif // MYOPENGL_H
