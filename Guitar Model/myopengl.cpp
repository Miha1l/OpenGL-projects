#include "myopengl.h"

MyOpenGL::MyOpenGL(QWidget *parent)
    : QOpenGLWidget(parent)
{
    setFocusPolicy(Qt::StrongFocus);

    options = {
        {0.0f, 0.0f, 0.0f},
        0,
        false,
        false
    };

    light = {
        0,
        {0.3f, 0.3f, 0.3f},
        {0.1f, 0.1f, 0.1f},
        {1.0f, 1.0f, 1.0f},
        {1.0f, 1.0f, 1.0f}
    };

    fbHeight = 1024;
    fbWidth = 1024;

    lightRotateX = 30;
    lightRotateY = 40;
    shadowLightMatrix.setToIdentity();
    shadowLightMatrix.rotate(lightRotateX, 1.0f, 0.0f, 0.0f);
    shadowLightMatrix.rotate(lightRotateY, 0.0f, 1.0f, 0.0f);

    lightMatrix.setToIdentity();
    lightMatrix.rotate(-lightRotateY, 0.0f, 1.0f, 0.0f);
    lightMatrix.rotate(-lightRotateX, 1.0f, 0.0f, 0.0f);

    shadowPointCloudFilteringQuality = 1.0f;
}

MyOpenGL::~MyOpenGL()
{
    makeCurrent();
    for (auto obj: guitarParts) {
        delete obj;
    }
    delete depthBuffer;
}

void MyOpenGL::initializeGL()
{
    initializeOpenGLFunctions();
    initShaders();
    initScene();
    glClearColor(0.5, 0.5, 0.5, 1);
    depthBuffer = new QOpenGLFramebufferObject(fbWidth, fbHeight, QOpenGLFramebufferObject::Depth);
}

void MyOpenGL::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    camera.setViewport(w, h);
}

