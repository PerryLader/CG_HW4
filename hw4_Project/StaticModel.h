#pragma once
#ifndef STATICMODEL_H
#define STATICMODEL_H

#include "Model.h"

class StaticModel : public Model {
public:
    // Constructor
    StaticModel(Geometry* T);

    // Destructor
    ~StaticModel() override;
    // Override the draw function
    void draw(/*Renderer& r*/) override;
    // Override the print function
    void print() override;
};

#endif // STATICMODEL_H
