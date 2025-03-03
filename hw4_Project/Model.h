#ifndef	MODEL_H
#define	MODEL_H
#include <iomanip>
#include "Geometry.h"
#include <vector>

class Model
{
protected:

	Geometry* T;
	Matrix4 mTransform;
	uint8_t m_transp;

public:
	virtual ~Model() {}

	Model(Geometry* T) : T(T), mTransform(Matrix4::identity()), m_transp(255) {};
	
	std::string getModelsName() const;

	std::unique_ptr<Geometry> applyTransformation(const Matrix4 viewProjectionMatrix, bool flipNormals) const;

	void modifiyTransformation(const Matrix4& tMat);

	void setAlpha(uint8_t alpha);

	uint8_t getAlpha()const;

	BBox getGeormetryBbox();

	void virtual print();
};



class StrongModel : public Model {
public:
	// Constructor
	StrongModel(const Model& m) : Model(m) { }

	virtual ~StrongModel() override {
		delete T;
	};
};



#endif