#ifndef LINE_H
#define LINE_H
#include <glm/glm.hpp>
#include <algorithm>
class Line {
public:
    Line(const glm::vec2& startPos, const glm::vec2& endPos);

    /**
     * @brief getIntersection 获得 y = a 与 该线的交点
     * @param y
     * @param x
     * @return
     */
    bool getIntersection(int y, float* x);
private:
    glm::vec2 m_startPos, m_endPos;
    float m_dx;
    float m_dy;
};

#endif // LINE_H
