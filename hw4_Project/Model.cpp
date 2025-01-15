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
