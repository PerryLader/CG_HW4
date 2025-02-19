#pragma once
#ifndef SCENE_H
#define SCENE_H
#include <string>
#include <vector>
#include "Model.h"
#include "Camera.h"
#include "Renderer.h"
#include "resource.h"
#include "PngWrapper.h"
#include "Shader.h"
#include "Light.h"

class ScreenCommand;

class Scene {
public:
    
    // Constructor
    Scene();

    // Function to add a model to the scene
    void addModel(Model* model);

    void addModels(const std::vector<Model*>& models);
    // Function to add a camera to the scene
    void addCamera(Camera* camera);
    // Function to render the scene
    void render(int width, int height, RenderMode& renderMode) const;
 //   void setBgfromPng(bool streched, const std::string& fileLocation);
    uint32_t* getBuffer();
    void executeCommand(ScreenCommand* command);
    void setFogColor(const ColorGC& color);
    ColorGC getFogColor()const;
    void startRecording() { m_keyTransformations.push_back(std::pair<std::vector<Matrix4>, Camera*>()); m_movieMaker = true; }
    void stopRecording() { if(!m_keyTransformations.empty()) m_keyTransformations.back().second = m_cameras[m_primaryCameraIndex]->clone(); m_movieMaker = false; }
    void produceMovie(int width, int height, int fps, int length, bool linear, RenderMode& rm);
    void applyToObjectSpace(const Matrix4& tMat);
    void applyToCamera(const Matrix4& tMat);
    void setCamera(CAMERA_TYPE cameraType);
    Camera* getPerspectiveCamera();
    void invalidateLighting(LightParams lights[MAX_LIGHT], LightParams ambient, int sceneSpecExp);
    int getNumOfObjects()const;
    void setAlphaValues(std::vector<std::pair<std::string, int>> table);
    std::vector<std::pair<std::string, int>> getObjNameTable()const;
    void handleTransformationAction(float dx, float dy, float aspectRatio, int action, int axis,
                                    float sensitivity, int tSpace);
    void print() const;

private:
    std::vector<Model*> m_models;
    std::vector<Camera*> m_cameras;
    std::vector<std::pair<std::vector<Matrix4>, Camera*>> m_keyTransformations;
    Renderer* m_renderer;
    int m_primaryCameraIndex;
    bool m_movieMaker; 
};

#endif // SCENE_H

