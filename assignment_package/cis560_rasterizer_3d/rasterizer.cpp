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
            Line AB(glm::vec2(polygon.m_verts[polygon.m_tris[i].m_indices[0]].m_pos.x, polygon.m_verts[polygon.m_tris[i].m_indices[0]].m_pos.y),
                    glm::vec2(polygon.m_verts[polygon.m_tris[i].m_indices[1]].m_pos.x, polygon.m_verts[polygon.m_tris[i].m_indices[1]].m_pos.y));
            Line AC(glm::vec2(polygon.m_verts[polygon.m_tris[i].m_indices[0]].m_pos.x, polygon.m_verts[polygon.m_tris[i].m_indices[0]].m_pos.y),
                    glm::vec2(polygon.m_verts[polygon.m_tris[i].m_indices[2]].m_pos.x, polygon.m_verts[polygon.m_tris[i].m_indices[2]].m_pos.y));
            Line BC(glm::vec2(polygon.m_verts[polygon.m_tris[i].m_indices[1]].m_pos.x, polygon.m_verts[polygon.m_tris[i].m_indices[1]].m_pos.y),
                    glm::vec2(polygon.m_verts[polygon.m_tris[i].m_indices[2]].m_pos.x, polygon.m_verts[polygon.m_tris[i].m_indices[2]].m_pos.y));
            for (int j = box[1]; j <= box[3]; j++) {
                float minX = 512.0f;
                float maxX = 0.0f;
                float interAB = 0;
                float interAC = 0;
                float interBC = 0;
                if (AB.getIntersection(j, &interAB)) {
                    minX = std::min(minX, interAB);
                    maxX = std::max(maxX, interAB);
                }
                if (AC.getIntersection(j, &interAC)) {
                    minX = std::min(minX, interAC);
                    maxX = std::max(maxX, interAC);
                }
                if (BC.getIntersection(j, &interBC)) {
                    minX = std::min(minX, interBC);
                    maxX = std::max(maxX, interBC);
                }
                for (int k = minX; k <= maxX; k++) {
                    if (k < 0 || k > 512) {
                        std::cout << k << " " << "minX" << minX << " " << "maxX " << maxX;
                        break;
                    }
                    result.setPixel(k, j, qRgb(0, 0, 255));
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
