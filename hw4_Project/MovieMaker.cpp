#include "MovieMaker.h"

void MovieDirector::MovieScene::push_back(const Matrix4& tMat) {
    m_keyTransformations.push_back(std::pair<Matrix4, Matrix4>(tMat, m_cumulativeObjMat));
}

int MovieDirector::MovieScene::size() const {
    return m_keyTransformations.size();
}

std::shared_ptr<Camera>&  MovieDirector::MovieScene::camera(){
    return m_origCamera;
}

const std::shared_ptr<Camera> MovieDirector::MovieScene::ccamera() const{
    return m_origCamera;
}
void MovieDirector::MovieScene::apply_to_frames(const Matrix4& tmat , bool L, bool R, bool rtl) {
    if (rtl) {
        for (auto& frame : m_keyTransformations) {
            if (L) frame.first = frame.first * tmat;
            if (R) 
                frame.second = frame.second * tmat;
        }
    }
    else {
        for (auto& frame : m_keyTransformations) {
            if (L) frame.first = tmat * frame.first;
            if (R) frame.second = tmat * frame.second;
        }
    }

}

std::vector<Matrix4> MovieDirector::MovieScene::getLTransformations() const {
    std::vector<Matrix4> res(m_keyTransformations.size());
    std::transform(m_keyTransformations.begin(), m_keyTransformations.end(), res.begin(),
        [](const std::pair<Matrix4, Matrix4>& p) { return p.first; });
    return res;
}

std::vector<Matrix4> MovieDirector::MovieScene::getRTransformations() const {
    std::vector<Matrix4> res(m_keyTransformations.size());
    std::transform(m_keyTransformations.begin(), m_keyTransformations.end(), res.begin(),
        [](const std::pair<Matrix4, Matrix4>& p) { return p.second; });
    return res;
}

void MovieDirector::MovieScene::applyObjTrans(const Matrix4& tMat) {
    m_cumulativeObjMat = tMat * m_cumulativeObjMat;
}

Matrix4 MovieDirector::MovieScene::getObjOrigin() const {
    return m_cumulativeObjMat.irit_inverse();
}
Matrix4 MovieDirector::MovieScene::getObjCurrent() const {
    return m_cumulativeObjMat;
}

void MovieDirector::create_new_scene(const Camera* camera) {
    m_movie_scenes.push_back(camera);
    m_movie_scenes.back().push_back(Matrix4::identity());
}

void MovieDirector::addKeyFrame() {
    if (!m_movie_scenes.empty()) {
        if (m_viewTrans) {
            m_movie_scenes.back().push_back(m_viewspaceTrans);
            m_viewTrans = false;
        }
        if (m_objTrans) {
            m_movie_scenes.back().push_back(m_viewspaceTrans);
            m_objTrans = false;
        }
    }
}
void MovieDirector::start() {
    m_filming = true;
}
void MovieDirector::stop() {
    m_filming = false;
}
bool MovieDirector::status() {
    return m_filming;
}

void MovieDirector::applyCameraTrans(const Matrix4& tMat) {
    if (status()) {
        m_viewspaceTrans = tMat * m_viewspaceTrans;
        m_viewTrans = true;
    }
}
void MovieDirector::applyObjTrans(const Matrix4& tMat) {
    for (auto& scene : m_movie_scenes) {
        scene.applyObjTrans(tMat);
    }
    if (status()) {
        m_objTrans = true;
    }
}

std::vector<Model*> copyModels(const std::vector<Model*>& models) {
    std::vector<Model*> weakModels;

    // Convert each Model pointer to a WeakModel instance
    for (const Model* model : models) {
        // Using the copy constructor of WeakModel and storing it in a unique_ptr
        weakModels.emplace_back(new Model(*model));
    }
    return weakModels;
}

void MovieDirector::produceMovie(int width, int height, const MovieMode& mm, RenderMode& rm , Renderer* renderer ,const std::vector<Model*>& models) {
    // Create a vector of Model pointers to hold WeakModel instances
    int totalFrames = mm.mov_fps * mm.mov_length;
    int mov_width = mm.mov_width, mov_height = mm.mov_height;
    if (mm.mov_realSize) {
        mov_width = width;
        mov_height = height;
    }
    std::vector<int> frames_per_scene = getFramesPerScene(totalFrames, m_movie_scenes);
    char frameName[40];
    for (int j = 0; j < m_movie_scenes.size(); j++) {
        BSplineInterpolator interpolator(m_movie_scenes[j].size(), mm.mov_bez ? m_movie_scenes[j].size() : mm.mov_degree);
        std::vector<double> timeVec = generateTimeVector(frames_per_scene[j]);
        std::vector<Matrix4> LeftkeyFrameInterpolants = m_movie_scenes[j].getLTransformations();
        std::vector<Matrix4> RightkeyFrameInterpolants = m_movie_scenes[j].getRTransformations();
        for (int i = 0; i < timeVec.size(); ++i) {
            std::vector<Model*> weakModels = copyModels(models);
            Matrix4 objectCorrection = m_movie_scenes[j].getObjOrigin();
            for (auto& mod : weakModels) {
                mod->modifiyTransformation(objectCorrection);
            }
            Camera* interpolatedCamera = m_movie_scenes[j].camera()->clone();
            interpolatedCamera->translate(interpolator.interpolate(LeftkeyFrameInterpolants, timeVec[i]));
            Matrix4 interpolatedObjTrans = interpolator.interpolate(RightkeyFrameInterpolants, timeVec[i]);
            for (auto& mod : weakModels) {
                mod->modifiyTransformation(interpolatedObjTrans);
            }
            renderer->render(interpolatedCamera, mov_width, mov_height, weakModels, rm);
            sprintf(frameName, "movie/scene%d_f%d.png", j, i);
            PngWrapper png = PngWrapper(frameName, mov_width, mov_height);
            png.WriteFromBuffer(renderer->getBuffer());
            delete interpolatedCamera;
            for (auto& mod : weakModels) {
                delete mod;
            }
        }
    }
}

std::vector<double> MovieDirector::generateTimeVector(int totalFrames) {
    std::vector<double> tValues;
    tValues.reserve(totalFrames);
    for (int i = 0; i < totalFrames; ++i) {
        double t = static_cast<double>(i) / (totalFrames - 1);
        tValues.push_back(t);
    } return tValues;
}

std::vector<int> MovieDirector::getFramesPerScene(int totalFrames, const std::vector<MovieScene>& mov_scenes) {
    std::vector<int> res;
    int sumOfKeyFrames = 0;
    for (const auto& elem : mov_scenes) {
        res.push_back(elem.size());
        sumOfKeyFrames += elem.size();
    }
    for (int& elem : res) {
        elem = (totalFrames * elem) / sumOfKeyFrames;
    }
    return res;
}