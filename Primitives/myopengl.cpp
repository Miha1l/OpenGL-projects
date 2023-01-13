#include "myopengl.h"

MyOpenGL::MyOpenGL(QWidget* parent)
    : QOpenGLWidget(parent) {
    srand(time(0));
    curPrimitive = "GL_POINTS";
    drawPrimitive = {
        {"GL_POINTS", &MyOpenGL::drawPoints},
        {"GL_LINES", &MyOpenGL::drawLines},
        {"GL_LINE_STRIP", &MyOpenGL::drawLineStrip},
        {"GL_LINE_LOOP", &MyOpenGL::drawLineLoop},
        {"GL_TRIANGLES", &MyOpenGL::drawTriangles},
        {"GL_TRIANGLE_STRIP", &MyOpenGL::drawTriangleStrip},
        {"GL_TRIANGLE_FAN", &MyOpenGL::drawTriangleFan},
        {"GL_QUADS", &MyOpenGL::drawQuads},
        {"GL_QUAD_STRIP", &MyOpenGL::drawQuadStrip},
        {"GL_POLYGON", &MyOpenGL::drawPolygon}
    };
    points = {};
    generatePoints();
    x = 0;
    y = 0;
    height = 1000;
    width = 1000;
    alphaFunc = GL_ALWAYS;
    ref = 0;
    sfactor = GL_ONE;
    dfactor = GL_ZERO;
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
    glEnable(GL_SCISSOR_TEST);
    glEnable(GL_ALPHA_TEST);
    glEnable(GL_BLEND);
    glScissor(x, y, width, height);
    glAlphaFunc(alphaFunc, ref);
    glBlendFunc(sfactor, dfactor);
    (this->*drawPrimitive[curPrimitive])();
    glDisable(GL_SCISSOR_TEST);
    glDisable(GL_ALPHA_TEST);
    glDisable(GL_BLEND);
}

void MyOpenGL::generatePoints() {
    for (int i = 0; i < 100; i++) {
        Point p = getRandomPoint();
        points.push_back(p);
    }
}

Color MyOpenGL::getRandomColor() {
    double color[4];
    for (int i = 0; i < 4; i++) {
        color[i] = (rand() % 101) / 100.0;
    }
    return { color[0], color[1], color[2], color[3] };
}

Point MyOpenGL::getRandomPoint() {
    double point[2];
    for (int i = 0; i < 2; i++) {
        point[i] = (rand() % 101) / 100.0;
    }
    Color c = getRandomColor();
    return { point[0], point[1], c };
}

void MyOpenGL::placePoint(int i) {
    glColor4d(points[i].color.r, points[i].color.g, points[i].color.b, points[i].color.a);
    glVertex2d(points[i].x, points[i].y);
}

void MyOpenGL::drawPoints() {
    glBegin(GL_POINTS);
    for (int i = 0; i < 25; i++) {
        placePoint(i);
    }
    glEnd();
}

void MyOpenGL::drawLines() {
    glBegin(GL_LINES);
    for (int i = 25; i < 37; i++) {
        placePoint(i);
    }
    glEnd();
}

void MyOpenGL::drawLineStrip() {
    glBegin(GL_LINE_STRIP);
    for (int i = 37; i < 49; i++) {
        placePoint(i);
    }
    glEnd();
}

void MyOpenGL::drawLineLoop() {
    glBegin(GL_LINE_LOOP);
    for (int i = 49; i < 61; i++) {
        placePoint(i);
    }
    glEnd();
}

void MyOpenGL::drawTriangles() {
    glBegin(GL_TRIANGLES);
    for (int i = 61; i < 67; i++) {
        placePoint(i);
    }
    glEnd();
}

void MyOpenGL::drawTriangleStrip() {
    glBegin(GL_TRIANGLE_STRIP);
    for (int i = 67; i < 72; i++) {
        placePoint(i);
    }
    glEnd();
}

void MyOpenGL::drawTriangleFan() {
    glBegin(GL_TRIANGLE_FAN);
    for (int i = 72; i < 77; i++) {
        placePoint(i);
    }
    glEnd();
}

void MyOpenGL::drawQuads() {
    glBegin(GL_QUADS);
    for (int i = 77; i < 85; i++) {
        placePoint(i);
    }
    glEnd();
}

void MyOpenGL::drawQuadStrip() {
    glBegin(GL_QUAD_STRIP);
    for (int i = 85; i < 93; i++) {
        placePoint(i);
    }
    glEnd();
}

void MyOpenGL::drawPolygon() {
    glBegin(GL_POLYGON);
    for (int i = 93; i < 100; i++) {
        placePoint(i);
    }
    glEnd();
}

void MyOpenGL::changePrimitive(std::string s) {
    curPrimitive = s;
    update();
}

void MyOpenGL::changeScissorsTest(int x, int y, int w, int h) {
    this->x = x;
    this->y = y;
    this->width = w;
    this->height = h;
    update();
}

void MyOpenGL::changeAlphaTest(std::string func, int ref) {
    std::map<std::string, int> funcs = {
        {"GL_ALWAYS", GL_ALWAYS},
        {"GL_NEVER", GL_NEVER},
        {"GL_EQUAL", GL_EQUAL},
        {"GL_LEQUAL", GL_LEQUAL},
        {"GL_GREATER", GL_GREATER},
        {"GL_NOTEQUAL", GL_NOTEQUAL},
        {"GL_GEQUAL", GL_GEQUAL},
        {"GL_LESS", GL_LESS}
    };
    this->ref = ref / 100.0;
    this->alphaFunc = funcs[func];
    update();
}

void MyOpenGL::changeBlendTest(std::string sfactor, std::string dfactor) {
    std::map<std::string, int> blendTest = {
        {"GL_ONE", GL_ONE},
        {"GL_ZERO", GL_ZERO},
        {"GL_DST_COLOR", GL_DST_COLOR},
        {"GL_ONE_MINUS_DST_COLOR", GL_ONE_MINUS_DST_COLOR},
        {"GL_SRC_ALPHA", GL_SRC_ALPHA},
        {"GL_ONE_MINUS_SRC_ALPHA", GL_ONE_MINUS_SRC_ALPHA},
        {"GL_DST_ALPHA", GL_DST_ALPHA},
        {"GL_ONE_MINUS_DST_ALPHA", GL_ONE_MINUS_DST_ALPHA},
        {"GL_SRC_ALPHA_SATURATE", GL_SRC_ALPHA_SATURATE},
        {"GL_SRC_COLOR", GL_SRC_COLOR},
        {"GL_ONE_MINUS_SRC_COLOR", GL_ONE_MINUS_SRC_COLOR}
    };
    this->sfactor = blendTest[sfactor];
    this->dfactor = blendTest[dfactor];
    update();
}
