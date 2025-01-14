#include "Camera.h"
#include <cmath>

// Constructor
Camera::Camera(float nearPlane, float farPlane) :m_nearPlane(nearPlane), m_farPlane(farPlane) 
{    
   // setOrthogonal(Vector3(-1,1,0), Vector3(1, -1, 5), 0, 0);
    //setPerspective(60, 1, 1, 5);
    lookAt(Vector3(0, 0,-3), Vector3(0, 0, 0), Vector3(0,1,0));
}

// Function to set the view transformation matrix
void Camera::setViewMatrix(const Matrix4& viewMatrix) {
    this->viewMatrix = viewMatrix;
}

// Function to get the view transformation matrix
const Matrix4& Camera::getViewMatrix() const {
    return viewMatrix;
}




// Function to set the camera using LookAt
void Camera::lookAt(const Vector3& eye, const Vector3& target, const Vector3& up) {
    const Vector3 zAxis = (eye - target).normalized();
    const Vector3 xAxis = Vector3::cross(zAxis,up ).normalized();
    const Vector3 yAxis = Vector3::cross(xAxis,zAxis);

    orientation = Matrix4(
        xAxis.x, xAxis.y, xAxis.z, 0,
        yAxis.x, yAxis.y, yAxis.z, 0,
        zAxis.x, zAxis.y, zAxis.z, 0,
        0, 0, 0, 1
    );

    translation = Matrix4::translate(-eye);
    camera_pos = eye;
    viewMatrix = orientation * translation;
}

// Function to set orthogonal projection


// Function to set perspective projection


void Camera::orientate(const Matrix4& tMat) {
    orientation = orientation * tMat;
    this->viewMatrix = orientation * translation;
}

void Camera::translate(const Matrix4& tMat) {
    this->viewMatrix = tMat * this->viewMatrix;
}

