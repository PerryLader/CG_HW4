#include "Geometry.h"

Geometry::Geometry(const std::string& name, const ColorGC& color) : m_polygons(1),m_name(name) ,m_bBox(), m_objColor(color), hasVertDataNormal(true) , hasPolyDataNormal(true){}

Geometry::Geometry(const std::string& name, const ColorGC& color, int partitions) : m_polygons(partitions), m_name(name), m_bBox(), m_objColor(color), hasVertDataNormal(true), hasPolyDataNormal(true) {}

// Destructor
Geometry::~Geometry() {
	//for (PolygonGC* polygon : m_polygons) {
	//	delete polygon;
	//}
	m_polygons.clear();
}

BBox Geometry::getBBox() const{
	return m_bBox;
}

ColorGC Geometry::getColor() const
{
	return this->m_objColor;
}

void Geometry::setColor(ColorGC newColor)
{
	this->m_objColor = newColor;
}

void Geometry::setAlpha(uint8_t alpha)
{
	this->m_objColor.setAlpha(alpha);
	auto transformPartition = [&](const std::vector<std::shared_ptr<PolygonGC>>& partition) {
		for (const auto& poly : partition) {
			poly->setAlpha(alpha);
		}
		};
	run_in_parts(transformPartition);
}

std::string Geometry::getName() const{
	return this->m_name;
}

void Geometry::addPolygon(std::shared_ptr<PolygonGC> poli)
{
	this->m_polygons[0].push_back(poli);
	if (!poli->hasDataNormalLine())
		hasPolyDataNormal = false;
	if(!poli->hasVertsDataNormalLine())
		hasVertDataNormal = false;
	m_bBox.updateBBox(poli->getBbox());
}

void Geometry::addPolygon(std::shared_ptr<PolygonGC> poly, int index) {
    if (index < m_polygons.size()) {
        this->m_polygons[index].push_back(poly);
    }
}

std::unique_ptr<Geometry> Geometry::applyTransformation(const Matrix4& tMat, bool flipNormals, int partitions) const {
    std::unique_ptr<Geometry> res = std::make_unique<Geometry>(m_name, this->m_objColor, partitions);
    std::vector<std::thread> threads;
    std::mutex resMutex;

    // Calculate the total number of polygons
    int totalPolygons = 0;
    for (const auto& partition : m_polygons) {
        totalPolygons += partition.size();
    }

    // Calculate the number of polygons per partition
    int polygonsPerPartition = (totalPolygons + partitions - 1) / partitions; // Round up division

    auto transformPartition = [&](int partitionIndex) {
        int startIndex = partitionIndex * polygonsPerPartition;
        int endIndex = std::min(startIndex + polygonsPerPartition, totalPolygons);
        int currentIndex = 0;

        for (const auto& subPartition : m_polygons) {
            for (const auto& poly : subPartition) {
                if (currentIndex >= startIndex && currentIndex < endIndex) {
                    if (!poly->isClippedByBBox(tMat)) {
                        auto transformedPolygon = poly->applyTransformation(tMat, flipNormals);
                        res->addPolygon(transformedPolygon, partitionIndex);
                    }
                }
                ++currentIndex;
            }
        }
    };

    for (int i = 0; i < partitions; ++i) {
        threads.emplace_back(transformPartition, i);
    }

    for (auto& thread : threads) {
        thread.join();
    }
	res->resetBounds();
    return res;
}

void Geometry::run_in_parts(const std::function<void(const std::vector<std::shared_ptr<PolygonGC>>& part)>& func) {
	std::vector<std::thread> threads;

	for (const auto& part : m_polygons) {
		threads.emplace_back(func, std::cref(part));
	}

	for (auto& thread : threads) {
		thread.join();
	}
}
void Geometry::run_in_parts(const std::function<void(const std::vector<std::shared_ptr<PolygonGC>>& part)>& func) const{
	std::vector<std::thread> threads;

	for (const auto& part : m_polygons) {
		threads.emplace_back(func, std::cref(part));
	}

	for (auto& thread : threads) {
		thread.join();
	}
}
void Geometry::calcVertxNormal()
{
	for (auto& t : m_map)
	{
		if(auto vert = t.second.lock())
			vert->setCalcNormalLine();
	}
}

void Geometry::backFaceCulling(bool isPerspective , const Matrix4 tMat_inv) {
	auto transformPartition = [&](const std::vector<std::shared_ptr<PolygonGC>>& partition) {
		for (const auto& poly : partition) {
			float dot_prod;
			Line poly_normal_camera_coord = poly->getCalcNormalLine(nullptr);
			if (isPerspective) {
				poly_normal_camera_coord = poly_normal_camera_coord.getTransformedLine(tMat_inv);
				dot_prod = Vector3::dot(poly_normal_camera_coord.m_a, poly_normal_camera_coord.direction());
			}
			else
				dot_prod = Vector3::dot(Vector3::unitZ(), poly_normal_camera_coord.direction());
			if (dot_prod > 0)
			{
				poly->setVisibility(false);
			}
		}
	};
	run_in_parts(transformPartition);
}

