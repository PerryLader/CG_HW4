#include "Vector3.h"

// Constructor
Vector3::Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

void Vector3::adjustForNumericalErrors(float eps)
{
   
    if (fabs(this->x - -1) < eps) this->x = -1;
    if (fabs(this->x - 1) < eps) this->x = 1;
    if (fabs(this->y - -1) < eps) this->y = -1;
    if (fabs(this->y - 1) < eps) this->y = 1;
    if (fabs(this->z - -1) < eps)this->z = -1;
    if (fabs(this->z - 1) < eps) this->z = 1;
    
}

bool Vector3::isPointInUnitCube() const
{
        return (x >= -1 && x <= 1 &&
            y >= -1 && y <= 1 &&
            z >= -1 && z <= 1);
}

// Addition
Vector3 Vector3::operator+(const Vector3& other) const {
    __m128 vecValues = _mm_set_ps(0.0f, z, y, x);
    __m128 otherValues = _mm_set_ps(0.0f, other.z, other.y, other.x);
    //__m128 scalarValue = _mm_set1_ps(scalar);
    __m128 result = _mm_add_ps(vecValues, otherValues);
    float resultArray[4];
    _mm_storeu_ps(resultArray, result);
    return Vector3(resultArray[0], resultArray[1], resultArray[2]);
}

// Subtraction
Vector3 Vector3::operator-(const Vector3& other) const {
    __m128 vecValues = _mm_set_ps(0.0f, z, y, x);
    __m128 otherValues = _mm_set_ps(0.0f, other.z, other.y, other.x);
    //__m128 scalarValue = _mm_set1_ps(scalar);
    __m128 result = _mm_sub_ps(vecValues, otherValues);
    float resultArray[4];
    _mm_storeu_ps(resultArray, result);
    return Vector3(resultArray[0], resultArray[1], resultArray[2]);
}

// Scaling
Vector3 Vector3::operator*(const float scalar) const {
    __m128 vecValues = _mm_set_ps(0.0f, z, y, x);
    //__m128 otherValues = _mm_set_ps(other.z, other.z, other.y, other.x);
    __m128 scalarValue = _mm_set1_ps(scalar);
    __m128 result = _mm_mul_ps(vecValues, scalarValue);
    float resultArray[4];
    _mm_storeu_ps(resultArray, result);
    return Vector3(resultArray[0], resultArray[1], resultArray[2]);
}

// Division
Vector3 Vector3::operator/(const float scalar) const {
    __m128 vecValues = _mm_set_ps(0.0f, z, y, x);
    //__m128 otherValues = _mm_set_ps(other.z, other.z, other.y, other.x);
    __m128 scalarValue = _mm_set1_ps(scalar);
    __m128 result = _mm_div_ps(vecValues, scalarValue);
    float resultArray[4];
    _mm_storeu_ps(resultArray, result);
    return Vector3(resultArray[0], resultArray[1], resultArray[2]);
}

// Compound assignment operators
Vector3& Vector3::operator+=(const Vector3& other) {
    *this = *this + other;
    return *this;
}

Vector3& Vector3::operator-=(const Vector3& other) {
    *this = *this - other;
    return *this;
}

Vector3& Vector3::operator*=(const float scalar) {
    *this = *this * scalar;
    return *this;
}

Vector3& Vector3::operator/=(const float scalar) {
    *this = *this / scalar;
    return *this;
}

// Rotation around the X-axis
Vector3 Vector3::rotationX(const Vector3& vec, float angle) {
    float c = std::cos(angle);
    float s = std::sin(angle);
    return Vector3(
        vec.x,
        vec.y * c - vec.z * s,
        vec.y * s + vec.z * c
    );
}

// Rotation around the Y-axis
Vector3 Vector3::rotationY(const Vector3& vec, float angle) {
    float c = std::cos(angle);
    float s = std::sin(angle);
    return Vector3(
        vec.x * c + vec.z * s,
        vec.y,
        -vec.x * s + vec.z * c
    );
}

// Rotation around the Z-axis
Vector3 Vector3::rotationZ(const Vector3& vec, float angle) {
    float c = std::cos(angle);
    float s = std::sin(angle);
    return Vector3(
        vec.x * c - vec.y * s,
        vec.x * s + vec.y * c,
        vec.z
    );
}

// Scaling
Vector3 Vector3::scaling(const Vector3& vec, float sx, float sy, float sz) {
    __m128 vecValues = _mm_set_ps(0.0f, vec.z, vec.y, vec.x);
    __m128 otherValues = _mm_set_ps(0.0f, sz, sy, sx);
    //__m128 scalarValue = _mm_set1_ps(scalar);
    __m128 result = _mm_mul_ps(vecValues, otherValues);
    float resultArray[4];
    _mm_storeu_ps(resultArray, result);
    return Vector3(resultArray[0], resultArray[1], resultArray[2]);
    //return Vector3(
    //    vec.x * sx,
    //    vec.y * sy,
    //    vec.z * sz
    //);
}

