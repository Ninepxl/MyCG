#include "rasterizer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include "line.h"
#include <iostream>
#include <algorithm>
Rasterizer::Rasterizer(const std::vector<Polygon>& polygons)
    : m_polygons(polygons), zBuffer(std::vector<float>(512 * 512, std::numeric_limits<float>::max())),
    width(512.f), height(512.f), modelMatrix(glm::mat4(1.f)), rotateAngle(0), camera(Camera()), simple(1),
    lightDirection(glm::normalize(glm::vec3(1, 1, 1))), lightColor(glm::vec3(1.0f, 1.0f, 1.0f)), kd(1), shading(false)
{}

QImage Rasterizer::RenderScene()
{
    QImage result(512, 512, QImage::Format_RGB32);
    result.fill(qRgb(0.f, 0.f, 0.f));
    std::fill(zBuffer.begin(), zBuffer.end(), std::numeric_limits<float>::max());

    // MVP 矩阵准备
    auto projectMatrix = camera.GetProjectMatrix();
    auto viewMatrix = camera.GetCameraMatrix();
    float rad = glm::radians((float)this->rotateAngle);
    modelMatrix = {glm::vec4{glm::cos(rad), 0, -glm::sin(rad), 0},
                   glm::vec4{0, 1, 0, 0},
                   glm::vec4{glm::sin(rad), 0, glm::cos(rad), 0},
                   {0, 0, 0, 1}};

    auto normalMatrix = glm::transpose(glm::inverse(modelMatrix));
    for (auto& polygon : m_polygons) {
        // 遍历多边形中的三角形
        for (size_t i = 0; i < polygon.m_tris.size(); i++) {
            auto tri = polygon.m_tris[i];
            std::array<Vertex, 3> triVertex{polygon.m_verts[tri.m_indices[0]],
                                            polygon.m_verts[tri.m_indices[1]],
                                            polygon.m_verts[tri.m_indices[2]]};
            std::array<glm::vec3, 3> triWordPos;
            for (int i = 0; i < 3; i++) {
                triWordPos[i] = glm::vec3(modelMatrix * triVertex[i].m_pos);
            }
            for (Vertex& vertex : triVertex) {
                // 模型空间 -> 世界空间 -> 相机空间 -> 裁剪空间
                vertex.m_normal = normalMatrix * vertex.m_normal;
                vertex.m_pos = projectMatrix * viewMatrix * modelMatrix * vertex.m_pos;
                // 透视除法
                vertex.m_pos /= vertex.m_pos.w;
                vertex.m_pos.x = (vertex.m_pos.x + 1.0f) * 0.5f * width;
                vertex.m_pos.y = (1.0f - vertex.m_pos.y) * 0.5f * height;
            }

            if (IsFrontFacing(triVertex)) {
                auto box = GetTriangleBoundingBox(triVertex);
                auto lines = this->GetTriangleEdgeSegment(triVertex);
                // 光栅化
                SweepLineFillPixel(box, lines, triVertex, polygon.mp_texture, result, triWordPos);
            }
        }
    }
    return result;
}

void Rasterizer::ClearScene()
{
    m_polygons.clear();
}


void Rasterizer:: SweepLineFillPixel(const std::array<float, 4>& box, const std::array<Line, 3>& lines,
                                    const std::array<Vertex, 3>& triVertex,
                                    const QImage* const texture, QImage& result,
                                    const std::array<glm::vec3, 3> wordPos) {
    for (int row = box[1]; row <= box[3]; row++) {
        if (row < 0 || row >= 512.f) {
            continue;
        }
        float xLeft = 512.0f;
        float xRight = 0.0f;
        for (auto line : lines) {
            float interX = 0;
            if (line.getIntersection(row, &interX)) {
                xLeft = std::min(xLeft, interX);
                xRight = std::max(xRight, interX);
            }
        }
        for (int k = xLeft; k <= xRight; k++) {
            if (k < 0 || k >= 512) {
                continue;
            }
            // 获得重心坐标
            auto bary = BarycentricInterpolate(triVertex[0], triVertex[1], triVertex[2], {k + 0.5, row + 0.5});
            glm::vec2 uz;
            float zInv = 0;
            glm::vec3 nz;
            glm::vec3 frameWordPos;
            for (int i = 0; i < 3; i++) {
                zInv += bary[i] * (1 / triVertex[i].m_pos.z);
                uz += bary[i] * (triVertex[i].m_uv / triVertex[i].m_pos.z);
                nz += bary[i] * (glm::vec3(triVertex[i].m_normal) / triVertex[i].m_pos.z);
                frameWordPos += bary[i] * wordPos[i];
            }
            glm::vec2 uv = uz / zInv;
            glm::vec3 n = glm::normalize(nz / zInv);

            if (zBuffer[row * width + k] < zInv) {
                continue;
            }
            glm::vec3 color = simple > 1 ? this->SSAAColorSimple(k, row, triVertex, texture) : GetImageColor(uv, texture);
            if (shading) {
                auto diffColor = DiffuseShader(n);
                auto specularColor = SpecularShader(n, frameWordPos);
                color = (specularColor + diffColor) * color;
            }
            zBuffer[row * width + k] = zInv;
            color.r = std::clamp(color.r, 0.f, 255.f);
            color.g = std::clamp(color.g, 0.f, 255.f);
            color.b = std::clamp(color.b, 0.f, 255.f);
            result.setPixel(k, row, qRgb(color.r, color.g, color.b));
        }
    }
}

