#ifndef LINE_H
#define LINE_H
#include <glm/glm.hpp>
#include <algorithm>
class Line {
public:
    Line(const glm::vec2& startPos, const glm::vec2& endPos);
    Line();
    /**
     * @brief getIntersection 获得 y = a 与 该线的交点
     * @param y
     * @param x
     * @return 返回两条线是否相交
     */
    bool getIntersection(int y, float* x);
private:
    glm::vec2 m_startPos, m_endPos;
    float m_dx;
    float m_dy;
};

#endif // LINE_H
