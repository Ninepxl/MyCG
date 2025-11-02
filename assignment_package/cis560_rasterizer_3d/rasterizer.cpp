#include "rasterizer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include "line.h"

Rasterizer::Rasterizer(const std::vector<Polygon>& polygons)
    : m_polygons(polygons)
{}

QImage Rasterizer::RenderScene()
{
    QImage result(512, 512, QImage::Format_RGB32);
    // Fill the image with black pixels.
    // Note that qRgb creates a QColor,
    // and takes in values [0, 255] rather than [0, 1].
    result.fill(qRgb(0.f, 0.f, 0.f));
    for (const auto& polygon : m_polygons) {
        for (size_t i = 0; i < polygon.m_tris.size(); i++) {
            auto box = GetTriangleBoundingBox(polygon, i);
            auto lines = this->GetTriangleEdgeSegment(polygon, i);
            auto tri = polygon.m_tris[i];
            std::array<Vertex, 3> triVertex{polygon.m_verts[tri.m_indices[0]], polygon.m_verts[tri.m_indices[1]], polygon.m_verts[tri.m_indices[2]]};
            for (int row = box[1]; row <= box[3]; row++) {
                float xLeft = 511.0f;
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
                    auto bary = BarycentricInterpolate(triVertex[0], triVertex[1], triVertex[2], {k, row});
                    glm::vec3 color = bary[0] * triVertex[0].m_color
                                      + bary[1] * triVertex[1].m_color
                                      + bary[2] * triVertex[2].m_color;
                    result.setPixel(k, row, qRgb(color.r, color.g, color.b));
                }
            }
        }
    }

    // TODO: Complete the various components of code that make up this function.
    // It should return the rasterized image of the current scene.

    // Make liberal use of helper functions; writing your rasterizer as one
    // long RenderScene function will make it (a) hard to debug and
    // (b) hard to write without copy-pasting. Also, Adam will be sad when
    // he reads your code.

    // Also! As per the style requirements for this assignment, make sure you
    // use std::arrays to store things like your line segments, Triangles, and
    // vertex coordinates. This lets you easily use loops to perform operations
    // on your scene components, rather than copy-pasting operations three times
    // each!
    return result;
}

void Rasterizer::ClearScene()
{
    m_polygons.clear();
}

std::array<Line, 3> Rasterizer::GetTriangleEdgeSegment(const Polygon& polygon, size_t i) {
    std::array<Line, 3> lines{};
    const auto& tri = polygon.m_tris[i];
    for (int j = 0; j < 3; j++) {
        auto vert1 = polygon.m_verts[tri.m_indices[j]];
        int nextIndex = j == 2 ? 0 : j + 1;
        auto vert2 = polygon.m_verts[tri.m_indices[nextIndex]];
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
