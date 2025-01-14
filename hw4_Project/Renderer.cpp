#include "Renderer.h"
#include <cstring> // For memset
#include <iostream>
#include <algorithm> // For std::sort

Renderer::Renderer():m_Buffer(nullptr),
m_GBuffer(nullptr),
m_BgBuffer(nullptr),
m_shader(),
m_width(0),
m_height(0),
m_bgInfo({ BG_MODE::SOLID,"",ColorGC(230,230,230) }) {
}


Renderer::~Renderer() {
    clear(true);
}
void Renderer::drawWireFrame(std::vector<Line> lines[LineVectorIndex::LAST])
{
   
    for (int i = LineVectorIndex::SHAPES; i < LineVectorIndex::LAST; i++) {        
        for (Line& line : lines[i]) {
            if (line.clip())
            {   
                line.draw(m_Buffer, m_GBuffer, m_width, m_height);
            }
        }
    }
}

void Renderer::drawSilhoutteEdges(const std::unordered_map<Line, EdgeMode, LineKeyHash, LineKeyEqual>& SilhoutteMap)
{    
    for (auto& pair : SilhoutteMap)
    {
        if (pair.second == EdgeMode::SILHOUTTE)
        {
            pair.first.drawSilhoutte(m_Buffer, m_GBuffer, this->m_width, this->m_height);
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
    if(force || !isvalidBG(bg_info))
        invalidateBG(bg_info);
}
void Renderer::updateLighting(LightParams lights[MAX_LIGHT], LightParams ambient, int sceneSpecExp) {
    m_shader.updateLighting(lights , ambient, sceneSpecExp);
}

void Renderer::render(const Camera* camera, int width, int height, const std::vector<Model*> models, RenderMode& renderMode){

    bool forceAll = false;
    if (getWidth() != width || getHeight() != height) {
        setWidth(width); setHeight(height);
        forceAll = true;
    }
   
    invalidate(renderMode, forceAll);
   
    memcpy(m_Buffer, m_BgBuffer, sizeof(uint32_t) * m_width * m_height);

    Matrix4 aspectRatioMatrix = Matrix4::scaling(Vector3(1.0f / (width / height), 1.0f, 1.0f));
    const Matrix4 projectionAspectMatrix = aspectRatioMatrix * camera->getProjectionMatrix();
    const Matrix4 viewProjectionMatrix = projectionAspectMatrix  * camera->getViewMatrix();
    const Matrix4 projectionAspectMatrix_inv = projectionAspectMatrix.irit_inverse();
    if (renderMode.getRenderAddKeyFrame()) {
        //m_keyFrames.push_back(std::pair<Matrix4, Vector3>(viewProjectionMatrix, camera->getLocation()));
        renderMode.setRenderAddKeyFrame();
    }
    // Transform and cull geometry
    std::vector<Geometry*> transformedGeometries;
    std::vector<Line> lines[LineVectorIndex::LAST];
    std::unordered_map<Line, EdgeMode, LineKeyHash, LineKeyEqual> SilhoutteMap;
    
    Vector3 cameraPos = camera->getViewMatrix().irit_inverse().getCol(3);
    m_shader.setView(cameraPos, projectionAspectMatrix_inv, camera->isPerspective());
    //m_shader.applyTransformationToLights(viewProjectionMatrix);
    for (const auto& model : models) {
        Geometry* transformedGeometry;
        transformedGeometry = model->applyTransformation(viewProjectionMatrix, renderMode.getRenderWithFlipedNormalsFlag());
        if (transformedGeometry) {
            transformedGeometry->clip();            
            transformedGeometry->backFaceCulling(camera->isPerspective(), projectionAspectMatrix_inv);
            transformedGeometry->fillBasicSceneColors(m_shader,renderMode);
            transformedGeometry->loadLines(lines, renderMode, SilhoutteMap);
            if(!renderMode.getRenderShadeNoneFlag()) transformedGeometry->fillGbuffer(m_GBuffer, m_width, m_height , renderMode);
            transformedGeometries.push_back(transformedGeometry);
        }
    }
    lines[LineVectorIndex::SHAPES].push_back(Line((viewProjectionMatrix * Vector4(-1, 0, 0, 1)).toVector3(), (viewProjectionMatrix * Vector4(1, 0, 0, 1)).toVector3(), ColorGC(255, 0, 0)));
    lines[LineVectorIndex::SHAPES].push_back(Line((viewProjectionMatrix * Vector4(0, -1, 0, 1)).toVector3(), (viewProjectionMatrix * Vector4(0, 1, 0, 1)).toVector3(), ColorGC(0, 255, 0)));
    lines[LineVectorIndex::SHAPES].push_back(Line((viewProjectionMatrix * Vector4(0, 0, -1, 1)).toVector3(), (viewProjectionMatrix * Vector4(0, 0, 1, 1)).toVector3(), ColorGC(0, 0, 255)));
    //for (int y = 0; y < m_height; y+=4)
    //    for (int x = 0; x < m_width; x+=8)
    //        if (m_GBuffer[y*m_width+x].polygon) {
    //            lines[LineVectorIndex::SHAPES].push_back(Line(m_GBuffer[y * m_width + x].pixPos, m_GBuffer[y * m_width + x].pixPos+ (viewProjectionMatrix * viewProjectionMatrix.transpose()* Vector4::extendOne(m_GBuffer[y * m_width + x].pixNorm)).toVector3()*0.25, ColorGC(255, 0, 255)));
    //     }
    
    //the Final draw

  //  m_shader.applyShading(m_Buffer, m_GBuffer, m_width, m_height);

    m_shader.applyShading(m_Buffer, m_GBuffer, m_width, m_height, renderMode);

    //this->drawSolid(m_shader);
    this->drawWireFrame(lines);

    if (renderMode.getSilohetteFlag()) this->drawSilhoutteEdges(SilhoutteMap);

    for (const auto& geo : transformedGeometries) {
        delete geo;
    }

}

void Renderer::clear(bool clearBg) {
    delete[] m_Buffer;
    m_Buffer = nullptr;
    delete[] m_GBuffer;
    m_GBuffer = nullptr;
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
    m_GBuffer = new gData[m_width * m_height]; // Z-buffer
    gData initGdataObj = { FLT_MAX, nullptr, 0, 0,0, nullptr};
    std::fill(m_GBuffer, m_GBuffer + (m_width * m_height), initGdataObj);
    std::memset(m_Buffer, 0, sizeof(uint32_t) * m_width * m_height);
}
void Renderer::fillColorBG() {
    for (int i = 0; i < m_width; i++)
        for (int j = 0; j < m_height; j++)
            m_BgBuffer[i + j*m_width] = m_bgInfo.color.getARGB();
}

void Renderer::fillPngBG() {
    PngWrapper bgImage(m_bgInfo.pngPath, m_width, m_height);
    if (!bgImage.ReadPng())
    {
        std::cout << "hereeeeeee123";
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
//void Renderer::setupInterpolator() {
//    interpolator.setOrder(m_keyTMats.size() - 1);
//}
//void Renderer::interpolateTmatAndCameraPos(Matrix4& tmat, Vector3& pos, double t) {
//    tmat = interpolator.interpolate(m_keyTMats, t);
//    pos = interpolator.interpolate(m_keyCameraPos, t);
//}

static std::vector<double> generateTimeVector(double movieLength, double frameRate) {
    std::vector<double> tValues;
    int totalFrames = static_cast<int>(movieLength * frameRate);
    tValues.reserve(totalFrames);
    for (int i = 0; i < totalFrames; ++i) {
        double t = static_cast<double>(i) / (totalFrames - 1); tValues.push_back(t); 
    } return tValues; 
}


void Renderer::generateMovie(double movieLength, double frameRate, const std::vector<Model*> models, RenderMode& renderMode) {
    Matrix4 tMat;
    Vector3 camPos;
    std::vector<double> timeVec = generateTimeVector(movieLength, frameRate);
    // Transform and cull geometry
    for (int i = 0; i < timeVec.size(); i++) {
        //interpolateTmatAndCameraPos(tMat, camPos, timeVec[i]);
        std::vector<Geometry*> transformedGeometries;
        //m_shader.setViewPos(camPos);

        for (const auto& model : models) {
            Geometry* transformedGeometry = model->applyTransformation(tMat, renderMode.getRenderWithFlipedNormalsFlag());
            if (transformedGeometry) {
                transformedGeometry->clip();
        //        transformedGeometry->backFaceCulling(camPos);
                transformedGeometry->fillBasicSceneColors(m_shader, renderMode);
                transformedGeometry->fillGbuffer(m_GBuffer, m_width, m_height, renderMode);
                transformedGeometries.push_back(transformedGeometry);
            }
        }

        memcpy(m_Buffer, m_BgBuffer, sizeof(uint32_t) * m_width * m_height);
        m_shader.applyShading(m_Buffer, m_GBuffer, m_width, m_height, renderMode);

        // Save frame to PNG or some other file
        // (You need to implement the frame saving logic here)

        for (const auto& geo : transformedGeometries) {
            delete geo;
        }
    }
}

