#include "Polygon.h"
#include <afxwin.h>
#include <unordered_set>

void BBox::toPrint() const{
    std::cout << "Boudning Box: " << m_minBounds <<", " << m_maxBounds << std::endl;
}
Vector3 BBox::getMax() const
{
    return m_maxBounds;
}
Vector3 BBox::getMin() const
{
    return m_minBounds;
}
void BBox::updateBBox(const Vector3& vert) {
    m_minBounds.x = min(m_minBounds.x, vert.x);
    m_minBounds.y = min(m_minBounds.y, vert.y);
    m_minBounds.z = min(m_minBounds.z, vert.z);
    m_maxBounds.x = max(m_maxBounds.x, vert.x);
    m_maxBounds.y = max(m_maxBounds.y, vert.y);
    m_maxBounds.z = max(m_maxBounds.z, vert.z);
}
void BBox::updateBBox(const BBox& box)
{
    updateBBox(box.m_maxBounds);
    updateBBox(box.m_minBounds);
}
bool BBox::bboxCollide(const BBox& bbox) const{
    return BBox::bboxCollide(*this, bbox);
}
bool BBox::bboxCollide(const BBox& bbox1, const BBox& bbox2) {
    // Check for overlap along the x-axis
    bool xOverlap = (bbox1.m_minBounds.x <= bbox2.m_maxBounds.x) && (bbox1.m_maxBounds.x >= bbox2.m_minBounds.x);

    // Check for overlap along the y-axis
    bool yOverlap = (bbox1.m_minBounds.y <= bbox2.m_maxBounds.y) && (bbox1.m_maxBounds.y >= bbox2.m_minBounds.y);

    // Check for overlap along the z-axis
    bool zOverlap = (bbox1.m_minBounds.z <= bbox2.m_maxBounds.z) && (bbox1.m_maxBounds.z >= bbox2.m_minBounds.z);

    // Bounding boxes collide if they overlap along all three axes
    return xOverlap && yOverlap && zOverlap;
}
BBox BBox::transformBBox(const Matrix4& tmat) const {
    BBox res;
    res.updateBBox((tmat * Vector4::extendOne(m_maxBounds)).toVector3());
    res.updateBBox((tmat * Vector4::extendOne(Vector3(m_minBounds.x, m_minBounds.y, m_maxBounds.z))).toVector3());
    res.updateBBox((tmat * Vector4::extendOne(Vector3(m_minBounds.x, m_maxBounds.y, m_minBounds.z))).toVector3());
    res.updateBBox((tmat * Vector4::extendOne(Vector3(m_minBounds.x, m_maxBounds.y, m_maxBounds.z))).toVector3());
    res.updateBBox((tmat * Vector4::extendOne(Vector3(m_maxBounds.x, m_minBounds.y, m_minBounds.z))).toVector3());
    res.updateBBox((tmat * Vector4::extendOne(Vector3(m_maxBounds.x, m_minBounds.y, m_maxBounds.z))).toVector3());
    res.updateBBox((tmat * Vector4::extendOne(Vector3(m_maxBounds.x, m_maxBounds.y, m_minBounds.z))).toVector3());
    res.updateBBox((tmat * Vector4::extendOne(m_maxBounds)).toVector3());
    return res;
}
std::vector<Line> BBox::getLinesOfBbox(const ColorGC& bBoxColor) const
{
    Vector3 corners[8] = {
       {m_minBounds.x, m_minBounds.y, m_minBounds.z},
       {m_minBounds.x, m_minBounds.y, m_maxBounds.z},
       {m_minBounds.x, m_maxBounds.y, m_minBounds.z},
       {m_minBounds.x, m_maxBounds.y, m_maxBounds.z},
       {m_maxBounds.x, m_minBounds.y, m_minBounds.z},
       {m_maxBounds.x, m_minBounds.y, m_maxBounds.z},
       {m_maxBounds.x, m_maxBounds.y, m_minBounds.z},
       {m_maxBounds.x, m_maxBounds.y, m_maxBounds.z}
    };
    std::vector<Line> lines = {
        // Bottom face
        {corners[0], corners[1],bBoxColor},
        {corners[1], corners[5],bBoxColor},
        {corners[5], corners[4],bBoxColor},
        {corners[4], corners[0],bBoxColor},

        // Top face
        {corners[2], corners[3],bBoxColor},
        {corners[3], corners[7],bBoxColor},
        {corners[7], corners[6],bBoxColor},
        {corners[6], corners[2],bBoxColor},

        // Vertical edges
        {corners[0], corners[2],bBoxColor},
        {corners[1], corners[3],bBoxColor},
        {corners[4], corners[6],bBoxColor},
        {corners[5], corners[7],bBoxColor}
    };
    return lines;

}