void MyOpenGL::paintGL()
{
    // Отрисовка во фрейм буффер
    depthBuffer->bind();

    glViewport(0, 0, fbWidth, fbHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    QMatrix4x4 projectionLightMatrix = camera.getProjection(1);

    depthShaderProgram.bind();
    depthShaderProgram.setUniformValue("projectionLightMatrix", projectionLightMatrix);
    depthShaderProgram.setUniformValue("shadowLightMatrix", shadowLightMatrix);

    wall.draw(&depthShaderProgram, GL_QUADS);
    lamp.draw(&depthShaderProgram, GL_QUADS);
    guitarParts[0]->draw(&depthShaderProgram, GL_POLYGON);
    guitarParts[1]->draw(&depthShaderProgram, GL_POLYGON);
    guitarParts[2]->draw(&depthShaderProgram, GL_POLYGON);
    guitarParts[3]->draw(&depthShaderProgram, GL_POLYGON);
    guitarParts[4]->draw(&depthShaderProgram, GL_QUADS);
    guitarParts[5]->draw(&depthShaderProgram, GL_QUADS);
    guitarParts[6]->draw(&depthShaderProgram, GL_QUADS);
    guitarParts[7]->draw(&depthShaderProgram, GL_POLYGON);
    guitarParts[8]->draw(&depthShaderProgram, GL_QUADS);
    guitarParts[9]->draw(&depthShaderProgram, GL_LINES);

    depthShaderProgram.release();
    depthBuffer->release();

    GLuint texture = depthBuffer->texture();
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Отрисовка на экран
    glViewport(0, 0, width(), height());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (options.drawInvisible) {
        glDisable(GL_DEPTH_TEST);
    }
    else {
        glEnable(GL_DEPTH_TEST);
    }
    drawScene();
}

void MyOpenGL::keyPressEvent(QKeyEvent* event)
{
    switch (event->key()) {
    case Qt::Key_W:
        camera.move(Camera::MoveDirection::Forward, true);
        break;
    case Qt::Key_S:
        camera.move(Camera::MoveDirection::Back, true);
        break;
    case Qt::Key_A:
        camera.move(Camera::MoveDirection::Left, true);
        break;
    case Qt::Key_D:
        camera.move(Camera::MoveDirection::Right, true);
        break;
    case Qt::Key_Space:
        camera.move(Camera::MoveDirection::Up, true);
        break;
    case Qt::Key_C:
        camera.move(Camera::MoveDirection::Down, true);
        break;
    case Qt::Key_Q:
        camera.rotate(-0.5f, 0.0f);
        break;
    case Qt::Key_E:
        camera.rotate(0.5f, 0.0f);
        break;
    case Qt::Key_R:
        camera.rotate(0.0f, 0.5f);
        break;
    case Qt::Key_F:
        camera.rotate(0.0f, -0.5f);
        break;
    }
    update();
}

void MyOpenGL::keyReleaseEvent(QKeyEvent* event)
{
    switch (event->key()) {
    case Qt::Key_W:
        camera.move(Camera::MoveDirection::Forward, false);
        break;
    case Qt::Key_S:
        camera.move(Camera::MoveDirection::Back, false);
        break;
    case Qt::Key_A:
        camera.move(Camera::MoveDirection::Left, false);
        break;
    case Qt::Key_D:
        camera.move(Camera::MoveDirection::Right, false);
        break;
    case Qt::Key_Space:
        camera.move(Camera::MoveDirection::Up, false);
        break;
    case Qt::Key_C:
        camera.move(Camera::MoveDirection::Down, false);
        break;
    }

    update();
}

void MyOpenGL::mousePressEvent(QMouseEvent* event)
{
    if (event->buttons() == Qt::LeftButton) {
        mousePosition = QVector2D(event->position());
    }
    event->accept();
}

void MyOpenGL::mouseMoveEvent(QMouseEvent* event)
{
    if (event->buttons() != Qt::LeftButton) {
        return;
    }

    QVector2D diff = QVector2D(event->position()) - mousePosition;
    mousePosition = QVector2D(event->position());

    float angle = diff.length() / 2.0f;

    QVector3D axis = QVector3D(diff.y(), diff.x(), 0.0);
    QQuaternion rotation = QQuaternion::fromAxisAndAngle(axis, angle);
    for (auto& obj: guitarParts) {
        obj->rotate(rotation);
    }

    update();
}

void MyOpenGL::wheelEvent(QWheelEvent* event)
{
    camera.zoom(event->angleDelta().y() / 100.0f);
    update();
}

void MyOpenGL::initShaders()
{
    if (!objectShaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/Object/vobject.vsh")
        || !objectShaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/Object/fobject.fsh")
        || !objectShaderProgram.link()) {
        close();
    }
    if (!lampShaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/Lamp/vlamp.vsh")
        || !lampShaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/Lamp/flamp.fsh")
        || !lampShaderProgram.link()) {
        close();
    }
    if (!depthShaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/Depth/vdepth.vsh")
        || !depthShaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/Depth/fdepth.fsh")
        || !depthShaderProgram.link()) {
        close();
    }
}

void MyOpenGL::initScene()
{
    initObject();
    initCoord();
    initLamp();
    initWall();
}

void MyOpenGL::initObject()
{
    QVector<VertexData> vertices1;
    QVector<VertexData> vertices2;
    QVector<VertexData> vertices3;
    QVector<VertexData> vertices4;
    QVector<VertexData> vertices5;
    QVector<VertexData> vertices6;
    QVector<VertexData> vertices7;
    QVector<VertexData> vertices8;
    QVector<VertexData> vertices9;
    QVector<VertexData> vertices10;

    Object* frontPanelDown = new Object();
    Object* frontPanelUp = new Object();
    Object* backPanelDown = new Object();
    Object* backPanelUp = new Object();
    Object* contourDown = new Object();
    Object* contourUp = new Object();
    Object* neck = new Object();
    Object* resonator = new Object();
    Object* stand = new Object();
    Object* strings = new Object();

    int fragments = 20;

    // Нижняя часть панелей
    for (int i = 0; i < fragments; i++) {
        float angle = 2.0f * M_PI * float(i) / (float)fragments;
        float x = 0.2f * qCos(angle);
        float y = 0.2f * qSin(angle);

        vertices1.append(VertexData(QVector3D(x, y - 0.2, 0.05), QVector3D(0.69, 0.49, 0.31), QVector3D(0.0, 0.0, 1.0)));
        vertices2.append(VertexData(QVector3D(x, y - 0.2, -0.05), QVector3D(0.69, 0.49, 0.31), QVector3D(0.0, 0.0, -1.0)));
    }

    // Верхняя часть панелей
    for (int i = 0; i < fragments; i++) {
        float angle = 2.0f * M_PI * float(i) / (float)fragments;
        float x = 0.15f * qCos(angle);
        float y = 0.15f * qSin(angle);

        vertices3.append(VertexData(QVector3D(x, y, 0.05), QVector3D(0.69, 0.49, 0.31), QVector3D(0.0, 0.0, 1.0)));
        vertices4.append(VertexData(QVector3D(x, y, -0.05), QVector3D(0.69, 0.49, 0.31), QVector3D(0.0, 0.0, -1.0)));
    }

    // Контур вокруг панелей
    for (int i = 0; i < fragments; i++) {
        float angle = 2.0f * M_PI * float(i) / (float)fragments;
        float nextAngle = 2.0f * M_PI * float(i + 1) / (float)fragments;

        float x = 0.2f * qCos(angle);
        float y = 0.2f * qSin(angle);

        vertices5.append(VertexData(QVector3D(x, y - 0.2, -0.05), QVector3D(0.69, 0.49, 0.31), QVector3D(0.0, 0.0, 0.0)));
        vertices5.append(VertexData(QVector3D(x, y - 0.2, 0.05), QVector3D(0.69, 0.49, 0.31), QVector3D(0.0, 0.0, 0.0)));

        x = 0.2f * qCos(nextAngle);
        y = 0.2f * qSin(nextAngle);

        vertices5.append(VertexData(QVector3D(x, y - 0.2, 0.05), QVector3D(0.69, 0.49, 0.31), QVector3D(0.0, 0.0, 0.0)));
        vertices5.append(VertexData(QVector3D(x, y - 0.2, -0.05), QVector3D(0.69, 0.49, 0.31), QVector3D(0.0, 0.0, 0.0)));

        x = 0.15f * qCos(angle);
        y = 0.15f * qSin(angle);

        vertices6.append(VertexData(QVector3D(x, y, -0.05), QVector3D(0.69, 0.49, 0.31), QVector3D(0.0, 0.0, 0.0)));
        vertices6.append(VertexData(QVector3D(x, y, 0.05), QVector3D(0.69, 0.49, 0.31), QVector3D(0.0, 0.0, 0.0)));

        x = 0.15f * qCos(nextAngle);
        y = 0.15f * qSin(nextAngle);

        vertices6.append(VertexData(QVector3D(x, y, 0.05), QVector3D(0.69, 0.49, 0.31), QVector3D(0.0, 0.0, 0.0)));
        vertices6.append(VertexData(QVector3D(x, y, -0.05), QVector3D(0.69, 0.49, 0.31), QVector3D(0.0, 0.0, 0.0)));

        int lastElem = vertices5.size() - 1;
        QVector3D center1 = (vertices5[lastElem].position + vertices5[lastElem - 1].position
                + vertices5[lastElem - 2].position + vertices5[lastElem - 3].position) / 4.0f;
        QVector3D normal1 = center1 - QVector3D(0.0, -0.2, 0.0);
        vertices5[lastElem].normal = normal1;
        vertices5[lastElem - 1].normal = normal1;
        vertices5[lastElem - 2].normal = normal1;
        vertices5[lastElem - 3].normal = normal1;

        QVector3D center2 = (vertices6[lastElem].position + vertices6[lastElem - 1].position
                + vertices6[lastElem - 2].position + vertices6[lastElem - 3].position) / 4.0f;
        QVector3D normal2 = center2 - QVector3D(0.0, 0.0, 0.0);
        vertices6[lastElem].normal = normal2;
        vertices6[lastElem - 1].normal = normal2;
        vertices6[lastElem - 2].normal = normal2;
        vertices6[lastElem - 3].normal = normal2;

    }

    // Гриф
    vertices7.append(VertexData(QVector3D(-0.035, 0.05, 0.056), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 0.0, 1.0)));
    vertices7.append(VertexData(QVector3D(0.035, 0.05, 0.056), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 0.0, 1.0)));
    vertices7.append(VertexData(QVector3D(0.035, 0.55, 0.056), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 0.0, 1.0)));
    vertices7.append(VertexData(QVector3D(-0.035, 0.55, 0.056), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 0.0, 1.0)));

    vertices7.append(VertexData(QVector3D(-0.035, 0.14, 0.0), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 0.0, -1.0)));
    vertices7.append(VertexData(QVector3D(0.035, 0.14, 0.0), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 0.0, -1.0)));
    vertices7.append(VertexData(QVector3D(0.035, 0.55, 0.0), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 0.0, -1.0)));
    vertices7.append(VertexData(QVector3D(-0.035, 0.55, 0.0), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 0.0, -1.0)));

    vertices7.append(VertexData(QVector3D(0.035, 0.05, 0.056), QVector3D(0.0, 0.0, 0.0), QVector3D(1.0, 0.0, 0.0)));
    vertices7.append(VertexData(QVector3D(0.035, 0.05, 0.0), QVector3D(0.0, 0.0, 0.0), QVector3D(1.0, 0.0, 0.0)));
    vertices7.append(VertexData(QVector3D(0.035, 0.55, 0.0), QVector3D(0.0, 0.0, 0.0), QVector3D(1.0, 0.0, 0.0)));
    vertices7.append(VertexData(QVector3D(0.035, 0.55, 0.056), QVector3D(0.0, 0.0, 0.0), QVector3D(1.0, 0.0, 0.0)));

    vertices7.append(VertexData(QVector3D(-0.035, 0.05, 0.056), QVector3D(0.0, 0.0, 0.0), QVector3D(-1.0, 0.0, 0.0)));
    vertices7.append(VertexData(QVector3D(-0.035, 0.05, 0.0), QVector3D(0.0, 0.0, 0.0), QVector3D(-1.0, 0.0, 0.0)));
    vertices7.append(VertexData(QVector3D(-0.035, 0.55, 0.0), QVector3D(0.0, 0.0, 0.0), QVector3D(-1.0, 0.0, 0.0)));
    vertices7.append(VertexData(QVector3D(-0.035, 0.55, 0.056), QVector3D(0.0, 0.0, 0.0), QVector3D(-1.0, 0.0, 0.0)));

    vertices7.append(VertexData(QVector3D(-0.035, 0.55, 0.056), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 1.0, 0.0)));
    vertices7.append(VertexData(QVector3D(0.035, 0.55, 0.056), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 1.0, 0.0)));
    vertices7.append(VertexData(QVector3D(0.035, 0.55, 0.0), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 1.0, 0.0)));
    vertices7.append(VertexData(QVector3D(-0.035, 0.55, 0.0), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 1.0, 0.0)));

    // Голова
    vertices7.append(VertexData(QVector3D(0.056, 0.45, 0.056), QVector3D(0.0, 0.0, 0.0), QVector3D(0.42, 0.9, 0.0)));
    vertices7.append(VertexData(QVector3D(0.035, 0.55, 0.056), QVector3D(0.0, 0.0, 0.0), QVector3D(0.42, 0.9, 0.0)));
    vertices7.append(VertexData(QVector3D(0.035, 0.55, 0.0), QVector3D(0.0, 0.0, 0.0), QVector3D(0.42, 0.9, 0.0)));
    vertices7.append(VertexData(QVector3D(0.056, 0.45, 0.0), QVector3D(0.0, 0.0, 0.0), QVector3D(0.42, 0.9, 0.0)));

    vertices7.append(VertexData(QVector3D(0.035, 0.4, 0.056), QVector3D(0.0, 0.0, 0.0), QVector3D(0.92, -0.39, 0.0)));
    vertices7.append(VertexData(QVector3D(0.056, 0.45, 0.056), QVector3D(0.0, 0.0, 0.0), QVector3D(0.92, -0.39, 0.0)));
    vertices7.append(VertexData(QVector3D(0.056, 0.45, 0.0), QVector3D(0.0, 0.0, 0.0), QVector3D(0.92, -0.39, 0.0)));
    vertices7.append(VertexData(QVector3D(0.035, 0.4, 0.0), QVector3D(0.0, 0.0, 0.0), QVector3D(0.92, -0.39, 0.0)));

    vertices7.append(VertexData(QVector3D(-0.035, 0.55, 0.056), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 0.0, 1.0)));
    vertices7.append(VertexData(QVector3D(0.035, 0.4, 0.056), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 0.0, 1.0)));
    vertices7.append(VertexData(QVector3D(0.056, 0.45, 0.056), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 0.0, 1.0)));
    vertices7.append(VertexData(QVector3D(0.035, 0.55, 0.056), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 0.0, 1.0)));

    vertices7.append(VertexData(QVector3D(-0.035, 0.55, 0.0), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 0.0, -1.0)));
    vertices7.append(VertexData(QVector3D(0.035, 0.4, 0.0), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 0.0, -1.0)));
    vertices7.append(VertexData(QVector3D(0.056, 0.45, 0.0), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 0.0, -1.0)));
    vertices7.append(VertexData(QVector3D(0.035, 0.55, 0.0), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 0.0, -1.0)));

    vertices7.append(VertexData(QVector3D(-0.056, 0.45, 0.056), QVector3D(0.0, 0.0, 0.0), QVector3D(-0.42, 0.9, 0.0)));
    vertices7.append(VertexData(QVector3D(-0.035, 0.55, 0.056), QVector3D(0.0, 0.0, 0.0), QVector3D(-0.42, 0.9, 0.0)));
    vertices7.append(VertexData(QVector3D(-0.035, 0.55, 0.0), QVector3D(0.0, 0.0, 0.0), QVector3D(-0.42, 0.9, 0.0)));
    vertices7.append(VertexData(QVector3D(-0.056, 0.45, 0.0), QVector3D(0.0, 0.0, 0.0), QVector3D(-0.42, 0.9, 0.0)));

    vertices7.append(VertexData(QVector3D(-0.035, 0.4, 0.056), QVector3D(0.0, 0.0, 0.0), QVector3D(-0.92, -0.39, 0.0)));
    vertices7.append(VertexData(QVector3D(-0.056, 0.45, 0.056), QVector3D(0.0, 0.0, 0.0), QVector3D(-0.92, -0.39, 0.0)));
    vertices7.append(VertexData(QVector3D(-0.056, 0.45, 0.0), QVector3D(0.0, 0.0, 0.0), QVector3D(-0.92, -0.39, 0.0)));
    vertices7.append(VertexData(QVector3D(-0.035, 0.4, 0.0), QVector3D(0.0, 0.0, 0.0), QVector3D(-0.92, -0.39, 0.0)));

    vertices7.append(VertexData(QVector3D(0.035, 0.55, 0.056), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 0.0, 1.0)));
    vertices7.append(VertexData(QVector3D(-0.035, 0.4, 0.056), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 0.0, 1.0)));
    vertices7.append(VertexData(QVector3D(-0.056, 0.45, 0.056), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 0.0, 1.0)));
    vertices7.append(VertexData(QVector3D(-0.035, 0.55, 0.056), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 0.0, 1.0)));

    vertices7.append(VertexData(QVector3D(0.035, 0.55, 0.0), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 0.0, -1.0)));
    vertices7.append(VertexData(QVector3D(-0.035, 0.4, 0.0), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 0.0, -1.0)));
    vertices7.append(VertexData(QVector3D(-0.056, 0.45, 0.0), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 0.0, -1.0)));
    vertices7.append(VertexData(QVector3D(-0.035, 0.55, 0.0), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 0.0, -1.0)));

    // Резонаторный круг
    for (int i = 0; i < 40; i++) {
        float angle = 2.0f * M_PI * float(i) / 40.0f;
        float x = 0.08f * qCos(angle);
        float y = 0.08f * qSin(angle);

        vertices8.append(VertexData(QVector3D(x, y, 0.051), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 0.0, 1.0)));
    }

    // Подставка для струн
    vertices9.append(VertexData(QVector3D(-0.05, -0.2, 0.056), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 0.0, 1.0)));
    vertices9.append(VertexData(QVector3D(0.05, -0.2, 0.056), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 0.0, 1.0)));
    vertices9.append(VertexData(QVector3D(0.05, -0.18, 0.056), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 0.0, 1.0)));
    vertices9.append(VertexData(QVector3D(-0.05, -0.18, 0.056), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 0.0, 1.0)));

    vertices9.append(VertexData(QVector3D(-0.05, -0.18, 0.056), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 1.0, 0.0)));
    vertices9.append(VertexData(QVector3D(0.05, -0.18, 0.056), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 1.0, 0.0)));
    vertices9.append(VertexData(QVector3D(0.05, -0.18, 0.0), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 1.0, 0.0)));
    vertices9.append(VertexData(QVector3D(-0.05, -0.18, 0.0), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 1.0, 0.0)));

    vertices9.append(VertexData(QVector3D(-0.05, -0.2, 0.056), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, -1.0, 0.0)));
    vertices9.append(VertexData(QVector3D(0.05, -0.2, 0.056), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, -1.0, 0.0)));
    vertices9.append(VertexData(QVector3D(0.05, -0.2, 0.0), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, -1.0, 0.0)));
    vertices9.append(VertexData(QVector3D(-0.05, -0.2, 0.0), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, -1.0, 0.0)));

    vertices9.append(VertexData(QVector3D(0.05, -0.18, 0.056), QVector3D(0.0, 0.0, 0.0), QVector3D(1.0, 0.0, 0.0)));
    vertices9.append(VertexData(QVector3D(0.05, -0.18, 0.00), QVector3D(0.0, 0.0, 0.0), QVector3D(1.0, 0.0, 0.0)));
    vertices9.append(VertexData(QVector3D(0.05, -0.2, 0.0), QVector3D(0.0, 0.0, 0.0), QVector3D(1.0, 0.0, 0.0)));
    vertices9.append(VertexData(QVector3D(0.05, -0.2, 0.056), QVector3D(0.0, 0.0, 0.0), QVector3D(1.0, 0.0, 0.0)));

    vertices9.append(VertexData(QVector3D(-0.05, -0.18, 0.056), QVector3D(0.0, 0.0, 0.0), QVector3D(-1.0, 0.0, 0.0)));
    vertices9.append(VertexData(QVector3D(-0.05, -0.18, 0.00), QVector3D(0.0, 0.0, 0.0), QVector3D(-1.0, 0.0, 0.0)));
    vertices9.append(VertexData(QVector3D(-0.05, -0.2, 0.0), QVector3D(0.0, 0.0, 0.0), QVector3D(-1.0, 0.0, 0.0)));
    vertices9.append(VertexData(QVector3D(-0.05, -0.2, 0.056), QVector3D(0.0, 0.0, 0.0), QVector3D(-1.0, 0.0, 0.0)));

    // Струны
    for (float x = -0.032; x < 0.032; x += 0.008) {
        vertices10.append(VertexData(QVector3D(x, -0.2, 0.0561), QVector3D(0.8, 0.8, 0.8), QVector3D(0.0, 0.0, 1.0)));
        vertices10.append(VertexData(QVector3D(x, 0.55, 0.0561), QVector3D(0.8, 0.8, 0.8), QVector3D(0.0, 0.0, 1.0)));
    }

    frontPanelDown->init(vertices1, context()->functions());
    backPanelDown->init(vertices2, context()->functions());
    frontPanelUp->init(vertices3, context()->functions());
    backPanelUp->init(vertices4, context()->functions());
    contourDown->init(vertices5, context()->functions());
    contourUp->init(vertices6, context()->functions());
    neck->init(vertices7, context()->functions());
    resonator->init(vertices8, context()->functions());
    stand->init(vertices9, context()->functions());
    strings->init(vertices10, context()->functions());

    guitarParts.append(frontPanelDown);
    guitarParts.append(backPanelDown);
    guitarParts.append(frontPanelUp);
    guitarParts.append(backPanelUp);
    guitarParts.append(contourDown);
    guitarParts.append(contourUp);
    guitarParts.append(neck);
    guitarParts.append(resonator);
    guitarParts.append(stand);
    guitarParts.append(strings);

    Object::Material material = {
        {0.5f, 0.5f, 0.5f},
        {0.5f, 0.5f, 0.5f},
        {0.5f, 0.5f, 0.5f},
        32.0f
    };

    for (auto& obj: guitarParts) {
        obj->changeMaterialProperties(material);
    }
}

