#include "shader.h"
#include "Polygon.h"
#include <chrono>
#include <thread>

float clampF(float x, float low, float high) {
	if (x < low) return low;
	if (x > high) return high;
	return x;
}
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
		Vector3 V = m_isperspective ? ((m_mat_inv * Vector4::extendOne(pos)).toVector3()).normalized() : -Vector3::unitZ();
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
	

	// Calculate fog factor (linear interpolation based on distance)
	float fogStart = -1;     // Fog starts at this distance
	float fogEnd = 1;         // Fog fully covers at this distance
	float fogFactor = clampF((pos.z+1) / (fogEnd - fogStart), 0.0f, 1.0f) * m_fogIntensity/*Magic number just to make it look better can be daleted*/;

	// Blend final color with fog color
	uint8_t tempAlpha=colorBeforeLight.getAlpha();
	ColorGC finalColor = lightColor * colorBeforeLight + specColor;
	if(m_fogEnabled) finalColor = (finalColor * (1.0f - fogFactor)) + (m_fogColor * fogFactor);
	finalColor.setAlpha(tempAlpha);
	return finalColor;
}

Shader::Shader() {
	m_ambient.Ipower = 0.15;
	m_specularityExp = 2;
	m_fogColor = ColorGC(230, 230, 230);
}

void Shader::updateLighting(LightParams lights[MAX_LIGHT], LightParams ambient, int sceneSpecExp){
	for (int id = LIGHT_ID_1; id < MAX_LIGHT; id++)
		m_lightSources[id] = lights[id];
		m_ambient = ambient;
		m_specularityExp = sceneSpecExp;
}
void Shader::perThreadApllyShading(uint32_t* dest, GBuffer& gBuff, std::vector<std::pair<int, GBuffer::SetType*>>& gLists, const RenderMode& rd, const Shader* shader) {
//void Shader::perThreadApllyShading(uint32_t* dest, GBuffer& gBuff, std::vector<std::reference_wrapper<const std::pair<const int, GBuffer::SetType>>>& gLists, const RenderMode& rd, const Shader* shader){
int width = gBuff.getWidth();
	SHADE_MODE mode = rd.getRenderShadeFlag();

	for (auto& elem : gLists) {
		auto& dataSet = *elem.second;
		if (dataSet.empty())
			continue;
		std::sort(dataSet.begin(), dataSet.end());
		ColorGC tmp;
		auto start = dataSet.begin();
		switch (mode) {
		case SHADE_MODE::NONE:
			tmp = start->pixColor;
			break;
		case SHADE_MODE::GOUROUD:
			tmp = start->pixColor;
			for (auto it = std::next(start); it != dataSet.end() && tmp.getAlpha() != 255; ++it)
				tmp = ColorGC::alphaColorInterpolating(tmp, it->pixColor);
			break;
		case SHADE_MODE::PHONG:
			tmp = start->polygon ?
				shader->calcLightColorAtPos(start->pixPos, start->pixNorm, start->polygon->getColor()) : start->pixColor;
			for (auto it = std::next(start); it != dataSet.end() && tmp.getAlpha() != 255; ++it) {
				tmp = it->polygon ?
					ColorGC::alphaColorInterpolating(tmp, shader->calcLightColorAtPos(it->pixPos, it->pixNorm, it->polygon->getColor())): it->pixColor;
			}
			break;

		case SHADE_MODE::SOLID:
			tmp = start->polygon ?
				start->polygon->getSceneColor() : start->pixColor;
			for (auto it = std::next(start); it != dataSet.end() && tmp.getAlpha() != 255; ++it) {
				tmp = it->polygon ?
					ColorGC::alphaColorInterpolating(tmp, it->polygon->getSceneColor()) :
					ColorGC::alphaColorInterpolating(tmp, it->pixColor);
			}
			break;
		}
		int key = elem.first;
		ColorGC finalColor = ColorGC::alphaColorInterpolating(tmp, ColorGC(dest[key]));
		dest[key] = finalColor.getARGB();
	}
}

void Shader::applyShading(uint32_t* dest, GBuffer& gBuff, const RenderMode& rd) const {

	SHADE_MODE mode = rd.getRenderShadeFlag();
	int numThreads = 4;
	std::vector<std::vector<std::pair<int, GBuffer::SetType*>>> parts = gBuff.getNParts(numThreads);
	//std::vector<std::vector<std::reference_wrapper<const std::pair<const int, GBuffer::SetType>>>> parts = gBuff.getNParts(numThreads);
	std::vector<std::thread> threads;
	for (int i = 0; i < numThreads; ++i) {
		threads.emplace_back(perThreadApllyShading, dest, std::ref(gBuff), std::ref(parts[i]), rd, this);
	}
	for (auto& t : threads) {
		t.join();
	}
}

void Shader::setFog(const ColorGC& color, float intensity, bool enabled)
{
	this->m_fogIntensity = intensity;
	this->m_fogEnabled = enabled;
	this->m_fogColor = color;
}

ColorGC Shader::getFogColor() const
{
	return m_fogColor;

}
