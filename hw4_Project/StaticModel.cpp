#include "StaticModel.h"
#include <iostream>

// Constructor
StaticModel::StaticModel(Geometry* T) : Model(T){
    // Initialization code if needed
}

// Destructor
StaticModel::~StaticModel(){
}

void StaticModel::draw(/*Renderer& r*/) {
    if (T) {
      //  r.addModel(this);
    }
}

// Override the print function
void StaticModel::print() {
    // Implement the print logic for the static model
    std::cout << "StaticModel:" << std::endl;
    // Example: Print the transformation matrix and geometry
    mTransform.print();
    T->print();
}