void MyOpenGL::initCoord()
{
    QVector<VertexData> vertices;

    vertices.append(VertexData(QVector3D(-1.0, 0.0, 0.0), QVector3D(1.0, 1.0, 1.0), QVector3D(0.0, 0.0, 0.0)));
    vertices.append(VertexData(QVector3D(1.0, 0.0, 0.0), QVector3D(1.0, 1.0, 1.0), QVector3D(0.0, 0.0, 0.0)));
    vertices.append(VertexData(QVector3D(0.0, -1.0, 0.0), QVector3D(1.0, 1.0, 1.0), QVector3D(0.0, 0.0, 0.0)));
    vertices.append(VertexData(QVector3D(0.0, 1.0, 0.0), QVector3D(1.0, 1.0, 1.0), QVector3D(0.0, 0.0, 0.0)));
    vertices.append(VertexData(QVector3D(0.0, 0.0, -1.0), QVector3D(1.0, 1.0, 1.0), QVector3D(0.0, 0.0, 0.0)));
    vertices.append(VertexData(QVector3D(0.0, 0.0, 1.0), QVector3D(1.0, 1.0, 1.0), QVector3D(0.0, 0.0, 0.0)));

    coordinates.init(vertices, context()->functions());
}

void MyOpenGL::initWall()
{
    QVector<VertexData> vertices;

    for (float y = -2.0f; y < 2.0f; y += 0.1) {
        vertices.append(VertexData(QVector3D(-2.0, y, -0.15), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 0.0, 1.0)));
        vertices.append(VertexData(QVector3D(-2.0, y + 0.02, -0.15), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 0.0, 1.0)));
        vertices.append(VertexData(QVector3D(2.0, y + 0.02, -0.15), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 0.0, 1.0)));
        vertices.append(VertexData(QVector3D(2.0, y, -0.15), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 0.0, 1.0)));

        vertices.append(VertexData(QVector3D(-2.0, y + 0.02, -0.15), QVector3D(0.8, 0.8, 0.8), QVector3D(0.0, 0.0, 1.0)));
        vertices.append(VertexData(QVector3D(-2.0, y + 0.1, -0.15), QVector3D(0.8, 0.8, 0.8), QVector3D(0.0, 0.0, 1.0)));
        vertices.append(VertexData(QVector3D(2.0, y + 0.1, -0.15), QVector3D(0.8, 0.8, 0.8), QVector3D(0.0, 0.0, 1.0)));
        vertices.append(VertexData(QVector3D(2.0, y + 0.02, -0.15), QVector3D(0.8, 0.8, 0.8), QVector3D(0.0, 0.0, 1.0)));
    }

    wall.init(vertices, context()->functions());
    Object::Material material = {
        {0.5f, 0.5f, 0.5f},
        {0.5f, 0.5f, 0.5f},
        {0.5f, 0.5f, 0.5f},
        32.0f
    };
    wall.changeMaterialProperties(material);
}

