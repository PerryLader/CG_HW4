#pragma once
#include "Modules.h"
#include <algorithm>
#include <cmath>
#include "Light.h"


class Shader
{
private:
    ColorGC m_fogColor;
    float m_fogIntensity;
    bool m_fogEnabled;

    float m_specularityExp;
    LightParams m_lightSources[LightID::MAX_LIGHT];
    Vector3 m_light_pos[LightID::MAX_LIGHT];
    LightParams m_ambient;
    Vector3 m_viewPos;
    bool m_isperspective;
    Matrix4 m_mat_inv;
    //static void perThreadApllyShading(uint32_t* dest, GBuffer& gBuff, std::vector<std::reference_wrapper<const std::pair<const int, GBuffer::SetType>>>& gLists, const RenderMode& rd, const Shader* shader);
    static void perThreadApllyShading(uint32_t* dest, GBuffer& gBuff, std::vector<std::pair<int, GBuffer::SetType*>>& gLists, const RenderMode& rd, const Shader* shader);

	//virtual void scanConvertion() = 0;
public:
    //virtual void draw(std::vector<Geometry*> objs) = 0;
    ColorGC calcLightColorAtPos(Vector3 pos, Vector3 normal, ColorGC colorBeforeLight) const;
    void applyShading(uint32_t* dest, GBuffer& gBuff, const RenderMode& rd) const;
    Shader();
    void setFog(const ColorGC& color, float intensity, bool enabled);
    ColorGC getFogColor()const;
    void updateLighting(LightParams lights[MAX_LIGHT], LightParams ambient, int sceneSpecExp);


    void setView(const Vector3& camera_pos, const Matrix4& mat_inv , bool isPerspective) {
        m_viewPos = camera_pos;
        m_mat_inv = mat_inv;
        m_isperspective = isPerspective;
    }
};

