#include "Matrix4.h"
#include <cmath> 
#include <iostream>

// Constructor
Matrix4::Matrix4() {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            m[i][j] = (i == j) ? 1.0f : 0.0f; // Identity matrix
        }
    }
}

// Constructor that takes 16 doubles
Matrix4::Matrix4(
    double m00, double m01, double m02, double m03,
    double m10, double m11, double m12, double m13,
    double m20, double m21, double m22, double m23,
    double m30, double m31, double m32, double m33) {
    m[0][0] = m00; m[0][1] = m01; m[0][2] = m02; m[0][3] = m03;
    m[1][0] = m10; m[1][1] = m11; m[1][2] = m12; m[1][3] = m13;
    m[2][0] = m20; m[2][1] = m21; m[2][2] = m22; m[2][3] = m23;
    m[3][0] = m30; m[3][1] = m31; m[3][2] = m32; m[3][3] = m33;
}

Matrix4::Matrix4(const Vector4& v1, const Vector4& v2, const Vector4& v3, const Vector4& v4){
    m[0][0] = v1.getX(); m[0][1] = v2.getX(); m[0][2] = v3.getX(); m[0][3] = v4.getX();
    m[1][0] = v1.getY(); m[1][1] = v2.getY(); m[1][2] = v3.getY(); m[1][3] = v4.getY();
    m[2][0] = v1.getZ(); m[2][1] = v2.getZ(); m[2][2] = v3.getZ(); m[2][3] = v4.getZ();
    m[3][0] = v1.getW(); m[3][1] = v2.getW(); m[3][2] = v3.getW(); m[3][3] = v4.getW();
}
// Addition
Matrix4 Matrix4::operator+(const Matrix4& other) const {
    Matrix4 result;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result.m[i][j] = m[i][j] + other.m[i][j];
        }
    }
    return result;
}

// Subtraction
Matrix4 Matrix4::operator-(const Matrix4& other) const {
    Matrix4 result;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result.m[i][j] = m[i][j] - other.m[i][j];
        }
    }
    return result;
}

// Scaling
Matrix4 Matrix4::operator*(const float scalar) const {
    Matrix4 result;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result.m[i][j] = m[i][j] * scalar;
        }
    }
    return result;
}

// Division
Matrix4 Matrix4::operator/(const float scalar) const {
    Matrix4 result;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result.m[i][j] = m[i][j] / scalar;
        }
    }
    return result;
}

// Compound assignment operators
Matrix4& Matrix4::operator+=(const Matrix4& other) {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            m[i][j] += other.m[i][j];
        }
    }
    return *this;
}

Matrix4& Matrix4::operator-=(const Matrix4& other) {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            m[i][j] -= other.m[i][j];
        }
    }
    return *this;
}

Matrix4& Matrix4::operator*=(const float scalar) {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            m[i][j] *= scalar;
        }
    }
    return *this;
}

Matrix4& Matrix4::operator/=(const float scalar) {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            m[i][j] /= scalar;
        }
    }
    return *this;
}

// Multiplication
Matrix4 Matrix4::operator*(const Matrix4& other) const {
    Matrix4 result;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result.m[i][j] = 0.0f;
            for (int k = 0; k < 4; ++k) {
                result.m[i][j] += m[i][k] * other.m[k][j];
            }
        }
    }
    return result;
}

// Multiplication with Vector4
Vector4 Matrix4::operator*(const Vector4& vec) const {
    float vecX = vec.getX(), vecY = vec.getY(), vecZ = vec.getZ(), vecW = vec.getW();
    return Vector4(
        m[0][0] * vecX + m[0][1] * vecY + m[0][2] * vecZ + m[0][3] * vecW,
        m[1][0] * vecX + m[1][1] * vecY + m[1][2] * vecZ + m[1][3] * vecW,
        m[2][0] * vecX + m[2][1] * vecY + m[2][2] * vecZ + m[2][3] * vecW,
        m[3][0] * vecX + m[3][1] * vecY + m[3][2] * vecZ + m[3][3] * vecW
    );
}

// Transpose
Matrix4 Matrix4::transpose() const {
    Matrix4 result;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result.m[i][j] = m[j][i];
        }
    }
    return result;
}

// LU Decomposition
void Matrix4::luDecomposition(Matrix4& L, Matrix4& U) const {
    L = Matrix4();
    U = *this;
    for (int i = 0; i < 4; ++i) {
        for (int j = i + 1; j < 4; ++j) {
            float factor = U.m[j][i] / U.m[i][i];
            L.m[j][i] = factor;
            for (int k = i; k < 4; ++k) {
                U.m[j][k] -= factor * U.m[i][k];
            }
        }
    }
    for (int i = 0; i < 4; ++i) {
        L.m[i][i] = 1.0f;
    }
}

