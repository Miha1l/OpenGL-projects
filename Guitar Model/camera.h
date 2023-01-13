#ifndef CAMERA_H
#define CAMERA_H

#include <QMatrix4x4>
#include <QVector3D>
#include <QHash>
#include <QDateTime>

class Camera
{
public:
    enum class MoveDirection {
        Forward,
        Back,
        Left,
        Right,
        Up,
        Down
    };

    Camera();
    QMatrix4x4 getView();
    QMatrix4x4 getProjection(int type);
    QVector3D getPosition();
    void setViewport(int w, int h);
    void move(MoveDirection direction, bool move);
    void update();
    void zoom(float deltaAngle);
    void rotate(float deltaYaw, float deltaPitch);

private:
    void doMovement();
    void doRotation();

    QVector3D cameraPos;
    QVector3D cameraFront;
    QVector3D cameraUp;
    QHash<MoveDirection, bool> actions;

    float pitch;
    float yaw;
    float fov;

    int height;
    int width;

};

#endif // CAMERA_H