// Update min and max bounds
void PolygonGC::updateBounds(const Vertex& vert) {
    m_bbox.updateBBox(vert.loc());
}
bool PolygonGC::isClippedByBBox(const Matrix4& tMat) const {
    return !BBox::bboxCollide(getBbox().transformBBox(tMat), BBox::unitBBox());
}
// reset min and max bounds
void PolygonGC::resetBounds() {
    if (m_vertices.empty()) {
        m_bbox = BBox();
        return;
    }
    m_vertHaveDataNormal = true;
    m_bbox = BBox(Vector3(FLT_MAX, FLT_MAX, FLT_MAX), Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX));
    for (const auto& vertex : m_vertices) {
        m_bbox.updateBBox(vertex->loc());
        if (!vertex->hasDataNormalLine())
            m_vertHaveDataNormal = false;
    }
}

// Constructor with a default color
PolygonGC::PolygonGC(ColorGC color) :m_visible(true), m_primeColor(color), m_hasDataNormal(false), m_vertHaveDataNormal(true){
    resetBounds();
}

void PolygonGC::setCalcAndDataNormalLines( Vector3 dataNormal)
{
    Vector3 centerPoint(0, 0, 0);
    for (const auto& v : m_vertices)
    {
        centerPoint += v->loc();
    }
    centerPoint /= m_vertices.size();
    m_calcNormalLine= Line(centerPoint, centerPoint + (calculateNormal().normalized() * 0.25), m_primeColor);
    m_dataNormalLine = Line(centerPoint, centerPoint + (dataNormal.normalized() * 0.25), m_primeColor);
}

void PolygonGC::setCalcNormalLines()
{
    Vector3 centerPoint(0, 0, 0);
    for (const auto& v : m_vertices)
    {
        centerPoint += v->loc();
    }
    centerPoint /= m_vertices.size();
    m_calcNormalLine = Line(centerPoint, centerPoint + (calculateNormal().normalized() * 0.25));
    m_hasDataNormal = false;
}

Vector3 PolygonGC::getCalcNormalNormolized()
{
    return (m_calcNormalLine.m_b - m_calcNormalLine.m_a).normalized();
}

Vector3 PolygonGC::getDataNormalNormolized()
{
    if (!hasDataNormalLine())
    {
        throw std::exception();
    }
    return (m_dataNormalLine.m_b - m_dataNormalLine.m_a).normalized();;
}




// Set the color of the polygon
void PolygonGC::setColor(const ColorGC& newColor) {   
    m_primeColor = newColor;
}
void PolygonGC::setSceneColor(const ColorGC& newColor) {
    
    m_sceneColor = newColor;
}


// Get the primal color of the polygon
ColorGC PolygonGC::getColor() const
{
    return m_primeColor;
}
// Get sceneal color
ColorGC PolygonGC::getSceneColor() const
{
    return m_sceneColor;
}
void PolygonGC::addVertex(std::shared_ptr<Vertex> vertex) {
    if (vertex)
    {
        m_vertices.push_back(vertex);      
        if (!vertex->hasDataNormalLine())
            m_vertHaveDataNormal = false;
        updateBounds(*vertex);
    }
}

