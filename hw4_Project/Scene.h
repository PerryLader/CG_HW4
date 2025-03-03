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
#include "MovieMaker.h"


class ScreenCommand;

class Scene {
public:
    
    // Constructor
    Scene();

    ~Scene() {
        for (auto& camera : m_cameras)
            delete camera;

        for (auto& model : m_models)
            delete model;

        delete m_renderer;
    }

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
    void setFog(const ColorGC& color , float intesity, bool enabled);
    ColorGC getFogColor()const;
    void startRecording();
    bool getRecordingStatus();
    void stopRecording();
    void addKeyFrame();
    void produceMovie(int width, int height, const MovieMode& mm, RenderMode& rm);
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
    MovieDirector m_movieDirector;
    Renderer* m_renderer;
    int m_primaryCameraIndex;
    bool m_movieMaker; 
};

#endif // SCENE_H

