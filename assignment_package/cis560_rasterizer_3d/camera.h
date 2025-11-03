#ifndef CAMERA_H
#define CAMERA_H
#include <glm/glm.hpp>


class Camera {
public:
    Camera();
    glm::mat4 GetCameraMatrix();
    glm::mat4 GetProjectMatrix();
private:
    glm::vec4 forward; // 摄像机的前向方向
    glm::vec4 right; // 摄像机的右向方向
    glm::vec4 up; // 摄像机的向上方向
    glm::vec4 pos;
    float fov;
    float near;
    float far;
    float aspect;
};

#endif // CAMERA_H
