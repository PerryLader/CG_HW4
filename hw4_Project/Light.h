#ifndef LIGHT_H
#define LIGHT_H
#include "Vector4.h"
#include "Matrix4.h"
#include "Vector3.h"
#include "ColorGC.h"
#pragma once
typedef enum
{
    LIGHT_SCENE_SPEC = -2,
    LIGHT_ID_AMBIENT = -1,
    LIGHT_ID_1 = 0,
    LIGHT_ID_2,
    LIGHT_ID_3,
    LIGHT_ID_4,
    LIGHT_ID_5,
    LIGHT_ID_6,
    LIGHT_ID_7,
    LIGHT_ID_8,
    MAX_LIGHT
} LightID;

typedef  enum
{
    LIGHT_TYPE_DIRECTIONAL,
    LIGHT_TYPE_POINT,
    LIGHT_TYPE_SPOT
} LightType;

typedef  enum
{
    LIGHT_SPACE_VIEW,
    LIGHT_SPACE_LOCAL
} LightSpace;



class LightParams
{
public:

    //light enabled
    bool enabled;
    //type directional,point,spot
    LightType type;
    //local or view space
    LightSpace space;

    //color 0-255 RGB
    int colorR;
    int colorG;
    int colorB;

    ColorGC getColor() const{
        return ColorGC(colorR, colorG, colorB);
    }
    //position
    double posX;
    double posY;
    double posZ;

    Vector3 getPos() const {
        return Vector3(posX, posY, posZ);
    }
    //direction
    double dirX;
    double dirY;
    double dirZ;

    Vector3 getDir() const {
        return Vector3(dirX, dirY, dirZ);
    }
    //coefficiants
    double Kdiff;
    double Kspec;
    double Ipower;

    Vector3 transform(const Matrix4& mat){
        Vector3 transformed_loc = (mat * Vector4(posX, posY, posZ,1)).toVector3() ;
        posX = transformed_loc.x;
        posY = transformed_loc.y;
        posZ = transformed_loc.z;
        Vector3 transformed_dir = (mat * Vector4(dirX, dirY, dirZ, 1)).toVector3();
        dirX = transformed_dir.x;
        dirY = transformed_dir.y;
        dirZ = transformed_dir.z;
    }
    LightParams():
	enabled(false),type(LIGHT_TYPE_DIRECTIONAL),space(LIGHT_SPACE_VIEW),
	colorR(255),colorG(255),colorB(255),posX(0),posY(0),posZ(0),
	dirX(0),dirY(0),dirZ(0), Kdiff(0), Kspec(0), Ipower(0)
    {}

protected:
private:
};
#endif //LIGHT_H