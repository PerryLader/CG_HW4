#ifndef	VERTEX_H
#define	VERTEX_H
#include "Matrix4.h"
#include "Vector4.h"
#include <vector>
#include "Line.h"
#include "ColorGC.h"
#include <algorithm>

class PolygonGC;


class Vertex {
private:
    Vector3 m_point;
    Line m_dataNormalLine;    
    bool m_hasDataNormalLine;  
    Line m_calcNormalLine;
    bool m_hasCalcNormalLine;
    std::vector<PolygonGC*> m_neigberPolygons;
    ColorGC m_color;

    void setDataNormalLine(Line normal);
    void flipNormals();
    
public:
    
    // Constructors
    Vertex(Vector3 p);
    Vertex(Vector3 p, Vector3 n);
    Vertex(Vector3 p, Line m_dataNormalLine,bool m_hasDataNormalLine, Line m_calcNormalLine, bool m_hasCalcNormalLine,std::vector<PolygonGC*> m_neigberPolygons);
    Vertex(const Vertex& a, const Vertex& b, float t);
    
    static Line interpolate_cnormal(const Vertex& a, const Vertex& b, float t);
    static Line interpolate_dnormal(const Vertex& a, const Vertex& b, float t);
    static Vector3 interpolate_loc(const Vertex& a, const Vertex& b, float t);
    static ColorGC interpolate_color(const Vertex& a, const Vertex& b, float t);

    //getters and setters
    void setCalcNormalLine();
    Line getDataNormalLine() const;
    Line getCalcNormalLine()const;
    bool hasDataNormalLine() const;
    Vector3 loc() const;
    void setLoc(const Vector3& newLoc);
    ColorGC getColor()const;
    void setColor(ColorGC newColor);
    void setFromInterpolation(const Vertex& a, const Vertex& b, float t, bool location, bool color, bool dataNorm, bool calcNorm);

    //utils
    std::shared_ptr<Vertex> getTransformedVertex(const Matrix4& transformation, bool flipNormals)const;
    void transformVertex(const Matrix4& transformation);
    bool isInsideClipVolume();
    void addNeigberPolygon(PolygonGC* poly);
    void print();
    //statics
    static std::vector<Vector3> intersectionVertex(const std::shared_ptr<Vertex> &a, const std::shared_ptr<Vertex>& b);

    static std::shared_ptr<Vertex> intersectionVertexesWithPlan(const std::shared_ptr<Vertex>& a, const std::shared_ptr<Vertex>& b, Vector3& planPos);
    
    static Vector3 calculate_delta_direction(const Vertex& a, const Vertex& b, int steps, bool dynamic) {
        if (dynamic) {
            if (a.hasDataNormalLine() && b.hasDataNormalLine()) {
                return ((b.getDataNormalLine().direction() - a.getDataNormalLine().direction()) / steps);
            }
        }
        return ((b.getCalcNormalLine().direction() - a.getCalcNormalLine().direction()) / steps);
    }
    static ColorGC calculate_delta_color(const Vertex& a, const Vertex& b, int steps) {
        return ((b.getColor() - a.getColor()) / steps);
    }
    static Vector3 calculate_delta_location(const Vertex& a, const Vertex& b, int steps) {
        return ((b.loc() - a.loc()) / steps);
    }
};
#endif // VERTEX_H