std::array<Line, 3> Rasterizer::GetTriangleEdgeSegment(const std::array<Vertex, 3>& triVertex) {
    std::array<Line, 3> lines{};
    for (int j = 0; j < 3; j++) {
        const auto& vert1 = triVertex[j];
        int nextIndex = j == 2 ? 0 : j + 1;
        const auto& vert2 = triVertex[nextIndex];
        lines[j] = Line(glm::vec2(vert1.m_pos.x, vert1.m_pos.y), glm::vec2(vert2.m_pos.x, vert2.m_pos.y));
    }
    return lines;
}

glm::vec3 Rasterizer::BarycentricInterpolate(const Vertex& v0, const Vertex& v1, const Vertex& v2, const glm::vec2& point) {
    float x0 = v0.m_pos.x, y0 = v0.m_pos.y;
    float x1 = v1.m_pos.x, y1 = v1.m_pos.y;
    float x2 = v2.m_pos.x, y2 = v2.m_pos.y;
    float px = point.x, py = point.y;

    float area = (x1 - x0) * (y2 - y0) - (x2 - x0) * (y1 - y0);

    float lambda0 = ((x1 - px) * (y2 - py) - (x2 - px) * (y1 - py)) / area;
    float lambda1 = ((x2 - px) * (y0 - py) - (x0 - px) * (y2 - py)) / area;
    float lambda2 = ((x0 - px) * (y1 - py) - (x1 - px) * (y0 - py)) / area;

    return glm::vec3(lambda0, lambda1, lambda2);
}

glm::vec3 Rasterizer::SSAAColorSimple(int x, int y, const std::array<Vertex, 3>& triVertex, const QImage* const texture) {
    float step = 1.0f / simple;
    int simpleCount = 0;
    glm::vec3 colorSum(0.0f);
    for (int i = 0; i < simple; i++) {
        for (int j = 0; j < simple; j++) {
            float sampleX = x + (j + 0.5f) * step;
            float sampleY = y + (i + 0.5f) * step;
            auto bary = BarycentricInterpolate(triVertex[0], triVertex[1], triVertex[2], {sampleX, sampleY});

            glm::vec2 uz(0.0f);
            float zInv = 0;
            for (int vi = 0; vi < 3; vi++) {
                zInv += bary[vi] * (1 / triVertex[vi].m_pos.z);
                uz += bary[vi] * (triVertex[vi].m_uv / triVertex[vi].m_pos.z);
            }

            glm::vec2 uv = uz / zInv;
            glm::vec3 color = GetImageColor(uv, texture);

            colorSum += color;
            simpleCount++;
        }
    }
    return colorSum / (float)simpleCount;
}

std::array<float, 4> Rasterizer::GetTriangleBoundingBox(const std::array<Vertex, 3>& triVertex) {
    std::array<float, 4> box; // {minX, minY, maxX, maxY}
    box[0] = triVertex[0].m_pos.x;
    box[2] = triVertex[0].m_pos.x;
    box[1] = triVertex[0].m_pos.y;
    box[3] = triVertex[0].m_pos.y;
    for (int i = 1; i < 3; i++) {
        const glm::vec4& pos = triVertex[i].m_pos;
        box[0] = std::min(pos.x, box[0]);
        box[2] = std::max(pos.x, box[2]);
        box[1] = std::min(pos.y, box[1]);
        box[3] = std::max(pos.y, box[3]);
    }
    for (auto & pos : box) {
        pos = std::clamp(pos, 0.f, 512.f);
    }
    return box;
}

bool Rasterizer::IsFrontFacing(const std::array<Vertex, 3>& triVertex) {
    glm::vec2 edge1{triVertex[1].m_pos.x - triVertex[0].m_pos.x, triVertex[1].m_pos.y - triVertex[0].m_pos.y};
    glm::vec2 edge2{triVertex[2].m_pos.x - triVertex[0].m_pos.x, triVertex[2].m_pos.y - triVertex[0].m_pos.y};
    return edge1.x * edge2.y - edge1.y * edge2.x < 0;
}

glm::vec3 Rasterizer::DiffuseShader(const glm::vec3& nor) {
    float diffuseFactor = std::max(0.f, glm::dot(nor, this->lightDirection));
    return diffuseFactor * this->lightColor;
}

glm::vec3 Rasterizer::SpecularShader(const glm::vec3& nor, const glm::vec3& framePos) {
    glm::vec3 v = glm::normalize(this->camera.GetCameraPos() - framePos);
    glm::vec3 h = glm::normalize(v + this->lightDirection);
    float shininess = 32.f;
    float specularFactor = std::pow(std::max(0.f, glm::dot(nor, h)), shininess);
    return specularFactor * this->lightColor;
}
