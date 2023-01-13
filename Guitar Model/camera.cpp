#include "camera.h"

Camera::Camera()
{
    cameraPos = { 0.0f, 0.0f, 2.5f };
    cameraFront = { 0.0f, 0.0f, -1.0f };
    cameraUp = { 0.0f, 1.0f, 0.0f };

    actions = {
        { MoveDirection::Forward, false },
        { MoveDirection::Back, false },
        { MoveDirection::Left, false },
        { MoveDirection::Right, false },
        { MoveDirection::Up, false },
        { MoveDirection::Down, false }
    };

    fov = 45.0f;
    pitch = 0.0f;
    yaw = -90.0f;

    height = 640;
    width = 480;
}

QMatrix4x4 Camera::getView()
{
    QMatrix4x4 view;
    view.lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    return view;
}

QMatrix4x4 Camera::getProjection(int type)
{
    QMatrix4x4 projection;
    if (type == 0) {
        float aspect = width / (height ? (float)height : 1.0f);
        projection.perspective(fov, aspect, 0.1f, 50.0f);
    }
    else {
        projection.ortho(-20.0f, 20.0f, -20.0f, 20.0f, -20.0f, 20.0f);
    }
    return projection;
}

QVector3D Camera::getPosition()
{
    return cameraPos;
}

void Camera::setViewport(int w, int h)
{
    width = w;
    height = h;
}

void Camera::move(MoveDirection direction, bool move)
{
    actions[direction] = move;
}

void Camera::update()
{
    doMovement();
    doRotation();
}

void Camera::zoom(float deltaAngle)
{
    fov -= deltaAngle;
    if (fov <= 1.0f) {
        fov = 1.0f;
    }
    if (fov >= 90.0f) {
        fov = 90.0f;
    }
}

void Camera::rotate(float deltaYaw, float deltaPitch)
{
    yaw += deltaYaw;
    pitch += deltaPitch;

    if (pitch > 89.0f) {
        pitch = 89.0f;
    }
    if (pitch < -89.0f) {
        pitch = -89.0f;
    }
}

qint64 currentTime;
qint64 lastTime = 0;

void Camera::doMovement()
{
    currentTime = QDateTime::currentMSecsSinceEpoch();
    qint64 deltaTime = currentTime - lastTime;
    lastTime = currentTime;
    float cameraSpeed = 0.5f / deltaTime;
    //float cameraSpeed = 0.5f;

    QVector3D cameraRight = QVector3D::crossProduct(cameraFront, cameraUp).normalized();

    if (actions.value(MoveDirection::Forward)) {
        cameraPos += cameraSpeed * cameraFront;
    }
    if (actions.value(MoveDirection::Back)) {
        cameraPos -= cameraSpeed * cameraFront;
    }
    if (actions.value(MoveDirection::Left)) {
        cameraPos -= cameraSpeed * cameraRight;
    }
    if (actions.value(MoveDirection::Right)) {
        cameraPos += cameraSpeed * cameraRight;
    }
    if (actions.value(MoveDirection::Up)) {
        cameraPos += cameraSpeed * cameraUp;
    }
    if (actions.value(MoveDirection::Down)) {
        cameraPos -= cameraSpeed * cameraUp;
    }
}

void Camera::doRotation()
{
    QVector3D front;
    front.setX(qCos(qDegreesToRadians(yaw)) * qCos(qDegreesToRadians(pitch)));
    front.setY(qSin(qDegreesToRadians(pitch)));
    front.setZ(qSin(qDegreesToRadians(yaw)) * qCos(qDegreesToRadians(pitch)));

    cameraFront = front.normalized();
}
