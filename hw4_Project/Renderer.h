#pragma once
#ifndef RENDERER_H
#define RENDERER_H

#include <vector>
#include <string>
#include "Model.h"
#include "Camera.h"
#include "Shader.h"
#include <iomanip>
#include "PngWrapper.h"
#include <unordered_map>
#include "Modules.h"
#include "BezierInterpolator.h"

class Renderer
{
public:
    Renderer();
    ~Renderer();
   // void addModel(Model* model);
    uint32_t* getBuffer() const;
    void render(const Camera* camera, int width, int height, const std::vector<Model*> models, RenderMode& renderMode);
    void generateMovie(double movieLength, double frameRate, const std::vector<Model*> models, RenderMode& renderMode);
    void updateLighting(LightParams lights[MAX_LIGHT], LightParams ambient, int sceneSpecExp);
    void clear(bool clearBg);

private:
    uint32_t* m_Buffer; // RGB by width by height;
    gData* m_GBuffer; // gData width by height;
    uint32_t* m_BgBuffer; // width by height;
    int m_width, m_height;
    bgInfo m_bgInfo;
    Shader m_shader;
    std::vector<Matrix4> m_keyTMats;
    std::vector<Vector3> m_keyCameraPos;
    //BezierInterpolator interpolator;

    void drawWireFrame(std::vector<Line> lines[LineVectorIndex::LAST]);
    void drawSolid(Shader& shader);
    void drawSilhoutteEdges(const std::unordered_map<Line, EdgeMode, LineKeyHash, LineKeyEqual>& SilhoutteMap);

    void setWidth(int width) { m_width = width;};
    void setHeight(int height) { m_height = height;};

    int getWidth() const { return m_width; };
    int getHeight() const { return m_height; };

    void createBuffers();
    void fillColorBG();
    void fillPngBG();
    void invalidate(const RenderMode& bg_info, bool force);
    void invalidateBG(const RenderMode& bg_info);
    bool isvalidBG(const RenderMode& bg_info);
    /*void interpolateTmatAndCameraPos(Matrix4& tmat, Vector3& pos, double t);
    void setupInterpolator();*/

};

#endif // RENDERER_H