// Cholesky Decomposition
void Matrix4::choleskyDecomposition(Matrix4& L) const {
    L = Matrix4();
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j <= i; ++j) {
            float sum = 0.0f;
            for (int k = 0; k < j; ++k) {
                sum += L.m[i][k] * L.m[j][k];
            }
            if (i == j) {
                L.m[i][j] = std::sqrt(m[i][i] - sum);
            }
            else {
                L.m[i][j] = (m[i][j] - sum) / L.m[j][j];
            }
        }
    }
}

// QR Decomposition
void Matrix4::qrDecomposition(Matrix4& Q, Matrix4& R) const {
    Q = Matrix4();
    R = *this;
    for (int i = 0; i < 4; ++i) {
        float norm = 0.0f;
        for (int j = 0; j < 4; ++j) {
            norm += R.m[j][i] * R.m[j][i];
        }
        norm = std::sqrt(norm);
        for (int j = 0; j < 4; ++j) {
            Q.m[j][i] = R.m[j][i] / norm;
        }
        for (int j = i + 1; j < 4; ++j) {
            float dot = 0.0f;
            for (int k = 0; k < 4; ++k) {
                dot += Q.m[k][i] * R.m[k][j];
            }
            for (int k = 0; k < 4; ++k) {
                R.m[k][j] -= dot * Q.m[k][i];
            }
        }
    }
}

// Singular Value Decomposition (SVD)
void Matrix4::svdDecomposition(Matrix4& U, Matrix4& S, Matrix4& V) const {
    // Placeholder implementation for SVD
    // Replace this with a proper SVD algorithm
    U = Matrix4();
    S = Matrix4();
    V = Matrix4();
}

// Print
void Matrix4::print() const {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            std::cout << m[i][j] << " ";
        }
        std::cout << std::endl;
    }
}


// Overload stream insertion operator
std::ostream& operator<<(std::ostream& os, const Matrix4& mat) {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            os << mat.m[i][j] << " ";
        }
        os << std::endl;
    }
    return os;
}

// Overload stream extraction operator
std::istream& operator>>(std::istream& is, Matrix4& mat) {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            is >> mat.m[i][j];
        }
    }
    return is;
}

// Overload stream extraction operator for string input
std::istringstream& operator>>(std::istringstream& iss, Matrix4& mat) {
    char comma;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (!(iss >> mat.m[i][j])) {
                iss.setstate(std::ios::failbit);
                return iss;
            }
            if (i != 3 || j != 3) {
                if (!(iss >> comma) || comma != ',') {
                    iss.setstate(std::ios::failbit);
                    return iss;
                }
            }
        }
    }
    return iss;
}
// Inverse using LU Decomposition
Matrix4 Matrix4::inverse() const {
    Matrix4 L, U, inv;
    luDecomposition(L, U);

    // Invert L
    for (int i = 0; i < 4; ++i) {
        inv.m[i][i] = 1.0f / L.m[i][i];
        for (int j = i + 1; j < 4; ++j) {
            float sum = 0.0f;
            for (int k = i; k < j; ++k) {
                sum += L.m[j][k] * inv.m[k][i];
            }
            inv.m[j][i] = -sum / L.m[j][j];
        }
    }

    // Invert U
    for (int i = 3; i >= 0; --i) {
        inv.m[i][i] = 1.0f / U.m[i][i];
        for (int j = i - 1; j >= 0; --j) {
            float sum = 0.0f;
            for (int k = j + 1; k <= i; ++k) {
                sum += U.m[j][k] * inv.m[k][i];
            }
            inv.m[j][i] = -sum / U.m[j][j];
        }
    }

    return inv.transpose();
}

Matrix4 Matrix4::rotation(float angle, int axis) {
    switch (axis) {
    case ID_AXIS_X:
        return Matrix4::rotationX(angle);
        break;
    case ID_AXIS_Y:
        return Matrix4::rotationY(angle);
        break;
    case ID_AXIS_Z:
        return Matrix4::rotationZ(angle);
        break;
    }
}
// Rotation matrices
Matrix4 Matrix4::rotationX(float angle) {
    Matrix4 result;
    float c = std::cos(angle);
    float s = std::sin(angle);
    result.m[1][1] = c;
    result.m[1][2] = -s;
    result.m[2][1] = s;
    result.m[2][2] = c;
    return result;
}

