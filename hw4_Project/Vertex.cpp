
#include "Vertex.h"
#include "Polygon.h"

#define NORMAL_LENGTH_MODIFIER 0.25
// Constructor
Vertex::Vertex(Vector3 p) : m_point(p), m_dataNormalLine(), m_hasDataNormalLine(false), m_hasCalcNormalLine(false) {}
Vertex::Vertex(Vector3 p, Vector3 n) : m_point(p), m_hasDataNormalLine(true), m_hasCalcNormalLine(false)
{
    m_dataNormalLine = Line(p, (p + (n.normalized() * 0.25)));
}
Vertex::Vertex(Vector3 p, ColorGC c) : m_point(p), m_dataNormalLine(), m_hasDataNormalLine(false), m_hasCalcNormalLine(false), m_color(c) {}
Vertex::Vertex(Vector3 p, Vector3 n, ColorGC c) : m_point(p), m_hasDataNormalLine(true), m_hasCalcNormalLine(false), m_color(c)
{
    m_dataNormalLine = Line(p, (p + (n.normalized() * 0.25)),c);
}

Line Vertex::interpolate_cnormal(const Vertex& a, const Vertex& b, float t) {
    Vector3 loc = interpolate_loc(a, b, t);
    return Line(
        loc, loc + (a.getCalcNormalLine().direction() * (1 - t) + b.getCalcNormalLine().direction() * t).normalized() * NORMAL_LENGTH_MODIFIER);
}
Line Vertex::interpolate_dnormal(const Vertex& a, const Vertex& b, float t) {
    if (!(a.m_hasDataNormalLine && b.m_hasDataNormalLine))
        interpolate_cnormal(a, b, t);
    Vector3 loc = interpolate_loc(a, b, t);
    return Line(
        loc, loc + (a.getDataNormalLine().direction() * (1 - t) + b.getDataNormalLine().direction() * t).normalized() * NORMAL_LENGTH_MODIFIER);
}
Vector3 Vertex::interpolate_loc(const Vertex& a, const Vertex& b, float t) {
    return  (a.loc() * (1 - t)) + (b.loc() * t);
}
ColorGC Vertex::interpolate_color(const Vertex& a, const Vertex& b, float t) {
    return  (a.getColor() * (1 - t)) + (b.getColor() * t);
}

Vertex::Vertex(const Vertex &a,const Vertex &b, float t)
{
    this->m_point = interpolate_loc(a, b, t);
    this->m_calcNormalLine = interpolate_cnormal(a, b, t);
    this->m_color = interpolate_color(a, b, t);
    this->m_hasCalcNormalLine = true;
    if (a.m_hasDataNormalLine && b.m_hasDataNormalLine) {
        this->m_dataNormalLine = interpolate_dnormal(a, b, t); 
        this->m_hasDataNormalLine = true;
    }
    else
        this->m_hasDataNormalLine = false;
}
void Vertex::setFromInterpolation(const Vertex& a, const Vertex& b, float t, bool location, bool color, bool dataNorm, bool calcNorm) {
    if(dataNorm)
       if (a.m_hasDataNormalLine && b.m_hasDataNormalLine) {
            this->m_dataNormalLine = interpolate_dnormal(a, b, t);
            this->m_hasDataNormalLine = true;
       }
       else {
           calcNorm = true;
           this->m_hasDataNormalLine = false;
       }
    if (calcNorm) {
        this->m_calcNormalLine = interpolate_cnormal(a, b, t);
        this->m_hasCalcNormalLine = true;
    }
    else {
        this->m_hasCalcNormalLine = false;
    }
    if (location) {
        if (dataNorm || calcNorm)
            this->m_point = this->m_calcNormalLine.m_a;
        else
            this->m_point = interpolate_loc(a, b, t);
    }
    if (color) {
        this->m_color = interpolate_color(a, b, t);
    }

}

//getters and setters
void Vertex::setCalcNormalLine()
{
    Vector3 avrageNormal(0, 0, 0);
    for (const std::weak_ptr<PolygonGC>& weak_poly : m_neigberPolygons) if (auto poly = weak_poly.lock())
    {
        avrageNormal = avrageNormal + poly->getCalcNormalNormolized();
    }
    this->m_calcNormalLine = Line(m_point,
        m_point + ((avrageNormal * (1.0 / m_neigberPolygons.size())).normalized() * NORMAL_LENGTH_MODIFIER) , m_color);
    m_hasCalcNormalLine = true;
    //color here???
}
void Vertex::setDataNormalLine(Line normal) {
    m_dataNormalLine = normal;
    m_hasDataNormalLine = true;
}
void Vertex::flipNormals()
{
    if (m_hasCalcNormalLine)
    {
        m_calcNormalLine.flipLine();
    }
    if (m_hasDataNormalLine)
    {
        m_dataNormalLine.flipLine();
    }
}
Line Vertex::getCalcNormalLine()const
{
    if (!m_hasCalcNormalLine)
    {
        throw;
    }
    return m_calcNormalLine;
}
Line Vertex::getDataNormalLine()const 
{
    if (!m_hasDataNormalLine)
        throw std::exception();
    return m_dataNormalLine;
}
std::shared_ptr<Vertex> Vertex::getTransformedVertex(const Matrix4& transformation, bool flipNormals) const
{
    std::shared_ptr<Vertex> temp = std::make_shared<Vertex>((transformation * Vector4::extendOne(this->m_point)).toVector3());
    if (m_hasCalcNormalLine)
    {
        temp->m_hasCalcNormalLine = true;
        temp->m_calcNormalLine = this->m_calcNormalLine.getTransformedLine(transformation);
    }
    if (m_hasDataNormalLine/*(polygon*)*/)
    {
        temp->m_hasDataNormalLine = true;
        temp->m_dataNormalLine = this->m_dataNormalLine.getTransformedLine(transformation);

    }
    if (flipNormals)
    {
        temp->flipNormals();
    }
    return temp;
}
bool Vertex::hasDataNormalLine() const {
    return m_hasDataNormalLine;
}
Vector3 Vertex::loc() const    // Get location
{
    return m_point;
}
void Vertex::setLoc(const Vector3& newLoc)
{
    this->m_point = newLoc;
}

