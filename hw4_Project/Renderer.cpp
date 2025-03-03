#include "Renderer.h"
#include <cstring> // For memset
#include <iostream>
#include <algorithm> // For std::sort
#include <chrono>

Renderer::Renderer():m_Buffer(nullptr),
m_BgBuffer(nullptr),
m_shader(),
m_width(0),
m_height(0),
m_bgInfo({ BG_MODE::SOLID,"",ColorGC(230,230,230) }) {
}

Renderer::~Renderer() {
    clear(true);
}
void Renderer::drawWireFrame(std::vector<Line> lines[LineVectorIndex::LAST], GBuffer& gBuff)
{
   
    for (int i = LineVectorIndex::SHAPES; i < LineVectorIndex::LAST; i++) {        
        for (Line& line : lines[i]) {
            if (line.clip())
            {   
                line.draw(gBuff, m_width, m_height);
            }
        }
    }
}

void Renderer::drawSilhoutteEdges(const std::unordered_map<Line, EdgeMode, LineKeyHash, LineKeyEqual>& SilhoutteMap, GBuffer& gBuff)
{    
    for (auto& pair : SilhoutteMap)
    {
        if (pair.second == EdgeMode::SILHOUTTE)
        {
            pair.first.drawSilhoutte(gBuff, this->m_width, this->m_height);
        }
    }
}

void Renderer::invalidateBG(const RenderMode& bg_info) {
    m_bgInfo.color = bg_info.getBGColor();
    m_bgInfo.mode = bg_info.getRenderBGFlag();
    strcpy(m_bgInfo.pngPath, bg_info.getBGPngPath());
    delete[] m_BgBuffer;
    m_BgBuffer = (uint32_t*)malloc(sizeof(uint32_t) * m_width * m_height);
    if (m_bgInfo.mode == BG_MODE::SOLID)
        fillColorBG();
    else
        fillPngBG();
}
bool Renderer::isvalidBG(const RenderMode& bg_info) {
    if (bg_info.getRenderBGFlag() != m_bgInfo.mode)
        return false;
    if (strcmp(bg_info.getBGPngPath(), m_bgInfo.pngPath))
        return false;
    if (bg_info.getRenderBGSolidFlag() && bg_info.getBGColor().getARGB() != m_bgInfo.color.getARGB())
        return false;
    return true;
}
void Renderer::invalidate(const RenderMode& bg_info, bool force) {
    createBuffers();
    if (force || !isvalidBG(bg_info)) {
        invalidateBG(bg_info);
    }
}
void Renderer::updateLighting(LightParams lights[MAX_LIGHT], LightParams ambient, int sceneSpecExp) {
    m_shader.updateLighting(lights , ambient, sceneSpecExp);
}