// Get the number of vertices
size_t PolygonGC::vertexCount() const{
    return m_vertices.size();
}


// Print all vertices
void PolygonGC::printVertices() const{

    for (size_t i = 0; i < m_vertices.size(); ++i) {
        std::cout << "                  Vertex[" << i << "]: ";
        m_vertices.at(i)->print();
        std::cout << "\n";
    }
}

// Print bounds
void PolygonGC::printBounds() const{
    m_bbox.toPrint();
}

bool PolygonGC::isBehindCamera() const{
    for (size_t i = 0; i < m_vertices.size(); ++i)
        if (m_vertices[i]->loc().z > 0)
            return false;
    return true;
}
// Print polygon color
void PolygonGC::printColor() const{
    std::cout << m_primeColor.toHex();   
}


bool checkPointRelativeToPlane(const Vector3& planeLoc, const Vector3& point,bool isNagative) {
    if (isNagative)
    {
        if (planeLoc.x != 0) {
            return (point.x >= -1) ? true : false;
        }
        else if (planeLoc.y != 0) {
            return (point.y >= -1) ? true : false;
        }
        else if (planeLoc.z != 0) {
            return (point.z >= -1) ? true : false;
        }
    }
    else
    {
        if (planeLoc.x != 0) {
            return (point.x <= 1) ? true : false;
        }
        else if (planeLoc.y != 0) {
            return (point.y <= 1) ? true : false;
        }
        else if (planeLoc.z != 0) {
            return (point.z <= 1) ? true : false;
        }
    }
    throw;
}
void PolygonGC::clip()
{
    std::vector<std::pair<Vector3,bool >> plansVector;
    plansVector.push_back({Vector3(1,0,0),false });
    plansVector.push_back({ Vector3(0,1,0),false });
    plansVector.push_back({ Vector3(0,0,1),false });
    plansVector.push_back({ Vector3(-1,0,0),true });
    plansVector.push_back({ Vector3(0,-1,0),true });
    plansVector.push_back({ Vector3(0,0,-1),true });
    for(auto& planPair:plansVector)
    {
        std::vector<std::shared_ptr<Vertex> > inscopeVertices;
        for (size_t i = 0; i < m_vertices.size(); ++i) {
            std::shared_ptr<Vertex> v1 = m_vertices[i];
            std::shared_ptr<Vertex> v2 = m_vertices[(i + 1) % m_vertices.size()];

            // Check if vertices are inside the clipping volume
            //TODO low preformence func VVV maybe you can do it better 
            bool v1Inside = checkPointRelativeToPlane(planPair.first,v1->loc(),planPair.second);
            bool v2Inside = checkPointRelativeToPlane(planPair.first, v2->loc(), planPair.second);
          
            if (v1Inside && v2Inside) {
                // Both vertices are inside, add v2 to the clipped vertices
                inscopeVertices.push_back(v2);
            }    
            else if (v1Inside && !v2Inside) {
                //// v1 is inside, v2 is outside, add intersection point 
                std::shared_ptr<Vertex> tempVert(Vertex::intersectionVertexesWithPlan(v1, v2, planPair.first));
                inscopeVertices.push_back(tempVert);
            }
            else if (!v1Inside && v2Inside) {
                //// v1 is outside, v2 is inside, add intersection point and v2
                std::shared_ptr<Vertex> tempVert(Vertex::intersectionVertexesWithPlan(v1, v2, planPair.first));
                inscopeVertices.push_back(tempVert);
                inscopeVertices.push_back(v2);              
            }           
        }
        m_vertices = inscopeVertices;
    }
    this->m_calcNormalLine.clip();
    if (this->m_hasDataNormal)
    {
        this->m_dataNormalLine.clip();
    }
    this->resetBounds();
}
// Function to apply a transformation matrix to all vertices
std::shared_ptr<PolygonGC> PolygonGC::applyTransformation(const Matrix4& transformation, bool flipNormals) const
{
    std::shared_ptr<PolygonGC> newPoly = std::make_shared<PolygonGC>(this->m_primeColor);
    for (const auto& vertex : m_vertices) {
        newPoly->addVertex(vertex->getTransformedVertex(transformation, flipNormals));
    }
    newPoly->m_calcNormalLine = this->m_calcNormalLine.getTransformedLine(transformation);
    if (this->m_hasDataNormal)
    {
        newPoly->m_dataNormalLine = this->m_dataNormalLine.getTransformedLine(transformation);
        newPoly->m_hasDataNormal = true;
    }
    if (flipNormals)
    {
        newPoly->flipNormals();
    }
    return newPoly;
}
std::shared_ptr<PolygonGC> PolygonGC::applyTransformationAndFillMap(const Matrix4& transformation, bool flipNormals,
    std::unordered_map<Vector3, std::shared_ptr<Vertex>, VectorKeyHash, VectorKeyEqual> &map) const
{
    std::shared_ptr<PolygonGC> newPoly = std::make_shared<PolygonGC>(this->m_primeColor);
    for (const auto& vertex : m_vertices) {
        std::shared_ptr<Vertex> tempVer = vertex->getTransformedVertex(transformation, flipNormals);
        map[tempVer->loc()] = tempVer;
        newPoly->addVertex(tempVer);
      
    }
    newPoly->m_calcNormalLine = this->m_calcNormalLine.getTransformedLine(transformation);
    if (this->m_hasDataNormal)
    {
        newPoly->m_dataNormalLine = this->m_dataNormalLine.getTransformedLine(transformation);
        newPoly->m_hasDataNormal = true;
    }
    if (flipNormals)
    {
        newPoly->flipNormals();
    }

    return newPoly;
}
static bool ifEdgeBBOXCutUnitCube(const Vertex& v1, const Vertex& v2) {
    BBox b;
    b.updateBBox(v1.loc());
    b.updateBBox(v2.loc());
    BBox unit = BBox::unitBBox();
    return BBox::bboxCollide(b, unit);
}

