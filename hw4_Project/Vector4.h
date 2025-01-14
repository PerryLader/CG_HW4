#ifndef VECTOR4_H
#define VECTOR4_H

#include "Vector3.h"
#include <iostream>
#include <sstream>
#include <cmath>
//#include "Matrix4.h"

class Matrix4;

class Vector4 {
private:
    Vector3 vec;
    float w;
public:
    // Constructor
    Vector4(float x = 0, float y = 0, float z = 0, float w = 1);
    Vector4(const Vector3& vec, float w);

    // Getters 
    float getX() const { return vec.x; }
    float getY() const { return vec.y; } 
    float getZ() const { return vec.z; } 
    float getW() const { return w; }
    // Setters 
    void setX(float x) { vec.x = x; } 
    void setY(float y) { vec.y = y; } 
    void setZ(float z) { vec.z = z; } 
    void setW(float w) { this->w = w; }

    Vector3 toVector3();

    // Static functions for common vectors
    static Vector4 zero();
    static Vector4 one();
    static Vector4 unitX();
    static Vector4 unitY();
    static Vector4 unitZ();
    static Vector4 unitW();
    static Vector4 extendOne(const Vector3& vec);
    static Vector4 extendZero(const Vector3& vec);

    // Print
    void print() const;

    friend std::ostream& operator<<(std::ostream& os, const Vector4& vec);

   // Vector4 mult(const Matrix4& vec) const;

};

#endif // VECTOR4_H
