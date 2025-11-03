#pragma once
#include <polygon.h>
#include <QImage>
#include "line.h"
#include "camera.h"
class Rasterizer
{
private:
    //This is the set of Polygons loaded from a JSON scene file
    std::vector<Polygon> m_polygons;
    std::vector<float> zBuffer;
    Camera camera;
    /**
     * @brief GetTriangleEdgeSegment 获得三角形三个边的函数表达式
     * @param polygon 多边形
     * @param i 多边形中三角形的index
     * @return
     */
    std::array<Line, 3> GetTriangleEdgeSegment(const Polygon& polygon, size_t i);

    /**
     * @brief BarycentricInterpolate 得到重心坐标
     * @param v0 三角形三个顶点
     * @param v1
     * @param v2
     * @param point 要插值的点
     * @return
     */
    glm::vec3 BarycentricInterpolate(const Vertex& v0, const Vertex& v1, const Vertex& v2, const glm::vec2& point);

    /**
     * @brief InterpolateColor 插值颜色
     * @param bary
     * @param color1
     * @param color2
     * @param color3
     * @return
     */
    glm::vec3 InterpolateColor(const glm::vec3 bary, const glm::vec3 color1, const glm::vec3 color2, const glm::vec3 color3);

    /**
     * @brief SweepLineFillPixel 使用扫描线算法光栅化一个三角形
     * @param box 三角形的包围盒
     * @param lines 三角形三条边的函数表达式集合
     * @param triVertex 三角形的三个顶点集合
     */
    void SweepLineFillPixel(const std::array<float, 4>& box, const std::array<Line, 3>& lines, const std::array<Vertex, 3>& triVertex, QImage& result);

public:
    Rasterizer(const std::vector<Polygon>& polygons);
    QImage RenderScene();
    void ClearScene();
};

