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
	float fogFactor = clampF((pos.z+1) / (fogEnd - fogStart), 0.0f, 1.0f)/1.2/*Magic number just to make it look better can be daleted*/;

	// Blend final color with fog color
	uint8_t tempAlpha=colorBeforeLight.getAlpha();
	ColorGC finalColor = lightColor * colorBeforeLight + specColor;
	//finalColor = (finalColor * (1.0f - fogFactor)) + (m_fogColor * fogFactor);
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

void Shader::perThreadApllyShading(uint32_t* dest, GBuffer* gBuff, std::unordered_map<std::pair<size_t, size_t>, std::reference_wrapper<GBuffer::SetType>, PairHash>& gLists, const RenderMode& rd,const Shader* shader)
{
	int width = gBuff->getWidth();
	SHADE_MODE mode = rd.getRenderShadeFlag();
	for (auto& elem : gLists){
		auto& dataSet = elem.second.get();
		if (dataSet.empty())
			continue;
		ColorGC tmp;
		switch (mode) {
		case SHADE_MODE::GOUROUD:
			tmp = (*dataSet.begin()).pixColor;
			dataSet.erase(dataSet.begin());
			for (const GData& data : dataSet)
			{
				if (tmp.getAlpha() == 255)
				{
					break;
				}
				tmp = ColorGC::alphaColorInterpolating(tmp, data.pixColor);
			}
			break;
		case SHADE_MODE::PHONG:
			if ((*dataSet.begin()).m_pixType == pixType::FROM_POLYGON)
			{
				tmp = shader->calcLightColorAtPos(
					(*dataSet.begin()).pixPos,
					(*dataSet.begin()).pixNorm,
					(*dataSet.begin()).polygon->getColor());
			}
			else
			{
				tmp = (*dataSet.begin()).pixColor;
			}
			dataSet.erase(dataSet.begin());
			for (const GData& data : dataSet)
			{
				if (tmp.getAlpha() == 255)
				{
					break;
				}
				if (data.m_pixType == pixType::FROM_POLYGON)
				{
					tmp = ColorGC::alphaColorInterpolating(tmp,
						shader->calcLightColorAtPos(
							data.pixPos,
							data.pixNorm,
							data.polygon->getColor()));
				}
				else
				{
					tmp = ColorGC::alphaColorInterpolating(tmp, data.pixColor);
				}
			}
			break;
		case SHADE_MODE::SOLID:
			//auto tempForDebug = dataSet;
			if ((*dataSet.begin()).m_pixType == pixType::FROM_POLYGON)
			{
				tmp = (*dataSet.begin()).polygon->getSceneColor();
			}
			else
			{
				tmp = (*dataSet.begin()).pixColor;
			}
			dataSet.erase(dataSet.begin());
			for (const GData& data : dataSet)
			{
				if (tmp.getAlpha() == 255)
				{
					break;
				}
				if (data.m_pixType == pixType::FROM_POLYGON)
				{
					tmp = ColorGC::alphaColorInterpolating(tmp, data.polygon->getSceneColor());
				}
				else
				{
					tmp = ColorGC::alphaColorInterpolating(tmp, data.pixColor);
				}
			}
			break;
		}
		ColorGC finalColor = ColorGC::alphaColorInterpolating(tmp, ColorGC(dest[(elem.first.second * width) + elem.first.first]));
		dest[(elem.first.second * width) + elem.first.first] = finalColor.getARGB();
	}
}
void Shader::applyShading(uint32_t* dest, GBuffer& gBuff, const RenderMode& rd) const{
	int width = gBuff.getWidth();
	int height = gBuff.getHeight();

	SHADE_MODE mode = rd.getRenderShadeFlag();
	if (mode == SHADE_MODE::NONE)
		return;
	int numThreads = 4;
	auto& parts = gBuff.getNParts(numThreads);
	std::vector<std::thread> threads;
	//int rowsPerThread = height / numThreads;
	auto start = std::chrono::high_resolution_clock::now();

	for (int i = 0; i < numThreads; ++i) {
		//int startRow = i * rowsPerThread;
		//int endRow = (i == numThreads - 1) ? height : startRow + rowsPerThread;

		// Create a thread for each portion of the rows
		threads.emplace_back(perThreadApllyShading, dest, &gBuff,  parts[i], rd, this);
	}
	for (auto& t : threads) {
		t.join();
	}
	auto end = std::chrono::high_resolution_clock::now();

	// Calculate the elapsed time in milliseconds
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

	std::cout << "Shading execution time: " << duration.count() << " ms" << std::endl;
}
void Shader::setFogColor(const ColorGC& color)
{
	this->m_fogColor = color;
}

ColorGC Shader::getFogColor() const
{
	return m_fogColor;

}
