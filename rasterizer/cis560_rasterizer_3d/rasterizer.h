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
    float width;
    float height;
    glm::mat4 modelMatrix;
    /**
     * @brief GetTriangleBoundingBox 得到三角形的包围盒
     * @param triVertex
     * @return
     */
    std::array<float, 4> GetTriangleBoundingBox(const std::array<Vertex, 3>& triVertex);

    /**
     * @brief GetTriangleEdgeSegment 获得三角形三个边的函数表达式
     * @param polygon 多边形
     * @param i 多边形中三角形的index
     * @return
     */
    std::array<Line, 3> GetTriangleEdgeSegment(const std::array<Vertex, 3>& triVertex);

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
    void SweepLineFillPixel(const std::array<float, 4>& box, const std::array<Line, 3>& lines,
                            const std::array<Vertex, 3>& triVertex,
                            const QImage* const texture, QImage& result,
                            const std::array<glm::vec3, 3> wordPos);

    /**
     * @brief IsFrontFacing 检查当前三角形是否是正面
     * @param tirVertex
     * @return
     */
    bool IsFrontFacing(const std::array<Vertex, 3>& tirVertex);

    /**
     * @brief SSAAColorSimple 超采样颜色
     * @param x
     * @param y
     * @param triVertex
     * @param texture
     * @return
     */
    glm::vec3 SSAAColorSimple(int x, int y, const std::array<Vertex, 3>& triVertex, const QImage* const texture);

    /**
     * @brief DiffuseShader 漫反射着色
     * @return
     */
    glm::vec3 DiffuseShader(const glm::vec3& nor);

    /**
     * @brief SpecularShader 镜面反射着色
     * @param nor
     * @return
     */
    glm::vec3 SpecularShader(const glm::vec3& nor, const glm::vec3& framePo);
public:
    int rotateAngle;
    Camera camera;
    int simple;
    glm::vec3 lightDirection;
    glm::vec3 lightColor;
    float kd; // 漫反射系数
    bool shading;
    Rasterizer(const std::vector<Polygon>& polygons);
    QImage RenderScene();
    void ClearScene();
};

