#include "Line.h"

#include <vector>

Line::Line(const Vector3& a, const Vector3& b, ColorGC color): m_a(a), m_b(b), m_color(color) {}
// Calculate the direction vector of the line
Vector3 Line::direction() const {
    return (m_b - m_a).normalized();
}

Line Line::getTransformedLine(const Matrix4& transformation) const
{
    return Line((transformation * Vector4::extendOne(m_a)).toVector3(),
        (transformation * Vector4::extendOne(m_b)).toVector3(),
        m_color);
}

// Calculate the length of the line
float Line::length() const {
    return (m_b - m_a).length();
}

int Line::yMin() const
{
    return m_a.y < m_b.y ? m_a.y : m_b.y;
}
int Line::yMax() const
{
    return m_a.y > m_b.y ? m_a.y : m_b.y;
}

void Line::flipLine()
{
    Vector3 temp = m_b-m_a;
    m_b = m_a-temp;
}


static bool isPointInUnitCube(const Vector3& point) {
    return (point.x >= -1 && point.x <= 1 &&
        point.y >= -1 && point.y <= 1 &&
        point.z >= -1 && point.z <= 1);
}

bool Line::isPointOnLineBetween(const Line& line, const Vector3& point) {
    return (point.x >= std::min(line.m_a.x, line.m_b.x) && point.x <= std::max(line.m_a.x, line.m_b.x)) &&
        (point.y >= std::min(line.m_a.y, line.m_b.y) && point.y <= std::max(line.m_a.y, line.m_b.y)) &&
        (point.z >= std::min(line.m_a.z, line.m_b.z) && point.z <= std::max(line.m_a.z, line.m_b.z));
}

bool Line::operator==(const Line& other) const
{
   
    return (m_a == other.m_a) && (m_b == other.m_b);
    
}

std::pair<bool, Vector3> Line::linePlaneIntercetion(const Line& line, const Vector3& planeNormal, const Vector3& planePoint) {
    float nominator = Vector3::dot(planeNormal, (planePoint - line.m_a));
    const Vector3 d = (line.m_b - line.m_a).normalized();
    float denom = Vector3::dot(planeNormal, d);
    if (denom == 0) return std::pair<bool, Vector3>(false, Vector3::zero());
    Vector3 intersectionPoint = line.m_a + d * (nominator / denom);
    return  std::pair<bool, Vector3>(isPointOnLineBetween(line, intersectionPoint), intersectionPoint);
}

bool Line::isTheSameOrFliped(const Line& a, const Line& b)
{
    return  ((a.m_a == b.m_a) && (a.m_b == b.m_b)) || ((a.m_a == b.m_b) && (a.m_b == b.m_a));
}

bool Line::clip()
{
    bool v1In = isPointInUnitCube(m_a);
    bool v2In = isPointInUnitCube(m_b);
    if (v1In && v2In)
        return true;
    std::pair<bool,Vector3> planeIntersectPoint[6];
    Vector3 Intersects[2];
    int intersectionCount = 0;
    Vector3 planeNormals[6] = { {-1, 0, 0}, {1, 0, 0}, {0, -1, 0}, {0, 1, 0}, {0, 0, -1}, {0, 0, 1} };
    Vector3 planePoints[6] = { {-1, 0, 0}, {1, 0, 0}, {0, -1, 0}, {0, 1, 0}, {0, 0, -1}, {0, 0, 1} };
    for (int i = 0; i < 6; i++) {
        planeIntersectPoint[i] = linePlaneIntercetion(*this, planeNormals[i], planePoints[i]);
        planeIntersectPoint[i].second.adjustForNumericalErrors(0.01);
        planeIntersectPoint[i].first = planeIntersectPoint[i].first && isPointOnLineBetween(*this, planeIntersectPoint[i].second) && isPointInUnitCube(planeIntersectPoint[i].second);
        if (planeIntersectPoint[i].first){
            if (intersectionCount < 2) {
                Intersects[intersectionCount++] = planeIntersectPoint[i].second; 
            }
            else
                break;
        }
    }
    if (intersectionCount == 0) {
        return false;
    }
    else if (intersectionCount == 1) {
        if (v1In)
            m_b = Intersects[0];
        else if (v2In)
            m_a = Intersects[0];
        else return false;
        return true;
    }
    else if (intersectionCount == 2) {
        if (Vector3::dot(Intersects[1] - Intersects[0], m_b - m_a) > 0) {
            m_b = Intersects[1];
            m_a = Intersects[0];
        }
        else {
            m_a = Intersects[1];
            m_b = Intersects[0];
        }
        return true;
    }
    return false;
}