void PolygonGC::loadSilhoutteToContainer(std::unordered_map<Line, EdgeMode, LineKeyHash, LineKeyEqual>& SilhoutteMap) const
{
    for (size_t i = 0; i < m_vertices.size(); ++i) {
        std::shared_ptr<Vertex> v1 = m_vertices[i];
        std::shared_ptr<Vertex> v2 = m_vertices[(i + 1) % m_vertices.size()];
        if (ifEdgeBBOXCutUnitCube(*v1, *v2))
        {
            EdgeMode tempEdgeMode;
            if (this->isVisible())
            {
                tempEdgeMode = EdgeMode::VISIBLE;
            }
            else
            {
                tempEdgeMode = EdgeMode::NO_VISIBLE;
            }
            Line temp(v1->loc(), v2->loc());
            if (SilhoutteMap.find(temp) == SilhoutteMap.end()) {

                SilhoutteMap.insert({ temp,tempEdgeMode });
            }
            else
            {
                if (SilhoutteMap[temp] != tempEdgeMode)
                {
                    SilhoutteMap[temp] = EdgeMode::SILHOUTTE;
                }
            }
        }
    }
}


void PolygonGC::loadVertexEdgesToContainer(std::vector<std::pair<std::shared_ptr<Vertex>, std::shared_ptr<Vertex>>>& container, const ColorGC* overridingColor) const {
    ColorGC line_color = overridingColor == nullptr ? m_primeColor : *overridingColor;
    for (size_t i = 0; i < m_vertices.size(); ++i) {
        std::shared_ptr<Vertex> v1 = m_vertices[i];
        std::shared_ptr<Vertex> v2 = m_vertices[(i + 1) % m_vertices.size()];
        if (ifEdgeBBOXCutUnitCube(*v1, *v2))
        {
            container.push_back(std::pair<std::shared_ptr<Vertex>, std::shared_ptr<Vertex>>(v1, v2));
        }
    }
}
void PolygonGC::loadEdgesToContainer(std::vector<Line>& container, const ColorGC* overridingColor) const {
    ColorGC line_color = overridingColor == nullptr ? m_primeColor : *overridingColor;
    for (size_t i = 0; i < m_vertices.size(); ++i) {
        std::shared_ptr<Vertex> v1 = m_vertices[i];
        std::shared_ptr<Vertex> v2 = m_vertices[(i + 1) % m_vertices.size()];
        if(ifEdgeBBOXCutUnitCube(*v1, *v2))
        {            
            container.push_back(Line(v1->loc(), v2->loc(), line_color));
        }
    }
}

