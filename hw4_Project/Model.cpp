#include "Model.h"

void Model::setAlpha(uint8_t alpha)
{
	m_transp = alpha;
	T->setAlpha(alpha);
}

uint8_t Model::getAlpha() const
{
	return m_transp;
}

// Override the print function
void Model::print() {
    // Implement the print logic for the static model
    std::cout << "StaticModel:" << std::endl;
    // Example: Print the transformation matrix and geometry
    mTransform.print();
    T->print();
}

BBox Model::getGeormetryBbox()
{
	return this->T->getBBox();

}

void Model::modifiyTransformation(const Matrix4& tMat) {
    mTransform = tMat * mTransform;
}

std::unique_ptr<Geometry> Model::applyTransformation(const Matrix4 viewProjectionMatrix, bool flipNormals, int partitions) const
{
	const Matrix4 fTransform = viewProjectionMatrix * mTransform;
	return (!T->isClippedByBBox(fTransform)) ? T->applyTransformation(fTransform, flipNormals, partitions) : nullptr;
}

std::string Model::getModelsName() const
{
	return T->getName();
}
