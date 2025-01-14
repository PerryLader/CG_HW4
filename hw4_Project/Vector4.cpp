#include "Vector4.h"


// Constructor
Vector4::Vector4(float x, float y, float z, float w) : vec(x,y,z), w(w) {}
Vector4::Vector4(const Vector3& vec, float w) : vec(vec), w(w) {}

Vector3 Vector4::toVector3() {
    return vec / w;
}
// Static functions for common vectors
Vector4 Vector4::zero() {
    return Vector4(0, 0, 0, 0);
}

Vector4 Vector4::one() {
    return Vector4(1, 1, 1, 1);
}

Vector4 Vector4::unitX() {
    return Vector4(1, 0, 0, 0);
}

Vector4 Vector4::unitY() {
    return Vector4(0, 1, 0, 0);
}

Vector4 Vector4::unitZ() {
    return Vector4(0, 0, 1, 0);
}

Vector4 Vector4::unitW() {
    return Vector4(0, 0, 0, 1);
}

Vector4 Vector4::extendOne(const Vector3& vec) {
    return Vector4(vec, 1);
}
Vector4 Vector4::extendZero(const Vector3& vec) {
    return Vector4(vec, 0);
}

// Print
void Vector4::print() const {
    std::cout << "(" << getX() << ", " << getY() << ", " << getZ() << ", " << getW() << ")" << std::endl;
}

// Overload stream insertion operator
std::ostream& operator<<(std::ostream& os, const Vector4& vec) {
    os << "(" << vec.getX() << ", " << vec.getY() << ", " << vec.getZ() << ", " << vec.getW() << ")";
    return os;
}
