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
    void setViewMatrix(const Matrix4& viewMatrix);

    // Function to get the view transformation matrix
    const Matrix4& getViewMatrix() const;      

    // Function to set the camera using LookAt
    void lookAt(const Vector3& eye, const Vector3& target, const Vector3& up);

    virtual const Matrix4& getProjectionMatrix() const {
        throw;
        return Matrix4();
    };

    virtual void setFar(float farPlane) {};
    void Camera::orientate(const Matrix4& tMat);

    void Camera::translate(const Matrix4& tMat);
    
    // Function to set orthogonal projection
    virtual void setOrthogonal() { throw; };

    // Function to set perspective projection
    virtual void setPerspective() { throw; };

    virtual bool isPerspective() const { throw; };

    Vector3 getLocation() const{ return camera_pos; };
protected:
    Matrix4 viewMatrix;
    Matrix4 orientation;
    Matrix4 translation;
    Vector3 camera_pos;
    float m_nearPlane;
    float m_farPlane;
};

class PerspectiveCamera :public Camera
{
private:
    Matrix4 m_projectionMatrix;    
    float m_fov;//field of view on the y 
    float m_aspect;//aspect
public:
    PerspectiveCamera(float fovY, float aspect, float nearPlane, float farPlane) :Camera(nearPlane,farPlane),
        m_projectionMatrix(Matrix4::identity()),m_fov(fovY),m_aspect(aspect) {}

    void setPerspective() override {
        double fovRad = m_fov * (3.14159265358979323846 / 180.0);

        // Calculate scale factors
        double tanHalfFovY = std::tan(fovRad / 2.0);
        double range = m_farPlane - m_nearPlane;

        // Calculate matrix elements
        double m00 = 1.0 / (m_aspect * tanHalfFovY);
        double m11 = 1.0 / tanHalfFovY;
        double m22 = -(m_farPlane + m_nearPlane) / range;
        double m23 = -2.0 * m_farPlane * m_nearPlane / range;
        double m32 = -1.0;

        // Create the perspective matrix using the Matrix4 constructor
        this->m_projectionMatrix = Matrix4(
            m00, 0.0, 0.0, 0.0,
            0.0, -m11, 0.0, 0.0,
            0.0, 0.0, m22, m23,
            0.0, 0.0, m32, 0.0
        );
    }

    void setFar(float farPlane)override
    {
        this->m_farPlane = farPlane;
        setPerspective();
    }
    // Function to get the projection matrix
    const Matrix4& getProjectionMatrix() const override
    {
        return m_projectionMatrix;
    }

    bool isPerspective() const{ return true; }
};

class OrthogonalCamera :public Camera
{
private:
    Matrix4 m_projectionMatrix;
    float m_left ;
    float m_bottom ;    
    float m_right ;
    float m_top ;
    float m_theta;
    float m_phi;

public:
    OrthogonalCamera(const Vector3& LBN, const Vector3& RTF, float theta, float phi) :Camera(LBN.z, RTF.z),
        m_projectionMatrix(Matrix4::identity()),
        m_left(LBN.x),
        m_right(RTF.x),
        m_top(RTF.y),
        m_bottom(LBN.y) {}
        
    void setOrthogonal() override{      
       
        // Orthogonal projection matrix
        m_projectionMatrix = Matrix4(
            (2.0f / (m_right - m_left)), 0, 0, -(m_right + m_left) / (m_right - m_left),
            0, 2.0f / (m_top - m_bottom), -(m_top + m_bottom) / (m_top - m_bottom), 0,
            0, 0, -2.0f / (m_farPlane - m_nearPlane), -(m_farPlane + m_nearPlane) / (m_farPlane - m_nearPlane),
            0, 0, 0, 1.0f);
    }
    

    void setFar(float farPlane)override
    {
        this->m_farPlane = farPlane;
        setOrthogonal();
    }
    // Function to get the projection matrix
    const Matrix4& getProjectionMatrix() const override
    {
        return m_projectionMatrix;
    }
    bool isPerspective() const { return false; }

};





#endif // CAMERA_H