void PolygonGC::loadLines(std::vector<Line> lines[LineVectorIndex::LAST], RenderMode& renderMode, std::unordered_map<Line, EdgeMode, LineKeyHash, LineKeyEqual>& SilhoutteMap) const {
    const ColorGC* wfClrOverride = renderMode.getRenderOverrideWireColorFlag() ? &renderMode.getWireColor() : &this->m_primeColor;
    const ColorGC* nrmClrOverride = renderMode.getRenderOverrideNormalColorFlag() ? &renderMode.getNormalColor() : &this->m_primeColor;
    if (renderMode.getSilohetteFlag()) loadSilhoutteToContainer(SilhoutteMap);
    if (!renderMode.getRenderCulledFlag() || renderMode.getRenderCulledFlag() && this->isVisible()) {
        if (renderMode.getWireFrameFlag()) loadEdgesToContainer(lines[LineVectorIndex::SHAPES], wfClrOverride);
        if (renderMode.getVertexShowDNormalFlag())
        {
            try {
                loadVertNLinesFromData(lines[LineVectorIndex::VERTICES_DATA_NORMAL], nrmClrOverride);
            }
            catch (...) {
                renderMode.setRenderDataVertivesNormalFlag();
                lines[LineVectorIndex::VERTICES_DATA_NORMAL].clear();
                if (!renderMode.getVertexShowCNormalFlag())
                    renderMode.setRenderCalcVertivesNormalFlag();
                AfxMessageBox(_T("This Object wasnt provided with vertice normals!"));
            }
        }
        if (renderMode.getVertexShowCNormalFlag()) loadVertNLinesFromCalc(lines[LineVectorIndex::VERTICES_CALC_NORMAL], nrmClrOverride);
        if (renderMode.getRenderPolygonsBboxFlag()) loadBboxLinesToContainer(lines[LineVectorIndex::POLY_BBOX], wfClrOverride);
        if (renderMode.getPolygonsShowDNormalFlag()) {
            try {
                lines[LineVectorIndex::POLY_DATA_NORNAL].push_back(getDataNormalLine(nrmClrOverride));
            }
            catch (...) {
                renderMode.setRenderPolygonsNormalFromDataFlag();
                lines[LineVectorIndex::POLY_DATA_NORNAL].clear();
                if (!renderMode.getPolygonsShowCNormalFlag())
                    renderMode.setRenderPolygonsCalcNormalFlag();
                AfxMessageBox(_T("This Object wasnt provided with polygon normals!"));
            }
        }
        if (renderMode.getPolygonsShowCNormalFlag()) lines[LineVectorIndex::POLY_CALC_NORNAL].push_back(getCalcNormalLine(nrmClrOverride));
    }
}


