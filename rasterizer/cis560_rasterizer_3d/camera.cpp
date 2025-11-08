#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera() : forward(glm::vec4{0, 0, -1, 0}), right(glm::vec4{1, 0, 0 ,0}), up(glm::vec4{0, 1, 0, 0}),
    pos(glm::vec4{0, 0, 10, 1}), fov(glm::radians(45.f)), near(0.01f), far(100.f), aspect(1.0f)
{}

glm::mat4 Camera::GetCameraMatrix() {
    glm::mat4 toOrigin{glm::vec4{1, 0, 0, 0},
                       glm::vec4{0, 1, 0, 0},
                       glm::vec4{0, 0, 1, 0},
                       glm::vec4{ -pos.x, -pos.y, -pos.z, 1}};
    glm::mat4 cameraMatrix{right, up, forward, glm::vec4{0, 0, 0, 1}};
    return glm::transpose(cameraMatrix) * toOrigin;
}

glm::mat4 Camera::GetProjectMatrix() {
    float t = glm::tan(fov / 2) * near;
    float b = -t;
    float r = t * aspect;
    float l = -r;
    glm::mat4 scaleMatrix{glm::vec4{2.f / (r - l), 0, 0, 0},
                          glm::vec4{0, 2.f / (t - b), 0, 0},
                          glm::vec4{0, 0, 2.f / (near - far), 0},
                          glm::vec4{0, 0, 0, 1}};
    glm::mat4 translMatrix{glm::vec4(1, 0, 0, 0),
                           glm::vec4{0, 1, 0, 0},
                           glm::vec4{0, 0, 1, 0},
                           glm::vec4{-(r + l) / 2.f, -(t + b) / 2.f, -(near + far) / 2.f, 1}};
    glm::mat4 ortho = scaleMatrix * translMatrix;
    glm::mat4 perspToOrtho{glm::vec4{near, 0, 0, 0},
                           glm::vec4{0, near, 0, 0},
                           glm::vec4{0, 0, near + far, 1},
                           glm::vec4{0, 0, -near * far, 0}};
    return ortho * perspToOrtho;
}

void Camera::MoveX(float x) {
    glm::vec4 translate = right * x;
    pos += translate;
}

void Camera::MoveY(float y) {
    glm::vec4 translate = up * y;
    pos += translate;
}

void Camera::MoveZ(float z) {
    glm::vec4 translate = forward * z;
    pos += translate;
}

void Camera::RotationX(float angle) {
    float rad = glm::radians(angle);
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), rad, glm::vec3(right));

    up = rotation * up;
    forward = rotation * forward;
}

void Camera::RotationY(float angle) {
    float rad = glm::radians(angle);
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), rad, glm::vec3(up));
    right = rotation * right;
    forward = rotation * forward;
}

void Camera::RotationZ(float angle) {
    float rad = glm::radians(angle);
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), rad, glm::vec3(forward));
    right = rotation * right;
    up = rotation * up;
}
glm::vec3 Camera::GetCameraPos() {
    return glm::vec3(this->pos);
}