void Line::draw(uint32_t* m_Buffer, gData* gBuffer,int width,int hight)const
{
    // Calculate differences
    int halfWidth = width / 2;
    int halfhight = hight / 2;
    int x1 = (m_a.x * halfWidth) + halfWidth;
    int x2 = (m_b.x * halfWidth) + halfWidth;
    int y1 = (m_a.y*halfhight)+halfhight;
    int y2 = (m_b.y * halfhight) + halfhight;


    int dx = abs(x2 - x1), dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1; // Step for x
    int sy = (y1 < y2) ? 1 : -1; // Step for y
    int err = dx - dy;
    uint32_t color = m_color.getARGB();

    while (true)
    {        
        if ((y1 * width) + x1 < width * hight && (y1 * width) + x1 >= 0)
        {
            float t = (x1 - (m_a.x * halfWidth) + halfWidth) / ((m_b.x * halfWidth) + halfWidth - (m_a.x * halfWidth) + halfWidth);
            float interpolatedZ = (m_a.z * (1 - t)) + t * m_b.z;
            if (gBuffer[(y1 * width) + x1].z_indx > interpolatedZ)
            {
                gBuffer[(y1 * width) + x1].z_indx = interpolatedZ;
                m_Buffer[(y1 * width) + x1] = color;
            }

        }
        
        // Break when we reach the end point
        if (x1 == x2 && y1 == y2)
            break;

        // Calculate error and adjust positions
        int e2 = err * 2;
        if (e2 > -dy)
        {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx)
        {
            err += dx;
            y1 += sy;
        }
    }
}

void Line::drawSilhoutte(uint32_t* m_Buffer, gData* m_GBuffer, int width, int hight) const
{
    // Calculate differences
    int halfWidth = width / 2;
    int halfhight = hight / 2;
    int x1 = (m_a.x * halfWidth) + halfWidth;
    int x2 = (m_b.x * halfWidth) + halfWidth;
    int y1 = (m_a.y * halfhight) + halfhight;
    int y2 = (m_b.y * halfhight) + halfhight;


    int dx = abs(x2 - x1), dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1; // Step for x
    int sy = (y1 < y2) ? 1 : -1; // Step for y
    int err = dx - dy;
    uint32_t color = 0xFFFFF000;//Glowing yellow;

    while (true)
    {

        if ((y1 * width) + x1 < width * hight && (y1 * width) + x1 >= 0)
        {
            float t = (double)(x1 - (m_a.x * halfWidth) + halfWidth) / ((m_b.x * halfWidth) + halfWidth - (m_a.x * halfWidth) + halfWidth);
            float interpolatedZ = (m_a.z * (1 - t)) + t * m_b.z;
            if (m_GBuffer[(y1 * width) + x1].z_indx >= interpolatedZ-0.03)
            {
                if (x1 != 1 && x1 != 0)
                {
                    m_GBuffer[(y1 * width) + (x1 - 1)].z_indx = interpolatedZ;
                    m_Buffer[(y1 * width) + (x1 - 1)] = color;
                    m_GBuffer[(y1 * width) + (x1 - 2)].z_indx = interpolatedZ;
                    m_Buffer[(y1 * width) + (x1 - 2)] = color;
                }
                if (x1 != width - 2 && x1 != width - 1)
                {
                    m_GBuffer[(y1 * width) + (x1 + 1)].z_indx = interpolatedZ;
                    m_Buffer[(y1 * width) + (x1 + 1)] = color;
                    m_GBuffer[(y1 * width) + (x1 + 2)].z_indx = interpolatedZ;
                    m_Buffer[(y1 * width) + (x1 + 2)] = color;
                }
                if (y1 != 1 && y1 != 0)
                {
                    m_GBuffer[((y1 - 1) * width) + x1].z_indx = interpolatedZ;
                    m_Buffer[((y1 - 1) * width) + x1] = color;
                    m_GBuffer[((y1 - 2) * width) + x1].z_indx = interpolatedZ;
                    m_Buffer[((y1 - 2) * width) + x1] = color;
                }
                if (y1 != hight - 2 && y1 != hight - 1)
                {
                    m_GBuffer[((y1 + 1) * width) + x1].z_indx = interpolatedZ;
                    m_Buffer[((y1 + 1) * width) + x1] = color;
                    m_GBuffer[((y1 + 2) * width) + x1].z_indx = interpolatedZ;
                    m_Buffer[((y1 + 2) * width) + x1] = color;
                }
                m_GBuffer[(y1 * width) + x1].z_indx = interpolatedZ;
                m_Buffer[(y1 * width) + x1] = color;
            }

        }

        // Break when we reach the end point
        if (x1 == x2 && y1 == y2)
            break;

        // Calculate error and adjust positions
        int e2 = err * 2;
        if (e2 > -dy)
        {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx)
        {
            err += dx;
            y1 += sy;
        }
    }
}

// Print the line
void Line::print() {
    std::cout << "Line[";
    std::cout << "(" << m_a << ")" <<
        " -> (" << m_b << ")" << "]";

}
//
//bool Line::isInClip()
//{
//    return m_a.isInsideClipVolume()&&m_b.isInsideClipVolume();
//}