void MyOpenGL::initLamp()
{
    QVector<VertexData> vertices;

    vertices.append(VertexData(QVector3D(-0.25, -0.25, 0.0), QVector3D(1.0, 1.0, 1.0), QVector3D(0.0, 0.0, 0.0)));
    vertices.append(VertexData(QVector3D(0.25, -0.25, 0.0), QVector3D(1.0, 1.0, 1.0), QVector3D(0.0, 0.0, 0.0)));
    vertices.append(VertexData(QVector3D(0.25, 0.25, 0.0), QVector3D(1.0, 1.0, 1.0), QVector3D(0.0, 0.0, 0.0)));
    vertices.append(VertexData(QVector3D(-0.25, 0.25, 0.0), QVector3D(1.0, 1.0, 1.0), QVector3D(0.0, 0.0, 0.0)));

    vertices.append(VertexData(QVector3D(-0.25, -0.25, 0.0), QVector3D(1.0, 1.0, 1.0), QVector3D(0.0, 0.0, 0.0)));
    vertices.append(VertexData(QVector3D(-0.25, -0.25, -0.25), QVector3D(1.0, 1.0, 1.0), QVector3D(0.0, 0.0, 0.0)));
    vertices.append(VertexData(QVector3D(-0.25, 0.25, -0.25), QVector3D(1.0, 1.0, 1.0), QVector3D(0.0, 0.0, 0.0)));
    vertices.append(VertexData(QVector3D(-0.25, 0.25, 0.0), QVector3D(1.0, 1.0, 1.0), QVector3D(0.0, 0.0, 0.0)));

    vertices.append(VertexData(QVector3D(0.25, -0.25, 0.0), QVector3D(1.0, 1.0, 1.0), QVector3D(0.0, 0.0, 0.0)));
    vertices.append(VertexData(QVector3D(0.25, -0.25, -0.25), QVector3D(1.0, 1.0, 1.0), QVector3D(0.0, 0.0, 0.0)));
    vertices.append(VertexData(QVector3D(0.25, 0.25, -0.25), QVector3D(1.0, 1.0, 1.0), QVector3D(0.0, 0.0, 0.0)));
    vertices.append(VertexData(QVector3D(0.25, 0.25, 0.0), QVector3D(1.0, 1.0, 1.0), QVector3D(0.0, 0.0, 0.0)));

    vertices.append(VertexData(QVector3D(-0.25, 0.25, 0.0), QVector3D(1.0, 1.0, 1.0), QVector3D(0.0, 0.0, 0.0)));
    vertices.append(VertexData(QVector3D(-0.25, 0.25, -0.25), QVector3D(1.0, 1.0, 1.0), QVector3D(0.0, 0.0, 0.0)));
    vertices.append(VertexData(QVector3D(0.25, 0.25, -0.25), QVector3D(1.0, 1.0, 1.0), QVector3D(0.0, 0.0, 0.0)));
    vertices.append(VertexData(QVector3D(0.25, 0.25, 0.0), QVector3D(1.0, 1.0, 1.0), QVector3D(0.0, 0.0, 0.0)));

    vertices.append(VertexData(QVector3D(-0.25, -0.25, 0.0), QVector3D(1.0, 1.0, 1.0), QVector3D(0.0, 0.0, 0.0)));
    vertices.append(VertexData(QVector3D(-0.25, -0.25, -0.25), QVector3D(1.0, 1.0, 1.0), QVector3D(0.0, 0.0, 0.0)));
    vertices.append(VertexData(QVector3D(0.25, -0.25, -0.25), QVector3D(1.0, 1.0, 1.0), QVector3D(0.0, 0.0, 0.0)));
    vertices.append(VertexData(QVector3D(0.25, -0.25, 0.0), QVector3D(1.0, 1.0, 1.0), QVector3D(0.0, 0.0, 0.0)));

    vertices.append(VertexData(QVector3D(-0.25, -0.25, -0.25), QVector3D(1.0, 1.0, 1.0), QVector3D(0.0, 0.0, 0.0)));
    vertices.append(VertexData(QVector3D(0.25, -0.25, -0.25), QVector3D(1.0, 1.0, 1.0), QVector3D(0.0, 0.0, 0.0)));
    vertices.append(VertexData(QVector3D(0.25, 0.25, -0.25), QVector3D(1.0, 1.0, 1.0), QVector3D(0.0, 0.0, 0.0)));
    vertices.append(VertexData(QVector3D(-0.25, 0.25, -0.25), QVector3D(1.0, 1.0, 1.0), QVector3D(0.0, 0.0, 0.0)));

    lamp.init(vertices, context()->functions());
    lamp.setPosition(light.position);
}

