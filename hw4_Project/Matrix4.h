

#ifndef MATRIX4_H
#define MATRIX4_H

#include <iostream>
#include <sstream>
#include <array>
#include "Vector4.h"
#include "resource.h"

extern int CGSkelInverseMatrix(double M[4][4], double InverseM[4][4]);


class Matrix4 {

    
public:
    std::array<std::array<float, 4>, 4> m;
    // Constructor
    Matrix4();
    // Constructor that takes 16 doubles
    Matrix4(double m00, double m01, double m02, double m03,
            double m10, double m11, double m12, double m13,
            double m20, double m21, double m22, double m23,
            double m30, double m31, double m32, double m33);

    Matrix4(const Vector4& v1, const Vector4& v2, const Vector4& v3, const Vector4& v4);

    // Addition
    Matrix4 operator+(const Matrix4& other) const;

    // Subtraction
    Matrix4 operator-(const Matrix4& other) const;

    // Scaling
    Matrix4 operator*(const float scalar) const;

    // Division
    Matrix4 operator/(const float scalar) const;

    // Compound assignment operators
    Matrix4& operator+=(const Matrix4& other);
    Matrix4& operator-=(const Matrix4& other);
    Matrix4& operator*=(const float scalar);
    Matrix4& operator/=(const float scalar);

    // Multiplication
    Matrix4 operator*(const Matrix4& other) const;

    // Multiplication with Vector4
    Vector4 operator*(const Vector4& vec) const;

    // Inverse using LU Decomposition
    Matrix4 inverse() const;

    // Rotation matrices
    static Matrix4 rotation(float angle, int axis);

    static Matrix4 rotationX(float angle);
    static Matrix4 rotationY(float angle);
    static Matrix4 rotationZ(float angle);

    // Scaling matrix
    static Matrix4 scaling(const Vector3& vec);

    // Translation matrix
    static Matrix4 translate(const Vector3& vec);

    // Identity matrix
    static Matrix4 identity();

    // Transpose
    Matrix4 transpose() const;

    //irit matrix inverse
    Matrix4 irit_inverse() const;

    Vector3 getRow(int i) const;

    Vector3 getCol(int i) const;

    // Function to check if the matrix is symmetric
    bool isSymmetric() const;

    // Function to check if the matrix is positive definite
    bool isPositiveDefinite() const;

    // Function to check if the matrix is invertible
    bool isInvertible() const;

    // Function to determine the decomposition method
    void determineDecompositionMethod() const;

    // LU Decomposition
    void luDecomposition(Matrix4& L, Matrix4& U) const;

    // Cholesky Decomposition
    void choleskyDecomposition(Matrix4& L) const;

    // QR Decomposition
    void qrDecomposition(Matrix4& Q, Matrix4& R) const;

    // Singular Value Decomposition (SVD)
    void svdDecomposition(Matrix4& U, Matrix4& S, Matrix4& V) const;

    // Function to calculate the determinant
    float determinant() const;

    // Function to calculate the trace
    float trace() const;

    // Function to calculate the L1 norm
    float normL1() const;

    // Function to calculate the L2 norm
    float normL2() const;

    // Function to calculate the Linf norm
    float normLinf() const;

    // Print
    void print() const;

    // Overload stream insertion operator
    friend std::ostream& operator<<(std::ostream& os, const Matrix4& mat);

    // Overload stream extraction operator
    friend std::istream& operator>>(std::istream& is, Matrix4& mat);

    // Overload stream extraction operator for string input
    friend std::istringstream& operator>>(std::istringstream& iss, Matrix4& mat);
};

#endif // MATRIX4_H
