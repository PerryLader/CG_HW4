#include "shader.h"
#include "Polygon.h"
Vector3 reflect(const Vector3& dir, const Vector3& normal, Vector3 V)
{
	float dotProduct = 2 * Vector3::dot(dir, normal);
	return (dir - (normal * dotProduct)).normalized();
}
float positivesOnly(float x) { return std::max((float)(0.0),x); }

ColorGC Shader::calcLightColorAtPos(Vector3 pos, Vector3 normal, ColorGC colorBeforeLight) const
{
	ColorGC lightColor = m_ambient.getColor() * m_ambient.Ipower;
	ColorGC specColor = (0, 0, 0);
	for (int id = LIGHT_ID_1; id < MAX_LIGHT; id++) if (m_lightSources[id].enabled)
	{
		Vector3 lightdir;
		switch (m_lightSources[id].type) {
		case LightSourceType::LightSourceType_DIRECTIONAL:
			lightdir = m_lightSources[id].getDir().normalized();
			break;
		case LightSourceType::LightSourceType_POINT:
			lightdir = (pos - m_lightSources[id].getPos()).normalized();
			break;
		default:
			throw;
		}
		Vector3 V = m_isperspective ? ((m_mat_inv* Vector4::extendOne(pos)).toVector3()).normalized() : -Vector3::unitZ();
		Vector3 R = reflect(lightdir, normal, V);
		double Ks = m_lightSources[id].Kspec;
		double Kd = m_lightSources[id].Kdiff;
		double Ip = m_lightSources[id].Ipower;

		// Calculate diffuse component
		float diffuseIntensity = Kd * positivesOnly(Vector3::dot(lightdir, -normal));
		ColorGC diffuseColor = m_lightSources[id].getColor() * (Ip * diffuseIntensity);

		// Calculate specular component
		float specularIntensity = Ks * std::pow(positivesOnly(Vector3::dot(R, V)), m_specularityExp);
		ColorGC specularColor = m_lightSources[id].getColor() * (Ip * specularIntensity);

		// Combine diffuse and specular components
		lightColor = lightColor + diffuseColor;
		specColor = specColor + specularColor;
	}

	return lightColor * colorBeforeLight + specColor;
}

Shader::Shader() {
	m_ambient.Ipower = 0.15;
	m_specularityExp = 2;
}

void Shader::updateLighting(LightParams lights[MAX_LIGHT], LightParams ambient, int sceneSpecExp){
	for (int id = LIGHT_ID_1; id < MAX_LIGHT; id++)
		m_lightSources[id] = lights[id];
		m_ambient = ambient;
		m_specularityExp = sceneSpecExp;
}

void Shader::applyShading(uint32_t* dest, const gData* gBuffer, int width, int height, const RenderMode& rd) const{
	SHADE_MODE mode = rd.getRenderShadeFlag();
	if (mode == SHADE_MODE::NONE)
		return;
	for (size_t y = 0; y < height; y++)
	{
		for (size_t x = 0; x < width; x++) if(gBuffer[(y * width) + x].polygon) {
			ColorGC tmp;
			switch (mode) {
			case SHADE_MODE::GOUROUD:
				tmp = gBuffer[(y * width) + x].pixColor;
				break;
			case SHADE_MODE::PHONG:
				tmp = calcLightColorAtPos(gBuffer[(y * width) + x].pixPos,gBuffer[(y * width) + x].pixNorm, gBuffer[(y * width) + x].polygon->getColor());
				break;
			case SHADE_MODE::SOLID:
				tmp = gBuffer[(y * width) + x].polygon->getSceneColor();
					break;
			}
			dest[(y * width) + x] = tmp.getARGB();
		}
	}
}
