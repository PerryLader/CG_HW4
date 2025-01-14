#include "Scene.h"
#include "ScreenCommand.h"

// Constructor
Scene::Scene():m_renderer(new Renderer()) {

    Camera* camOrtho = new OrthogonalCamera(Vector3(-1, 1, 1), Vector3(1, -1, 5), 0, 0);
    camOrtho->setOrthogonal();
    m_cameras.push_back(camOrtho);

    Camera* camPrespective = new PerspectiveCamera(45, 1, 1, 5);
    camPrespective->setPerspective();
    m_cameras.push_back(camPrespective);

    m_primaryCameraIndex = CAMERA_TYPE::ORTHOGONAL;
}
// Function to add a model to the scene
void Scene::addModel(Model* model) {
    m_models.push_back(model);
}

void Scene::addModels(const std::vector<Model*>& models) {
    
    BBox sceneBbox(Vector3(FLT_MAX, FLT_MAX, FLT_MAX), Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX));
    for (auto& m : models) sceneBbox.updateBBox(m->getGeormetryBbox());

    Vector3 max = sceneBbox.getMax();
    Vector3 min = sceneBbox.getMin();
    //scale
    //scale the scene into 2,-2 cube
    float scaleFactor = 1 / (std::max(max.x - min.x, std::max(max.y - min.y, max.z - min.z))/2);
    Matrix4 scaleMatrix= Matrix4::scaling(Vector3(scaleFactor, scaleFactor, scaleFactor));

    //centerized
    Vector3 oldCenter = (max + min) * 0.5;
    Vector3 diff = -oldCenter;
    Matrix4 trans = Matrix4::translate(diff);
    for (auto& m : models)
    {        
        m->modifiyTransformation(trans);
        m->modifiyTransformation(scaleMatrix);
    }
   m_models.insert(m_models.end(), models.begin(), models.end());
    for (auto& c : m_cameras)
    {
        c->lookAt(Vector3(0, 0, -3), Vector3(0, 0, 0), Vector3(0, 1, 0));
    }
    

}

// Function to add a camera to the scene
void Scene::addCamera(Camera* camera) {
    
    m_cameras.push_back(camera);
}

// Function to render the scene
void Scene::render(int width, int height, RenderMode& renderMode) const {
    m_renderer->render(m_cameras[m_primaryCameraIndex], width, height, m_models, renderMode);
}

uint32_t* Scene::getBuffer() {
    return m_renderer->getBuffer();
}

void Scene::executeCommand(ScreenCommand* command) {
    command->execute(*this);
}

void Scene::applyToObjectSpace(const Matrix4& tMat) {
    for (Model* mod : m_models) {
        mod->modifiyTransformation(tMat);
    }
}
void Scene::applyToCamera(const Matrix4& tMat) {
    for (auto& c : m_cameras)
    {
        c->translate(tMat);
    }
}

void Scene::setCamera(CAMERA_TYPE cameraType)
{
    this->m_primaryCameraIndex = cameraType;
}
Camera* Scene::getPerspectiveCamera() {
    return m_cameras[CAMERA_TYPE::PRESPECTIVE];
}

static void calculateTransformationMagnitude(float& X_mag, float& Y_mag, int dx, int dy,
    float aspectRatio, float sensitivity, int width, int height, int depth) {
    float depthFactor = 1;
    X_mag = ((float)dx / (float)width) * aspectRatio * sensitivity * depthFactor;
    Y_mag = ((float)dy / (float)height) * sensitivity * depthFactor;
}
void Scene::handleTransformationAction(int dx, int dy,
    float aspectRatio,
    int action,
    int axis,
    float sensitivity,
    int tSpace,
    int width,
    int height,
    float depth) {
    float X_mag, Y_mag; 
    calculateTransformationMagnitude(X_mag, Y_mag,dx,dy, aspectRatio, sensitivity, width, height, depth);
    Vector3 axisVector;
    Matrix4 invTrasformation;
    switch(axis){
    case ID_AXIS_X:
        axisVector = Vector3::unitX();
        break;
    case ID_AXIS_Y:
        axisVector = Vector3::unitY();
        break;
    case ID_AXIS_Z:
        axisVector = Vector3::unitZ();
        break;
    case ID_AXIS_XY:
        axisVector = Vector3::unitX() + Vector3::unitY();
        break;
    case ID_AXIS_XYZ:
        axisVector = Vector3::one();
        break;
    default:
        axisVector = Vector3::unitX();
    }
    float transX, transY, transZ;
    Vector3 scaleV;
    switch (action) {
    case ID_ACTION_ROTATE:
        invTrasformation = Matrix4::rotation(X_mag + Y_mag, axis);
        break;
    case ID_ACTION_SCALE:
        invTrasformation = Matrix4::scaling((Vector3::one() + axisVector * (X_mag + Y_mag)));
        break;
    case ID_ACTION_TRANSLATE:
        transZ = axis == ID_AXIS_Z ? (X_mag + Y_mag) : 0;
        invTrasformation = Matrix4::translate((axisVector.scale(X_mag, -Y_mag, -transZ)));
        break;
    default:
        invTrasformation = Matrix4::rotation(-(X_mag + Y_mag), axis);
    }
    tSpace == ID_OBJECT_SPACE ? applyToObjectSpace(invTrasformation) : applyToCamera(invTrasformation); // should be a comparison between tSpace and some definition of what is ObjectSpace
}
void Scene::invalidateLighting(LightParams lights[MAX_LIGHT], LightParams ambient, int sceneSpecExp) {
    m_renderer->updateLighting(lights, ambient, sceneSpecExp);
}
void Scene::print() const {
    std::cout << "Scene:" << std::endl;
    for (const auto& elem : m_models)
        elem->print();
}