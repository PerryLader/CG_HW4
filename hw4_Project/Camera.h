#ifndef CAMERA_H
#define CAMERA_H

#include "Vector4.h"
#include "Matrix4.h"
#include "Modules.h"


class Camera {
public:
    // Constructor
    Camera(float nearPlane, float farPlane);

    // Function to set the view transformation matrix
    void setViewMatrix(const Matrix4& m_viewMatrix);

    // Function to get the view transformation matrix
    const Matrix4& getViewMatrix() const;

    // Function to set the camera using LookAt
    void lookAt(const Vector3& eye, const Vector3& target, const Vector3& up);

    virtual const Matrix4& getProjectionMatrix() const {
        return m_projectionMatrix;
    }

    virtual void setFar(float farPlane) {
        this->m_farPlane = farPlane;
        setProjection();
    }

    void translate(const Matrix4& tMat);

    void right_side_translate(const Matrix4& tMat);

    // Function to set orthogonal projection

    virtual bool isPerspective() const = 0;

    virtual Camera* clone() const = 0;

protected:
    virtual void setProjection() = 0;
    Matrix4 m_projectionMatrix;
    Matrix4 m_viewMatrix;
    float m_nearPlane;
    float m_farPlane;
};

class PerspectiveCamera :public Camera
{
private:
    float m_fov;//field of view on the y 

    void setProjection() override {
        double fovRad = m_fov * (3.14159265358979323846 / 180.0);

        // Calculate scale factors
        float tanHalfFovY = std::tan(fovRad / 2.0);
        float range = m_farPlane - m_nearPlane;

        // Calculate matrix elements
        float m00 = 1.0 / tanHalfFovY;
        float m11 = m00;
        float m22 = -(m_farPlane + m_nearPlane) / range;
        float m23 = -2.0 * m_farPlane * m_nearPlane / range;
        double m32 = -1.0;

        // Create the perspective matrix using the Matrix4 constructor
        this->m_projectionMatrix = Matrix4(
            m00, 0.0, 0.0, 0.0,
            0.0, -m11, 0.0, 0.0,
            0.0, 0.0, m22, m23,
            0.0, 0.0, m32, 0.0
        );
    }
public:
    PerspectiveCamera(float fovY, float nearPlane, float farPlane) :Camera(nearPlane, farPlane),
        m_fov(fovY) {
        setProjection();
    }
    virtual Camera* clone() const override {
        return new PerspectiveCamera(*this);
    }
    bool isPerspective() const override { return true; }
};

class OrthogonalCamera :public Camera
{
private:
    float m_left;
    float m_bottom;
    float m_right;
    float m_top;
    float m_theta;
    float m_phi;

    void setProjection() override {
        // Orthogonal projection matrix
        m_projectionMatrix = Matrix4(
            (2.0f / (m_right - m_left)), 0, 0, -(m_right + m_left) / (m_right - m_left),
            0, 2.0f / (m_top - m_bottom), -(m_top + m_bottom) / (m_top - m_bottom), 0,
            0, 0, -2.0f / (m_farPlane - m_nearPlane), -(m_farPlane + m_nearPlane) / (m_farPlane - m_nearPlane),
            0, 0, 0, 1.0f);
    }

public:
    OrthogonalCamera(const Vector3& LBN, const Vector3& RTF) :Camera(LBN.z, RTF.z),
        m_left(LBN.x),
        m_right(RTF.x),
        m_top(RTF.y),
        m_bottom(LBN.y) {
        setProjection();
    }
    virtual Camera* clone() const override {
        return new OrthogonalCamera(*this);
    }
    bool isPerspective() const override { return false; }
};





#endif // CAMERA_H