inline int transformToScreenSpace(float p,int halfSize)
{
    return static_cast<int>(std::round((p * halfSize) + halfSize));
}
int findIntersectionAndFitToScreen(std::pair<std::shared_ptr<Vertex>, std::shared_ptr<Vertex>>& vertexPair, float y, int halfWidth, int halfhight, float& t) {

    int firstY = transformToScreenSpace(vertexPair.first->loc().y , halfhight);
    float secondY = transformToScreenSpace(vertexPair.second->loc().y ,halfhight);
   
    int diffA = firstY - y;
    int epsilon = 2;
    if ((firstY == secondY) && (diffA < epsilon) && (diffA > -epsilon))
        return 2; //horizontal line
    if ((firstY - y) * (secondY - y) > 0) {
        return 0; // No intersection
    }
    if ((secondY - firstY) == 0) {
        return 0; // No intersection
    }
    t = (y - firstY) / (secondY - firstY);
    //vec = (line.m_a * (1 - t)) + line.m_b * t;
    return 1;
}

// Function to sort edges by their y-coordinates
bool compareEdgesByY(const std::pair<std::shared_ptr<Vertex>, std::shared_ptr<Vertex>>& a, const std::pair<std::shared_ptr<Vertex>, std::shared_ptr<Vertex>>& b) {
    return min(a.first->loc().y, a.second->loc().y) < min(b.first->loc().y, b.second->loc().y);
}

bool updateActiveEdges(int y, float halfhight, std::vector<std::pair<std::shared_ptr<Vertex>, std::shared_ptr<Vertex>>>& lineVector, std::vector<std::pair<std::shared_ptr<Vertex>, std::shared_ptr<Vertex>>>::iterator& currentLine, std::unordered_set<std::pair<std::shared_ptr<Vertex>, std::shared_ptr<Vertex>>*>& activeEdges) {
    bool res = true; 
    while (currentLine != lineVector.end() && min(convertClipToScreen(currentLine->first->loc().y, halfhight), convertClipToScreen(currentLine->second->loc().y, halfhight)) == y) {
        activeEdges.insert(&(*currentLine));
        ++currentLine;
        res = false;
    }
    for (auto it = activeEdges.begin(); it != activeEdges.end();) {
        if (max(convertClipToScreen((*it)->first->loc().y, halfhight), convertClipToScreen((*it)->second->loc().y, halfhight)) < y) {
            it = activeEdges.erase(it);
            res = false;
        }
        else {
            ++it;
        }
    }
    return res;
}
void updateLineVertexes(std::pair<std::shared_ptr<Vertex>, std::shared_ptr<Vertex>>* edge, int y, float halfWidth, float halfhight, Vertex& smallestVecX, Vertex& biggestVecX, std::pair<std::shared_ptr<Vertex>, std::shared_ptr<Vertex>>*& smallestEdge, std::pair<std::shared_ptr<Vertex>, std::shared_ptr<Vertex>>*& biggestEdge, float& t_xmin ,float& t_xmax) {
    float t1 = 0;
    int result = findIntersectionAndFitToScreen(*edge, y, halfWidth, halfhight, t1);
    if (result == 1) {
        Vector3 interVertex = Vertex::interpolate_loc(*edge->first, *edge->second, t1);
        if (interVertex.x < smallestVecX.loc().x) {
            smallestVecX.setLoc(interVertex);
            smallestEdge = edge;
            t_xmin = t1;
        }
        if (interVertex.x > biggestVecX.loc().x) {
            biggestVecX.setLoc(interVertex);
            biggestEdge = edge;
            t_xmax = t1;
        }
    }
    else if (result == 2) {
        if (edge->first->loc().x < smallestVecX.loc().x) {
            smallestVecX = *edge->first;
            smallestEdge = edge;
            t_xmin = 0;
        }
        if (edge->first->loc().x > biggestVecX.loc().x) {
            biggestVecX = *edge->first;
            biggestEdge = edge;
            t_xmax = 1;
        }
        if (edge->second->loc().x < smallestVecX.loc().x) {
            smallestVecX = *edge->second;
            smallestEdge = edge;
            t_xmin = 1;
        }
        if (edge->second->loc().x > biggestVecX.loc().x) {
            biggestVecX = *edge->second;
            biggestEdge = edge;
            t_xmin = 0;
        }
    }
}


