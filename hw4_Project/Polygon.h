#ifndef	POLYGON_H
#define	POLYGON_H
#include <vector>
#include <set>
#include "Vertex.h"
#include "Matrix4.h"
#include "iritprsr.h"
#include "Modules.h"
#include <memory>
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include "Shader.h"

struct LineKeyHash {
    std::size_t operator()(const Line key) const {

        std::hash<float> hasher;
        return hasher(key.m_a.x) ^ (hasher(key.m_a.y) << 1) ^ (hasher(key.m_a.z) << 2) ^
            (hasher(key.m_b.x)) ^ (hasher(key.m_b.y) << 1) ^ (hasher(key.m_b.z) << 2);
    }
};
struct LineKeyEqual {
    bool operator()(const Line& lhs,
        const Line& rhs) const {
        return Line::isTheSameOrFliped(lhs, rhs);
    }
};

struct VectorKeyHash {
    std::size_t operator()(const Vector3 key) const {

        std::hash<float> hasher;
        return hasher(key.x) ^ (hasher(key.y) << 1) ^ (hasher(key.z) << 2);
    }
};
struct VectorKeyEqual {
    bool operator()(const Vector3& lhs,
        const Vector3& rhs) const {
        return lhs == rhs;
    }
};


//might be useful
class BBox {
    Vector3 m_minBounds, m_maxBounds;
public:
    BBox() : m_minBounds(Vector3(FLT_MAX, FLT_MAX, FLT_MAX)), m_maxBounds(Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX)){};
    BBox(const Vector3& minBound, const Vector3& maxBound) : m_minBounds(minBound), m_maxBounds(maxBound) {};
    Vector3 getMax()const;
    Vector3 getMin() const;
    void updateBBox(const Vector3& vert);
    void updateBBox(const BBox& box);
    bool bboxCollide(const BBox& bbox) const;
    BBox transformBBox(const Matrix4& tmat) const;
    static bool bboxCollide(const BBox& bbox1, const BBox& bbox2);
    static BBox unitBBox() { return BBox(-Vector3::one(), Vector3::one()); }
    std::vector<Line> getLinesOfBbox(const ColorGC& bBoxColor) const;
    void toPrint() const;
};

//might be useful


class PolygonGC {
private:
    std::vector<std::shared_ptr< Vertex>> m_vertices; // List of vertices
    ColorGC m_primeColor;                // Color of the polygon
    ColorGC m_sceneColor;
    BBox m_bbox;
    Line m_calcNormalLine;
    Line m_dataNormalLine;
    bool m_hasDataNormal;
    bool m_vertHaveDataNormal;
    bool m_visible;
    void updateBounds(const Vertex& vert);
    void resetBounds();
    Vector3 calculateNormal() const;
    void flipNormals();
public:
    // Constructors adn distrafctors    
    PolygonGC(ColorGC color);
    ~PolygonGC() = default;


    //getters and setters
    void setCalcAndDataNormalLines( Vector3 dataNormal);
    void setCalcNormalLines();
    Vector3 getCalcNormalNormolized();
    Vector3 getDataNormalNormolized();
    Line getCalcNormalLine(const ColorGC* overridingColor) const;
    Line getDataNormalLine(const ColorGC* overridingColor) const;
    void setColor(const ColorGC& newColor);
    void setSceneColor(const ColorGC& newColor);
    ColorGC getColor() const;
    ColorGC getSceneColor() const;
    BBox getBbox() const;
    bool hasDataNormalLine() const;
    bool hasVertsDataNormalLine() const;

    void setVisibility(bool isVisble);
    bool isVisible()const;

    //utils
    bool isClippedByBBox(const Matrix4& tMat) const;    
    void addVertex(std::shared_ptr<Vertex> vertex);    
    void clip();
    bool isBehindCamera() const;
    size_t vertexCount() const;    
    PolygonGC* applyTransformation(const Matrix4& transformation, bool flipNormals) const;
    PolygonGC* applyTransformationAndFillMap(const Matrix4& transformation, bool flipNormals, std::unordered_map<Vector3, std::shared_ptr<Vertex>, VectorKeyHash, VectorKeyEqual>& map) const;
    void loadSilhoutteToContainer(std::unordered_map<Line, EdgeMode, LineKeyHash, LineKeyEqual>& SilhoutteMap)const;
    void loadVertexEdgesToContainer(std::vector<std::pair<std::shared_ptr<Vertex>, std::shared_ptr<Vertex>>>& container, const ColorGC* overridingColor) const;
    void loadEdgesToContainer(std::vector<Line>& container, const ColorGC* overridingColor) const;
    void loadBboxLinesToContainer(std::vector<Line>& container, const ColorGC* overridingColor) const;
    void loadVertNLinesFromData(std::vector<Line>& container, const ColorGC* overridingColor)const;
    void loadVertNLinesFromCalc(std::vector<Line>& container, const ColorGC* overridingColor) const;
    void loadLines(std::vector<Line> lines[LineVectorIndex::LAST], RenderMode& renderMode, std::unordered_map<Line, EdgeMode, LineKeyHash, LineKeyEqual>& SilhoutteMap) const;
    void fillGbuffer(gData* gBuffer, int width, int hight, const RenderMode& rm)const;
    void fillBasicSceneColors(const Shader& shader, const RenderMode& rm);

    //printers
    void printVertices() const;
    void printBounds() const;
    void printColor() const;
};
#endif