void MyOpenGL::drawScene()
{
    camera.update();
    QMatrix4x4 viewMatrix = camera.getView();
    QMatrix4x4 projectionMatrix = camera.getProjection(options.projection);
    QVector3D viewPosition = camera.getPosition();
    QMatrix4x4 projectionLightMatrix = camera.getProjection(1);
    objectShaderProgram.bind();
    objectShaderProgram.setUniformValue("projectionMatrix", projectionMatrix);
    objectShaderProgram.setUniformValue("projectionLightMatrix", projectionLightMatrix);
    objectShaderProgram.setUniformValue("shadowLightMatrix", shadowLightMatrix);
    objectShaderProgram.setUniformValue("lightMatrix", lightMatrix);
    objectShaderProgram.setUniformValue("viewMatrix", viewMatrix);
    objectShaderProgram.setUniformValue("lightDirection", 0.0f, 0.0f, -1.0f, 0.0f); // Для создания тени
    objectShaderProgram.setUniformValue("ShadowMap", GL_TEXTURE4 - GL_TEXTURE0);
    objectShaderProgram.setUniformValue("ShadowMapSize", (float)fbWidth);
    objectShaderProgram.setUniformValue("ShadowPointCloudFilteringQuality", shadowPointCloudFilteringQuality);
    objectShaderProgram.setUniformValue("light.type", light.type);
    objectShaderProgram.setUniformValue("light.position", light.position);
    objectShaderProgram.setUniformValue("light.ambient", light.ambient);
    objectShaderProgram.setUniformValue("light.diffuse", light.diffuse);
    objectShaderProgram.setUniformValue("light.specular", 1.0f, 1.0f, 1.0f);
    objectShaderProgram.setUniformValue("light.direction", 0.0f, 0.0f, -1.0f);
    objectShaderProgram.setUniformValue("light.cutOff", qCos(qDegreesToRadians(18.0f)));
    objectShaderProgram.setUniformValue("light.constant", 1.0f);
    objectShaderProgram.setUniformValue("light.linear", 0.09f);
    objectShaderProgram.setUniformValue("light.quadratic", 0.032f);
    objectShaderProgram.setUniformValue("ViewPos", viewPosition);

    wall.draw(&objectShaderProgram, GL_QUADS);

    guitarParts[0]->draw(&objectShaderProgram, GL_POLYGON);
    guitarParts[1]->draw(&objectShaderProgram, GL_POLYGON);
    guitarParts[2]->draw(&objectShaderProgram, GL_POLYGON);
    guitarParts[3]->draw(&objectShaderProgram, GL_POLYGON);
    guitarParts[4]->draw(&objectShaderProgram, GL_QUADS);
    guitarParts[5]->draw(&objectShaderProgram, GL_QUADS);
    guitarParts[6]->draw(&objectShaderProgram, GL_QUADS);
    guitarParts[7]->draw(&objectShaderProgram, GL_POLYGON);
    guitarParts[8]->draw(&objectShaderProgram, GL_QUADS);
    guitarParts[9]->draw(&objectShaderProgram, GL_LINES);

    objectShaderProgram.release();

    lampShaderProgram.bind();
    lampShaderProgram.setUniformValue("projectionMatrix", projectionMatrix);
    lampShaderProgram.setUniformValue("viewMatrix", viewMatrix);
    lampShaderProgram.setUniformValue("Color", light.diffuse);
    QVector3D lampScale = { 0.2f, 0.2f, 0.2f };
    lamp.scale(lampScale);
    lamp.draw(&lampShaderProgram, GL_QUADS);
    if (options.drawCoordinates) {
        lampShaderProgram.setUniformValue("Color", 1.0f, 1.0f, 1.0f);
        coordinates.draw(&lampShaderProgram, GL_LINES);
    }

    lampShaderProgram.release();
}

void MyOpenGL::changeDisplayOptions(int index, bool invisible, bool coordinates)
{
    options.projection = index;
    options.drawInvisible = invisible;
    options.drawCoordinates = coordinates;
    update();
}

void MyOpenGL::changeObjectOptions(QVector3D& scaleVec, QVector3D& translateVec)
{
    for (auto& obj: guitarParts) {
        obj->scale(scaleVec);
        obj->setPosition(translateVec);
    }
    update();
}

void MyOpenGL::changeObjectMaterialOprions(QVector3D ambient, QVector3D diffuse, QVector3D specular, float shininess)
{
    Object::Material material = {
        ambient,
        diffuse,
        specular,
        shininess
    };
    for (auto& obj: guitarParts) {
        obj->changeMaterialProperties(material);
    }
    update();
}

void MyOpenGL::changeLightOptions(int type, QVector3D position, QVector3D ambient, QVector3D diffuse)
{
    light.type = type;
    light.position = position;
    lamp.setPosition(position);
    light.ambient = ambient;
    light.diffuse = diffuse;
    update();
}