void PolygonGC::fillGbuffer(GBuffer& gBuffer, const RenderMode& rm) const
{
    const int width = gBuffer.getWidth(); 
    const int hight = gBuffer.getHeight();
    const int halfWidth = width / 2;
    const int halfhight = hight / 2;
    const int yMax = convertClipToScreen(m_bbox.getMax().y , halfhight);
    const int yMin = convertClipToScreen(m_bbox.getMin().y, halfhight);
    std::pair<std::shared_ptr<Vertex>, std::shared_ptr<Vertex>>* xMaxEdge = nullptr;
    std::pair<std::shared_ptr<Vertex>, std::shared_ptr<Vertex>>* xMinEdge = nullptr;

    std::vector<std::pair<std::shared_ptr<Vertex>, std::shared_ptr<Vertex>>> lineVector;
    this->loadVertexEdgesToContainer(lineVector, nullptr);
    std::sort(lineVector.begin(), lineVector.end(), compareEdgesByY);

    std::unordered_set<std::pair<std::shared_ptr<Vertex>, std::shared_ptr<Vertex>>*> activeEdges;
    auto currentLine = lineVector.begin();
    int smallX, bigX;
    for (int y = yMin; y < yMax; ++y) {
        // Update active edges
        updateActiveEdges(y, halfhight, lineVector, currentLine, activeEdges);
        //  active edges
        Vector3 loc, dir, loc_delta;
        ColorGC color;
        float t_xmin = 0, t_xmax = 0;
        Vertex smallestVecX = Vertex(Vector3(FLT_MAX, FLT_MAX, FLT_MAX));
        Vertex biggestVecX = Vertex(Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX));
        for (const auto& edge : activeEdges)
            updateLineVertexes(edge, y, halfWidth, halfhight, smallestVecX, biggestVecX, xMinEdge, xMaxEdge, t_xmin, t_xmax);
        if (smallestVecX.loc() == Vector3(FLT_MAX, FLT_MAX, FLT_MAX) || biggestVecX.loc() == Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX))
            continue;
        smallestVecX.setFromInterpolation(*xMinEdge->first, *xMinEdge->second, t_xmin, false, rm.getRenderShadeGouroudFlag(), rm.getRenderShadePhongFlag() && rm.getPolygonsUseCNormalFlag(), rm.getRenderShadePhongFlag() && rm.getRenderDynemic());
        biggestVecX.setFromInterpolation(*xMaxEdge->first, *xMaxEdge->second, t_xmax, false, rm.getRenderShadeGouroudFlag(), rm.getRenderShadePhongFlag() && rm.getPolygonsUseCNormalFlag(), rm.getRenderShadePhongFlag() && rm.getRenderDynemic());
        smallX = convertClipToScreen(smallestVecX.loc().x, halfWidth);
        bigX = convertClipToScreen(biggestVecX.loc().x, halfWidth); 
        loc_delta = Vertex::calculate_delta_location(smallestVecX, biggestVecX, bigX - smallX);
        for (int x = smallX; x < bigX; x++)
        {
            float t2 = (float)(x - smallX) / (bigX - smallX);
            if (rm.getRenderShadePhongFlag()) {
                Line tmp = Line();
                if (!rm.getPolygonsUseCNormalFlag())
                    tmp = Vertex::interpolate_dnormal(smallestVecX, biggestVecX, t2);
                else
                    tmp = Vertex::interpolate_cnormal(smallestVecX, biggestVecX, t2);
                loc = tmp.m_a;
                dir = tmp.direction();
            }
            else
                loc = smallestVecX.loc() + loc_delta * (x - smallX);
            color = rm.getRenderShadeGouroudFlag() ? Vertex::interpolate_color(smallestVecX, biggestVecX, t2) : color;
            gBuffer.push(x, y, GData(this, color, loc, dir));
        }
    }
}

