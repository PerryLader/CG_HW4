#pragma once
#ifndef SCENE_H
#define SCENE_H

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

    void applyToObjectSpace(const Matrix4& tMat);
    void applyToCamera(const Matrix4& tMat);
    void setCamera(CAMERA_TYPE cameraType);
    Camera* getPerspectiveCamera();
    void invalidateLighting(LightParams lights[MAX_LIGHT], LightParams ambient, int sceneSpecExp);
    void handleTransformationAction(int dx, int dy,
         float aspectRatio,
         int action,
         int axis,
         float sensitivity,
         int tSpace,
        int width,
        int height,
        float depth);

    void print() const;

private:
    std::vector<Model*> m_models;
    std::vector<Camera*> m_cameras;
    Renderer* m_renderer;    
    int m_primaryCameraIndex;
};

#endif // SCENE_H
