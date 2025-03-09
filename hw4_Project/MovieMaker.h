#pragma once
#include <vector>
#include "Camera.h"
#include "Matrix4.h"
#include "BezierInterpolator.h"
#include "Renderer.h"
#include "Model.h"


class MovieMode {
public:
    int mov_width, mov_height, mov_fps, mov_length, mov_degree;
    bool mov_bez, mov_realSize;
    MovieMode(int mov_width, int mov_height, int mov_fps, int mov_length, int mov_degree, bool mov_bez, bool mov_realSize) : mov_width(mov_width), mov_height(mov_height),
        mov_fps(mov_fps), mov_length(mov_length), mov_degree(mov_degree), mov_bez(mov_bez), mov_realSize(mov_realSize){}
    MovieMode() : mov_width(1400), mov_height(800), mov_fps(25), mov_length(5), mov_degree(1), mov_bez(false) , mov_realSize(true){}
};


class MovieDirector
{
    class MovieScene
    {
    public:
        void push_back(const Matrix4& tMat);

        int size() const;

        const std::shared_ptr<Camera>  ccamera() const;
        std::shared_ptr<Camera>& camera();

        std::vector<Matrix4> getStackedTransformations() const;
        std::vector<Matrix4> getLTransformations() const;
        std::vector<Matrix4> getRTransformations() const;

        void apply_to_frames(const Matrix4& tmat, bool L, bool R, bool rtl);
        void applyObjTrans(const Matrix4& tMat);
        Matrix4 getObjOrigin() const;
        Matrix4 getObjCurrent() const;    
        MovieScene(const Camera* camera) : m_origCamera(camera->clone()) {}

    private:
        std::vector <std::pair<Matrix4, Matrix4>> m_keyTransformations;
        std::shared_ptr<Camera> m_origCamera;
        Matrix4 m_cumulativeObjMat;
    };

public:
    MovieDirector(){
        m_objTrans = m_viewTrans = m_filming = false;
    }
    void create_new_scene(const Camera* camera);
    void addKeyFrame();
    void produceMovie(int width, int height, const MovieMode& mm, RenderMode& rm, Renderer* renderer, const std::vector<Model*>& models);
    void start();
    void stop();
    bool status();
    void applyCameraTrans(const Matrix4& tMat);
    void applyObjTrans(const Matrix4& tMat);

private:
    static std::vector<int> getFramesPerScene(int totalFrames, const std::vector<MovieScene>& mov_scenes);
    static std::vector<double> generateTimeVector(int totalFrames);

    std::vector<MovieScene> m_movie_scenes;
    Matrix4 m_objspaceTrans, m_viewspaceTrans;
    std::vector <Matrix4> m_keyTransformations;
    bool m_objTrans, m_viewTrans , m_filming;
};