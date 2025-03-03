#ifndef	GEOMETRY_H
#define	GEOMETRY_H
#include "Polygon.h"
#include <vector>
#include <string>
#include <unordered_map>
#include "Camera.h"
#include "Shader.h"


class Geometry
{
private:
	std::vector<std::shared_ptr<PolygonGC>> m_polygons;
	std::string m_name;
	BBox m_bBox;
	ColorGC m_objColor;
	bool hasVertDataNormal;
	bool hasPolyDataNormal;
	void createObjBboxLines(std::vector<Line> lines[LineVectorIndex::LAST], const ColorGC* wireColor) const;

public:
	std::unordered_map<Vector3, std::weak_ptr<Vertex>, VectorKeyHash, VectorKeyEqual> m_map;

	//CONTRUCTOR
	Geometry(const std::string& name, const ColorGC& color);
	~Geometry();


	//GETTERS AND SETTERS
	std::string getName() const;
	BBox getBBox() const;
	ColorGC getColor()const;
	void setColor(ColorGC newColor);
	void setAlpha(uint8_t alpha);

	//UTILS
	void fillGbuffer(GBuffer& gBuffer, RenderMode& rm) const;
	void resetBounds();
	void loadLines(std::vector<Line> lines[LineVectorIndex::LAST], RenderMode& renderMode, std::unordered_map<Line, EdgeMode, LineKeyHash, LineKeyEqual>& SilhoutteMap) const;
	void addPolygon(std::shared_ptr<PolygonGC> poli);
	std::unique_ptr<Geometry> applyTransformation(const Matrix4& tMat, bool flipNormals) const;
	void calcVertxNormal();
	void backFaceCulling(bool isPerspective, const Matrix4 tMat_inv);
	void clip();
	bool isClippedByBBox(const Matrix4& tMat) const;
	void print() const;
	void fillBasicSceneColors(const Shader& shader, RenderMode& rm);
};

#endif