ColorGC Vertex::getColor() const
{
    return m_color;
}

void Vertex::setColor(ColorGC newColor)
{
    this->m_color = newColor;
}



//utils
void Vertex::transformVertex(const Matrix4& transformation)
{
    m_point = ((transformation * Vector4::extendOne(this->m_point)).toVector3());
    if (m_hasCalcNormalLine)
    {
        m_calcNormalLine = m_calcNormalLine.getTransformedLine(transformation);
    }
    if (m_hasDataNormalLine)
    {
        m_dataNormalLine = m_dataNormalLine.getTransformedLine(transformation);
    }
}
bool Vertex::isInsideClipVolume() {
    return m_point.x >= -1 && m_point.x <= 1 &&
        m_point.y >= -1 && m_point.y <= 1 &&
        m_point.z >= -1 && m_point.z <= 1;
}
void Vertex::print() {
    std::cout << "Vertex Located at: " << m_point << std::endl;
}

void Vertex::addNeigberPolygon(std::shared_ptr<PolygonGC> poly) { m_neigberPolygons.push_back(poly); }


std::vector<Vector3> Vertex::intersectionVertex(const std::shared_ptr<Vertex>& a, const std::shared_ptr<Vertex>& b)
{
    std::vector<Vector3> intersection;
    Line tempLine(a->loc(), b->loc());
    bool v1In = a->isInsideClipVolume();
    bool v2In = b->isInsideClipVolume();
    if (v1In && v2In)
        return intersection;
    std::pair<bool, Vector3> planeIntersectPoint[6];
    Vector3 planeNormals[6] = { {-1, 0, 0}, {1, 0, 0}, {0, -1, 0}, {0, 1, 0}, {0, 0, -1}, {0, 0, 1} };
    Vector3 planePoints[6] = { {-1, 0, 0}, {1, 0, 0}, {0, -1, 0}, {0, 1, 0}, {0, 0, -1}, {0, 0, 1} };
    for (int i = 0; i < 6; i++) {
        planeIntersectPoint[i] = Line::linePlaneIntercetion(tempLine, planeNormals[i], planePoints[i]);
        planeIntersectPoint[i].second.adjustForNumericalErrors(0.01);
        //isPointOnLineBetween is applied twice
        planeIntersectPoint[i].first = planeIntersectPoint[i].first &&
            Line::isPointOnLineBetween(tempLine, planeIntersectPoint[i].second) && planeIntersectPoint[i].second.isPointInUnitCube();
        if (planeIntersectPoint[i].first) {
            if (intersection.size() < 2) {
                intersection.push_back(planeIntersectPoint[i].second);
            }
            else
                break;
        }
    }

    if (intersection.size() == 0) {
        return intersection;
    }
    else if (intersection.size() == 1) {
        return intersection;
    }

    else if (intersection.size() == 2) {
        if (Vector3::dot(intersection[1] - intersection[0], b->loc() - a->loc()) < 0) {
            std::swap(intersection[1], intersection[0]);
        }

        return intersection;
    }
    throw;
}

std::shared_ptr<Vertex> Vertex::intersectionVertexesWithPlan(const std::shared_ptr<Vertex>& a, const std::shared_ptr<Vertex>& b, Vector3& planPos)
{
    Vector3 lineDir(b->loc() - a->loc());
    double denom = Vector3::dot(planPos, lineDir);
    if (std::abs(denom) < 1e-6) {
        // Line is parallel to the plane (no intersection or infinite)
        std::cout << "here1";
        throw;
    }
   
    double t = Vector3::dot(planPos, planPos - a->loc()) / denom;
    if (t > 1)
    {
        //after b
        std::cout << "maybe probelem here2";
        throw;
    }
    else if (t < 0)
    {
        //before a
        std::cout << "maybe probelem here3";
        throw;
    }  
    return std::make_shared<Vertex>(*a, * b,t);

}