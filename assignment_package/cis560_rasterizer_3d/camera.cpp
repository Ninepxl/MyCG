#include "camera.h"

Camera::Camera() : forward(glm::vec4{0, 0, -1, 0}), right(glm::vec4{1, 0, 0 ,0}), up(glm::vec4{0, 1, 0, 0}),
    pos(glm::vec4{0, 0, 10, 1}), fov(45.f), near(0.01f), far(100.f), aspect(1.0f)
{}

glm::mat4 Camera::GetCameraMatrix() {
    glm::mat4 toOrigin{glm::vec4{1, 0, 0, -pos.x},
              glm::vec4{0, 1, 0, -pos.y},
              glm::vec4{0, 0, 1, -pos.z},
              glm::vec4{0, 0, 0, 1}};
    glm::mat4 cameraMatrix{right, up, forward, glm::vec4{0, 0, 0, 1}};
    return glm::transpose(cameraMatrix) * toOrigin;
}

glm::mat4 Camera::GetProjectMatrix() {
    float t = glm::tan(fov / 2) * near;
    float b = -t;
    float r = t * aspect;
    float l = -r;
    glm::mat4 scaleMatrix{glm::vec4{2 / (r - l), 0, 0, 0},
                          glm::vec4{0, 2 / (t - b), 0, 0},
                          glm::vec4{0, 0, 2 / (far - near), 0},
                          glm::vec4{0, 0, 0, 1}};
    glm::mat4 translMatrix{glm::vec4(1, 0, 0, 0),
                           glm::vec4{0, 1, 0, 0},
                           glm::vec4{0, 0, 1, 0},
                           glm::vec4{-(r + l) / 2, -(t + b) / 2, -(near + far) / 2, 1}};
    glm::mat4 ortho = scaleMatrix * translMatrix;
    glm::mat4 perspToOrtho{glm::vec4{near, 0, 0, 0},
                           glm::vec4{0, near, 0, 0},
                           glm::vec4{0, 0, (far * far - near * near) / (far - near), 1},
                           glm::vec4{0, 0, far * far - (((far * far) - (near * near)) * far / (far - near)), 0}};
    return ortho * perspToOrtho;
}