void Renderer::render(const Camera* camera, int width, int height, const std::vector<Model*>& models, RenderMode& renderMode){

    bool forceAll = false;
    if (getWidth() != width || getHeight() != height) {
        setWidth(width); setHeight(height);
        forceAll = true;
    }
    invalidate(renderMode, forceAll);
    memcpy(m_Buffer, m_BgBuffer, sizeof(uint32_t) * m_width * m_height);
    GBuffer gBuffer(m_width, m_height);
    Matrix4 aspectRatioMatrix = Matrix4::scaling(Vector3(1.0f / (width / height), 1.0f, 1.0f));
    const Matrix4 projectionAspectMatrix = aspectRatioMatrix * camera->getProjectionMatrix();
    const Matrix4 viewProjectionMatrix = projectionAspectMatrix  * camera->getViewMatrix();
    const Matrix4 projectionAspectMatrix_inv = projectionAspectMatrix.irit_inverse();
    // Transform and cull geometry
    std::vector<std::unique_ptr<Geometry>> transformedGeometries;
    Vector3 cameraPos = camera->getViewMatrix().irit_inverse().getCol(3);
    m_shader.setView(cameraPos, projectionAspectMatrix_inv, camera->isPerspective());

    std::vector<Line> lines[LineVectorIndex::LAST];
    Line x_axis = Line(Vector3(-1, 0, 0), Vector3(1, 0, 0), ColorGC(255, 0, 0, 255));
    Line y_axis = Line(Vector3(0, -1, 0), Vector3(0, 1, 0), ColorGC(0, 255, 0, 255));
    Line z_axis = Line(Vector3(0, 0, -1), Vector3(0, 0, 1), ColorGC(0, 0, 255, 255));

    lines[0].push_back(x_axis.getTransformedLine(viewProjectionMatrix));
    lines[0].push_back(y_axis.getTransformedLine(viewProjectionMatrix));
    lines[0].push_back(z_axis.getTransformedLine(viewProjectionMatrix));
    this->drawWireFrame(lines, gBuffer);
    for (const auto& model : models) {
        std::vector<Line> lines[LineVectorIndex::LAST];
        std::unordered_map<Line, EdgeMode, LineKeyHash, LineKeyEqual> SilhoutteMap;
        std::unique_ptr<Geometry> transformedGeometry = model->applyTransformation(viewProjectionMatrix, renderMode.getRenderWithFlipedNormalsFlag());
        if (transformedGeometry) {
            transformedGeometry->clip();      
            transformedGeometry->backFaceCulling(camera->isPerspective(), projectionAspectMatrix_inv);
            transformedGeometry->fillBasicSceneColors(m_shader,renderMode);
            transformedGeometry->loadLines(lines, renderMode, SilhoutteMap);
            this->drawWireFrame(lines, gBuffer);
            if(!renderMode.getRenderShadeNoneFlag()) transformedGeometry->fillGbuffer(gBuffer , renderMode);
            if (renderMode.getSilohetteFlag()) this->drawSilhoutteEdges(SilhoutteMap, gBuffer);
            transformedGeometries.push_back(std::move(transformedGeometry));
        }
    }
    m_shader.applyShading(m_Buffer, gBuffer, renderMode);
}

void Renderer::clear(bool clearBg) {
    delete[] m_Buffer;
    m_Buffer = nullptr;
    if(clearBg){
        delete[] m_BgBuffer;
        m_BgBuffer = nullptr;
    }
}

uint32_t* Renderer::getBuffer() const{
    return m_Buffer;
}

void Renderer::createBuffers() {
    clear(false);
    m_Buffer = new uint32_t[m_width * m_height]; // RGB buffer
}
void Renderer::fillColorBG() {
    for (int i = 0; i < m_width; i++)
        for (int j = 0; j < m_height; j++)
            m_BgBuffer[i + j*m_width] = m_bgInfo.color.getARGB();
}

void Renderer::fillPngBG() {
    //TODO fix png run down of tranperncy
    PngWrapper bgImage(m_bgInfo.pngPath, m_width, m_height);
    if (!bgImage.ReadPng())
    {
        throw;
    }
    int imgHeight = bgImage.GetHeight();
    int imgWidth = bgImage.GetWidth();
    if (m_bgInfo.mode == BG_MODE::REPEATED)
    {
        for (int x = 0; x < m_width; x++)
            for (int y = 0; y < m_height; y++)
            {
                int tempRgbaColor = bgImage.GetValue(x % imgWidth, y % imgHeight);
                tempRgbaColor = (tempRgbaColor >> 8) | (tempRgbaColor << (24));
                m_BgBuffer[x + (y * m_width)] = tempRgbaColor;

            }
    }
    else if (m_bgInfo.mode == BG_MODE::STREACHED)
    {
        float xScale = static_cast<float>(imgWidth) / m_width;
        float yScale = static_cast<float>(imgHeight) / m_height;
        for (int x = 0; x < m_width; x++)
            for (int y = 0; y < m_height; y++)
            {
                int srcX = static_cast<int>(x * xScale);
                int srcY = static_cast<int>(y * yScale);
                int tempRgbaColor = bgImage.GetValue(srcX, srcY);
                tempRgbaColor = (tempRgbaColor >> 8) | (tempRgbaColor << (24));
                m_BgBuffer[x + (y * m_width)] = tempRgbaColor;
            }
    }
}

void Renderer::setFog(const ColorGC& color, float intensity, bool enabled)
{
    this->m_shader.setFog(color, intensity, enabled);
}

ColorGC Renderer::getFogColor() const
{
    return this->m_shader.getFogColor();
}