void PolygonGC::fillBasicSceneColors(const Shader& shader,const RenderMode& rm)
{
    if (rm.getVertexUseCNormalFlag())
        for (auto& vert : m_vertices)
           vert->setColor(shader.calcLightColorAtPos(vert->loc(), vert->getCalcNormalLine().direction(), this->getColor()));
    else
        for (auto& vert : m_vertices)
           vert->setColor(shader.calcLightColorAtPos(vert->loc(), vert->getDataNormalLine().direction(), this->getColor()));
    rm.getPolygonsUseCNormalFlag() ?
            setSceneColor(shader.calcLightColorAtPos(m_calcNormalLine.m_a, m_calcNormalLine.direction(), this->getColor())) :
            setSceneColor(shader.calcLightColorAtPos(m_dataNormalLine.m_a, m_dataNormalLine.direction(), this->getColor()));
}

void PolygonGC::flipNormals()
{    
    this->m_calcNormalLine.flipLine();
    if (m_hasDataNormal)
    {
        m_dataNormalLine.flipLine();
    }
}

// get polygon Bbox
BBox PolygonGC::getBbox() const{
    return m_bbox;
}

Line PolygonGC::getCalcNormalLine(const ColorGC* overridingColor) const
{
    return Line(m_calcNormalLine.m_a, m_calcNormalLine.m_b, overridingColor == nullptr ? m_calcNormalLine.m_color : *overridingColor);

}

Line PolygonGC::getDataNormalLine(const ColorGC* overridingColor) const
{
    if (!hasDataNormalLine())
    {
        throw std::exception();
    }

    return Line(m_dataNormalLine.m_a, m_dataNormalLine.m_b, overridingColor == nullptr ? m_dataNormalLine.m_color : *overridingColor);
}

bool PolygonGC::hasDataNormalLine() const{
    return m_hasDataNormal;
}
bool PolygonGC::hasVertsDataNormalLine()const {
    return m_vertHaveDataNormal;
}
void PolygonGC::setAlpha(uint8_t alpha)
{
    this->m_primeColor.setAlpha(alpha);
    this->m_sceneColor.setAlpha(alpha);
    for (auto& ver : this->m_vertices)
    {
        ColorGC tmp=ver->getColor();
        tmp.setAlpha(alpha);
        ver->setColor(tmp);
    }
}
void PolygonGC::setVisibility(bool isVisible)
{
    m_visible = isVisible;
}

bool PolygonGC::isVisible() const
{
    return m_visible;
}


void PolygonGC::loadVertNLinesFromData(std::vector<Line>& container, const ColorGC* overridingColor)const {
    for (const auto& vert : m_vertices) {
        try {
            container.push_back(vert->getDataNormalLine());
        }
        catch (...) {
            container.clear();
            throw std::exception();
        }
    }
}

void PolygonGC::loadVertNLinesFromCalc(std::vector<Line>&container, const ColorGC* overridingColor) const {
    for (const auto& vert : m_vertices) {
        container.push_back(vert->getCalcNormalLine());
    }
}

void PolygonGC::loadBboxLinesToContainer(std::vector<Line>& container, const ColorGC* overridingColor) const
{
    std::vector<Line> bboxLines = m_bbox.getLinesOfBbox(overridingColor == nullptr ? m_primeColor : *overridingColor);
    container.insert(container.end(), bboxLines.begin(), bboxLines.end());
}
Vector3 PolygonGC::calculateNormal() const {
    if (m_vertices.size() < 3)
    {
        throw std::runtime_error("whaht the hell just happend?is it polygon with less then 2 vertices???hemmm?");
    }
    const Vector3 vec1 = m_vertices.at(1)->loc() - m_vertices.at(0)->loc();
    const Vector3 vec2 = m_vertices.at(2)->loc() - m_vertices.at(1)->loc();
    return Vector3::cross(vec1,vec2).normalized();

}