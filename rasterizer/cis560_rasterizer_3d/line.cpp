#include "line.h"

Line::Line(const glm::vec2& startPos, const glm::vec2& endPos) : m_startPos(startPos), m_endPos(endPos),
    m_dx(endPos.x - startPos.x), m_dy(endPos.y - startPos.y) {}
Line::Line() : m_startPos(glm::vec2{0, 0}), m_endPos{glm::vec2{0, 0}}, m_dx{1}, m_dy{1} {}

bool Line::getIntersection(int y, float* x) {
    float maxY = std::max(m_startPos.y, m_endPos.y);
    float minY = std::min(m_startPos.y, m_endPos.y);
    if (y > maxY || y < minY) {
        *x = 0;
        return false;
    }

    *x = m_startPos.x + (y - m_startPos.y) * (m_dx / m_dy);
    return true;
}