Matrix4 Matrix4::rotationY(float angle) {
    Matrix4 result;
    float c = std::cos(angle);
    float s = std::sin(angle);
    result.m[0][0] = c;
    result.m[0][2] = s;
    result.m[2][0] = -s;
    result.m[2][2] = c;
    return result;
}

Matrix4 Matrix4::rotationZ(float angle) {
    Matrix4 result;
    float c = std::cos(angle);
    float s = std::sin(angle);
    result.m[0][0] = c;
    result.m[0][1] = -s;
    result.m[1][0] = s;
    result.m[1][1] = c;
    return result;
}

// Scaling matrix
Matrix4 Matrix4::scaling(const Vector3& vec) {
    Matrix4 result = identity();
    result.m[0][0] = vec.x;
    result.m[1][1] = vec.y;
    result.m[2][2] = vec.z;
    return result;
}

// Translation matrix
Matrix4 Matrix4::translate(const Vector3& vec) {
    Matrix4 result = identity();
    result.m[0][3] = vec.x;
    result.m[1][3] = vec.y;
    result.m[2][3] = vec.z;
    return result;
}

// Identity matrix
Matrix4 Matrix4::identity() {
    Matrix4 result;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result.m[i][j] = (i == j) ? 1.0f : 0.0f;
        }
    }
    return result;
}


// Function to check if the matrix is symmetric
bool Matrix4::isSymmetric() const {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (m[i][j] != m[j][i]) {
                return false;
            }
        }
    }
    return true;
}

// Function to check if the matrix is positive definite
bool Matrix4::isPositiveDefinite() const {
    // Check if all leading principal minors are positive
    for (int i = 1; i <= 4; ++i) {
        Matrix4 subMat;
        for (int j = 0; j < i; ++j) {
            for (int k = 0; k < i; ++k) {
                subMat.m[j][k] = m[j][k];
            }
        }
        if (subMat.determinant() <= 0) {
            return false;
        }
    }
    return true;
}

// Function to check if the matrix is invertible
bool Matrix4::isInvertible() const {
    return determinant() != 0;
}

// Function to determine the decomposition method
void Matrix4::determineDecompositionMethod() const {
    if (isSymmetric() && isPositiveDefinite()) {
        std::cout << "Use Cholesky decomposition." << std::endl;
        return;
    }

    if (isInvertible()) {
        std::cout << "Use LU decomposition." << std::endl;
        return;
    }

    std::cout << "Use QR decomposition." << std::endl;
}

// Function to calculate the determinant
float Matrix4::determinant() const {
    float det = 0.0f;
    for (int i = 0; i < 4; ++i) {
        Matrix4 subMat;
        for (int j = 1; j < 4; ++j) {
            int subCol = 0;
            for (int k = 0; k < 4; ++k) {
                if (k == i) continue;
                subMat.m[j - 1][subCol] = m[j][k];
                ++subCol;
            }
        }
        det += (i % 2 == 0 ? 1 : -1) * m[0][i] * subMat.determinant();
    }
    return det;
}

Vector3 Matrix4::getRow(int i) const {
    return Vector3(m[i][0], m[i][1], m[i][2]);
}
Vector3 Matrix4::getCol(int i) const {
    return Vector3(m[0][i], m[1][i], m[2][i]);
}

// Function to calculate the trace
float Matrix4::trace() const {
    float trace = 0.0f;
    for (int i = 0; i < 4; ++i) {
        trace += m[i][i];
    }
    return trace;
}

// Function to calculate the L1 norm
float Matrix4::normL1() const {
    float norm = 0.0f;
    for (int j = 0; j < 4; ++j) {
        float colSum = 0.0f;
        for (int i = 0; i < 4; ++i) {
            colSum += std::abs(m[i][j]);
        }
        norm = std::max(norm, colSum);
    }
    return norm;
}

// Function to calculate the L2 norm
float Matrix4::normL2() const {
    float norm = 0.0f;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            norm += m[i][j] * m[i][j];
        }
    }
    return std::sqrt(norm);
}

// Function to calculate the Linf norm
float Matrix4::normLinf() const {
    float norm = 0.0f;
    for (int i = 0; i < 4; ++i) {
        float rowSum = 0.0f;
        for (int j = 0; j < 4; ++j) {
            rowSum += std::abs(m[i][j]);
        }
        norm = std::max(norm, rowSum);
    }
    return norm;
}


Matrix4 Matrix4::irit_inverse() const{
    double mat[4][4];
    double res[4][4];
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            mat[i][j] = m[i][j];
        }
    }
    Matrix4 result;
    CGSkelInverseMatrix(mat, res);
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result.m[i][j] = res[i][j];
        }
    }
    return result;
}
