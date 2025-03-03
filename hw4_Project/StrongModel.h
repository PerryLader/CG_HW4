#pragma once
#ifndef STATICMODEL_H
#define STATICMODEL_H

#include "Model.h"

class StrongModel : public Model {
public:
    // Constructor
    StrongModel(const Model& m) : Model(m) { }

    virtual ~StrongModel() override {
        delete T;
    };
    // Override the draw function
    // Override the print function

};

#endif // STATICMODEL_H
