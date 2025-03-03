#include "Camera.h"
#include <cmath>

// Constructor
Camera::Camera(float nearPlane, float farPlane) :m_nearPlane(nearPlane), m_farPlane(farPlane)
{
    lookAt(Vector3(0, 0, -3), Vector3(0, 0, 0), Vector3(0, 1, 0));
}

// Function to set the view transformation matrix
void Camera::setViewMatrix(const Matrix4& m_viewMatrix) {
    this->m_viewMatrix = m_viewMatrix;
}

// Function to get the view transformation matrix
const Matrix4& Camera::getViewMatrix() const {
    return m_viewMatrix;
}

// Function to set the camera using LookAt
void Camera::lookAt(const Vector3& eye, const Vector3& target, const Vector3& up) {
    const Vector3 zAxis = (eye - target).normalized();
    const Vector3 xAxis = Vector3::cross(zAxis, up).normalized();
    const Vector3 yAxis = Vector3::cross(xAxis, zAxis);

    Matrix4 orientation = Matrix4(
        xAxis.x, xAxis.y, xAxis.z, 0,
        yAxis.x, yAxis.y, yAxis.z, 0,
        zAxis.x, zAxis.y, zAxis.z, 0,
        0, 0, 0, 1
    );

    Matrix4 translation = Matrix4::translate(-eye);
    m_viewMatrix = orientation * translation;
}

void Camera::translate(const Matrix4& tMat) {
    this->m_viewMatrix = tMat * this->m_viewMatrix;
}

void Camera::right_side_translate(const Matrix4& tMat) {
    this->m_viewMatrix = this->m_viewMatrix * tMat;
}