void Geometry::createObjBboxLines(std::vector<Line> lines[LineVectorIndex::LAST], const ColorGC* wireColor) const
{
	std::vector<Line> bBoxLines = this->getBBox().getLinesOfBbox(*wireColor);
	lines[LineVectorIndex::OBJ_BBOX].insert(lines[LineVectorIndex::OBJ_BBOX].end(), bBoxLines.begin(), bBoxLines.end());	
}
void validateRenderModeToPolyData(bool hasPolyDataNormal, bool hasVertDataNormal, RenderMode& rm) {
	if (rm.getRenderDynemic())
	{
		if (hasPolyDataNormal)
		{
			if (rm.getPolygonsUseCNormalFlag())
			{
				rm.setPolygonsUseCNormalFlag();
			}
		}
		else
		{
			if (!rm.getPolygonsUseCNormalFlag())
			{
				rm.setPolygonsUseCNormalFlag();
			}
		}
		if (hasVertDataNormal)
		{
			if (rm.getVertexUseCNormalFlag())
			{
				rm.setVertexUseCNormalFlag();
			}
		}
		else
		{
			if (!rm.getVertexUseCNormalFlag())
			{
				rm.setVertexUseCNormalFlag();
			}
		}
	}
	else
	{
		if (!rm.getVertexUseCNormalFlag())
		{
			rm.setVertexUseCNormalFlag();
		}
		if (!rm.getPolygonsUseCNormalFlag())
		{
			rm.setPolygonsUseCNormalFlag();
		}
	}
}
void Geometry::fillGbuffer(GBuffer& gBuffer, RenderMode& rm) const {
	validateRenderModeToPolyData(hasPolyDataNormal, hasVertDataNormal, rm);
	auto transformPartition = [&](const std::vector<std::shared_ptr<PolygonGC>>& partition) {
		for (const auto& poly : partition) if (!rm.getRenderCulledFlag() ||(rm.getRenderCulledFlag() && poly->isVisible())) {
			poly->fillGbuffer(gBuffer, rm);
		}
	};
	run_in_parts(transformPartition);
}

void Geometry::resetBounds()
{
	if (m_polygons.empty()) {
		m_bBox = BBox();
		return;
	}
	m_bBox = BBox(Vector3(FLT_MAX, FLT_MAX, FLT_MAX), Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX));
	hasPolyDataNormal = true;
	hasVertDataNormal = true;
	for (auto& partition : m_polygons) {
		for (auto& poly : partition) {
			m_bBox.updateBBox(poly->getBbox());
			if (!poly->hasDataNormalLine())
				hasPolyDataNormal = false;
			if (!poly->hasVertsDataNormalLine())
				hasVertDataNormal = false;
		}
	}
}

void Geometry::loadLines(std::vector<Line> lines[LineVectorIndex::LAST],
	RenderMode& renderMode, std::unordered_map<Line, EdgeMode, LineKeyHash, LineKeyEqual>& SilhoutteMap) const
{
	const BBox unit = BBox::unitBBox();
	const ColorGC* wfClrOverride = renderMode.getRenderOverrideWireColorFlag() ? &renderMode.getWireColor() : &this->m_objColor;
	if (renderMode.getRenderObjBboxFlag()) {
		std::vector<Line> bBoxLines = this->getBBox().getLinesOfBbox(*wfClrOverride);
		lines[LineVectorIndex::OBJ_BBOX].insert(lines[LineVectorIndex::OBJ_BBOX].end(), bBoxLines.begin(), bBoxLines.end());
	}
	for (const auto& partition : m_polygons) {
		for (const auto& p : partition) {
			if (BBox::bboxCollide(p->getBbox(), unit)) {
				p->loadLines(lines, renderMode, SilhoutteMap);
			}
		}
	}
}

void Geometry::clip() {
	auto transformPartition = [&](const std::vector<std::shared_ptr<PolygonGC>>& partition) {
		for (const auto& poly : partition) {
			poly->clip();
		}
	};
	run_in_parts(transformPartition);
	this->resetBounds();
}

bool Geometry::isClippedByBBox(const Matrix4& tMat) const {
	return !BBox::bboxCollide(getBBox().transformBBox(tMat), BBox::unitBBox());
}
void Geometry::print() const
{
	int i = 0;
	for (const auto& temp : m_polygons)
	{
		for (const auto& poly : temp)
		{
			std::cout << "		Polygon[" << i << "] vertices:" << std::endl;
			poly->printVertices();
			i++;
		}
	}
}

void Geometry::fillBasicSceneColors(const Shader& shader, RenderMode& rm)
{
	validateRenderModeToPolyData(hasPolyDataNormal, hasVertDataNormal, rm);
	auto transformPartition = [&](const std::vector<std::shared_ptr<PolygonGC>>& partition) {
		for (const auto& poly : partition) {
			poly->fillBasicSceneColors(shader, rm);
		}
	};
	run_in_parts(transformPartition);			
}
