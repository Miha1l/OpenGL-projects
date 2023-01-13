#include "myopengl.h"

MyOpenGL::MyOpenGL(QWidget* parent)
    : QOpenGLWidget(parent) {
    points = {
        {0.1, 0.1},
        {0.5, 0.9},
        {0.9, 0.1}
    };
    pressedPoint = -1;
}

void MyOpenGL::initializeGL() {
    initializeOpenGLFunctions();
}

void MyOpenGL::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 1, 0, 1, 0, 1);
}

void MyOpenGL::paintGL() {
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    drawPoints();
    drawSpline();
}

void MyOpenGL::drawPoints() {
    glPointSize(10);
    glBegin(GL_POINTS);
    glColor4d(1, 1, 1, 1);
    for (auto& point: points) {
        glVertex2d(point.x, point.y);
    }
    glEnd();
    glPointSize(1);
    glBegin(GL_LINE_STRIP);
    glColor4d(0.9, 0.9, 0.7, 1);
    for (auto& point: points) {
        glVertex2d(point.x, point.y);
    }
    glEnd();
}

void MyOpenGL::drawSpline() {
    glBegin(GL_LINE_STRIP);
    glColor4d(1, 0, 0, 1);
    for (float t = 0; t <= 1; t += 0.01) {
        Point p = bezier(t);
        glVertex2d(p.x, p.y);
    }
    glEnd();
}

Point MyOpenGL::bezier(float t) {
    float x = pow((1 - t), 2) * points[0].x + 2 * t * (1 - t) * points[1].x + pow(t, 2) * points[2].x;
    float y = pow((1 - t), 2) * points[0].y + 2 * t * (1 - t) * points[1].y + pow(t, 2) * points[2].y;
    return { x, y };
}

void MyOpenGL::mousePressEvent(QMouseEvent *event) {
    if (event->buttons() == Qt::LeftButton) {
        int h = height();
        int w = width();
        float x = event->position().x() * 1.0 / w;
        float y = (h - event->position().y() - 1) * 1.0 / h;
        for (int i = 0; i < points.size(); i++) {
            if (abs(x - points[i].x) < 0.02 and abs(y - points[i].y) < 0.02) {
                pressedPoint = i;
                break;
            }
        }
    }
}

void MyOpenGL::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() == Qt::LeftButton and pressedPoint != -1) {
        int h = height();
        int w = width();
        points[pressedPoint].x = event->position().x() * 1.0 / w;
        points[pressedPoint].y = (h - event->position().y() - 1) * 1.0 / h;
        update();
    }
}

void MyOpenGL::mouseReleaseEvent(QMouseEvent *event) {
    pressedPoint = -1;
}
