#ifndef MYOPENGL_H
#define MYOPENGL_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <GL/glu.h>
#include <GL/gl.h>
#include <ctime>

struct Color {
    double r;
    double g;
    double b;
    double a;
};

struct Point {
    double x;
    double y;
    Color color;
};

class MyOpenGL : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT
public:
    MyOpenGL(QWidget* parent = nullptr);
protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
private:
    typedef void (MyOpenGL::*DrawFunc)();
    std::string curPrimitive;
    std::map<std::string, DrawFunc> drawPrimitive;
    std::vector<Point> points;
    int x;
    int y;
    int width;
    int height;
    int alphaFunc;
    float ref;
    int sfactor;
    int dfactor;
    void generatePoints();
    Color getRandomColor();
    Point getRandomPoint();
    void placePoint(int i);
    void drawPoints();
    void drawLines();
    void drawLineStrip();
    void drawLineLoop();
    void drawTriangles();
    void drawTriangleStrip();
    void drawTriangleFan();
    void drawQuads();
    void drawQuadStrip();
    void drawPolygon();
public slots:
    void changePrimitive(std::string s);
    void changeScissorsTest(int x, int y, int w, int h);
    void changeAlphaTest(std::string func, int ref);
    void changeBlendTest(std::string sfactor, std::string dfactor);
};

#endif // MYOPENGL_H