Vector3 Vector3::scale(float sx, float sy, float sz) {
    this->x *= sx;
    this->y *= sy;
    this->z *= sz;
    return *this;
}
void Vector3::xyRound()
{
    x = std::round(x);
    y = std::round(y);
}
// Translation
Vector3 Vector3::translate(const Vector3& vec, float tx, float ty, float tz) {
    return Vector3(
        vec.x + tx,
        vec.y + ty,
        vec.z + tz
    );
}

// Dot product
float Vector3::dot(const Vector3& v1, const Vector3& v2) {
    __m128 vec1Values = _mm_set_ps(0.0f, v1.z, v1.y, v1.x);
    __m128 vec2Values = _mm_set_ps(0.0f, v2.z, v2.y, v2.x);

    __m128 result = _mm_dp_ps(vec1Values, vec2Values, 0x71);
    float resultArray[4];
    _mm_storeu_ps(resultArray, result);

    return resultArray[0]; // The result of the dot product is stored in the lowest element
}

// Cross product
Vector3 Vector3::cross(const Vector3& v1, const Vector3& v2) {
    float zero = 0.0;
    __m128 vec11Values = _mm_set_ps(zero, v1.x, v1.z, v1.y);
    __m128 vec21Values = _mm_set_ps(zero, v2.y, v2.x, v2.z);
    __m128 vec12Values = _mm_set_ps(zero, v1.y, v1.x, v1.z);
    __m128 vec22Values = _mm_set_ps(zero, v2.x, v2.z, v2.y);
    //__m128 scalarValue = _mm_set1_ps(scalar);
    __m128 result = _mm_sub_ps(_mm_mul_ps(vec11Values, vec21Values), _mm_mul_ps(vec12Values, vec22Values));
    float resultArray[4];
    _mm_storeu_ps(resultArray, result);
    return Vector3(resultArray[0], resultArray[1], resultArray[2]);

}

// Magnitude
float Vector3::length() const {
    float zero = 0.0;
    __m128 vec1Values = _mm_set_ps(zero, z, y, x);
    //__m128 scalarValue = _mm_set1_ps(scalar);
    __m128 result = _mm_mul_ps(vec1Values, vec1Values);
    float resultArray[4];
    _mm_storeu_ps(resultArray, result);
    return std::sqrt(resultArray[0] + resultArray[1] + resultArray[2]);
}

// Normalize
Vector3 Vector3::normalized() const {
    float len = length();
    if (len > 0) {
        return *this / len;
    }
    return Vector3();
}

// Static functions for common vectors
Vector3 Vector3::zero() {
    return Vector3(0, 0, 0);
}

Vector3 Vector3::one() {
    return Vector3(1, 1, 1);
}

Vector3 Vector3::unitX() {
    return Vector3(1, 0, 0);
}

Vector3 Vector3::unitY() {
    return Vector3(0, 1, 0);
}

Vector3 Vector3::unitZ() {
    return Vector3(0, 0, 1);
}

// Print
void Vector3::print() const {
    std::cout << "(" << x << ", " << y << ", " << z << ")" << std::endl;
}

Vector3 operator-(const Vector3& vec) {
    return Vector3(-vec.x, -vec.y, -vec.z);
}
// Overload stream insertion operator
std::ostream& operator<<(std::ostream& os, const Vector3& vec) {
    os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
    return os;
}

// Overload stream extraction operator for formatted input
std::istream& operator>>(std::istream& is, Vector3& vec) {
    char comma;
    if (!(is >> vec.x >> comma >> vec.y >> comma >> vec.z)) {
        is.setstate(std::ios::failbit);
    }
    return is;
}

// Overload stream extraction operator for string input
std::istringstream& operator>>(std::istringstream& iss, Vector3& vec) {
    char comma;
    if (!(iss >> vec.x >> comma >> vec.y >> comma >> vec.z)) {
        iss.setstate(std::ios::failbit);
    }
    return iss;
}

float& Vector3::operator[](std::size_t index) {
    if (index == 0) return x;
    if (index == 1) return y;
    if (index == 2) return z;
    throw std::out_of_range("Index must be 0, 1, or 2");
}

// Access operator for reading only
const float& Vector3::operator[](std::size_t index) const {
    if (index == 0) return x;
    if (index == 1) return y;
    if (index == 2) return z;
    throw std::out_of_range("Index must be 0, 1, or 2");
}

bool Vector3::operator==(const Vector3& other) const {
    return x == other.x && y == other.y && z == other.z;
}
