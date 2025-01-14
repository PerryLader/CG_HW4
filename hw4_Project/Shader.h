#pragma once
#include "Modules.h"
#include <algorithm>
#include <cmath>
#include "Light.h"


class Shader
{
private:
    float m_specularityExp;
    LightParams m_lightSources[LightID::MAX_LIGHT];
    Vector3 m_light_pos[LightID::MAX_LIGHT];
    LightParams m_ambient;
    Vector3 m_viewPos;
    bool m_isperspective;
    Matrix4 m_mat_inv;
	//virtual void scanConvertion() = 0;
public:
    //virtual void draw(std::vector<Geometry*> objs) = 0;
    ColorGC calcLightColorAtPos(Vector3 pos, Vector3 normal, ColorGC colorBeforeLight) const;
    void applyShading(uint32_t* dest, const gData* gBuffer, int width, int height, const RenderMode& rd) const;
    Shader();

    void updateLighting(LightParams lights[MAX_LIGHT], LightParams ambient, int sceneSpecExp);

    void setView(const Vector3& camera_pos, const Matrix4& mat_inv , bool isPerspective) {
        m_viewPos = camera_pos;
        m_mat_inv = mat_inv;
        m_isperspective = isPerspective;
    }

    void applyTransformationToLights(const Matrix4& tMat) {
        for (int id = LIGHT_ID_1; id < MAX_LIGHT; id++) if (m_lightSources[id].type == LIGHT_TYPE_POINT)
            m_light_pos[id] = (tMat * Vector4::extendOne(m_lightSources[id].getPos())).toVector3();
    }
};

//class NoShadeShader :
//    public Shader
//{
//private:
//    
//public:
//
//    
//};

