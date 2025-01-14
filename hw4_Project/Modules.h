#pragma once
#include <iomanip>
#include "Vector3.h"
#include "ColorGC.h"
#include <vector>

class PolygonGC;
enum LightSourceType
{
    LightSourceType_DIRECTIONAL,
    LightSourceType_POINT,
    LightSourceType_SPOT
} ;

 
enum EdgeMode {
    VISIBLE = 0,
    NO_VISIBLE = 1,
    SILHOUTTE = 2
};

typedef struct GData {
    float z_indx;
    const PolygonGC* polygon;
    ColorGC pixColor;
    Vector3 pixPos;
    Vector3 pixNorm;
    GData* next;
} gData;


enum RENDER_FLAG {
    RENDER_WIREFRAME = 0,
    RENDER_SILOHETTE,
    RENDER_SHOW_P_C_NORMALS,
    RENDER_SHOW_P_D_NORMALS,
    RENDER_SHOW_V_C_NORMALS,
    RENDER_SHOW_V_D_NORMALS,
    RENDER_USE_V_C_NORMALS,
    RENDER_USE_P_C_NORMALS,
    RENDER_OBJ_BBOX,
    RENDER_POLYGONS_BBOX,
    RENDER_OVERRIDER_WIRE_COLOR,
    RENDER_OVERRIDER_NORMAL_COLOR,
    RENDER_BACKFACE_CULLED,
    RENDER_FLIPED_NORMALS,
    RENDER_DYNEMIC,
    RENDER_TO_PNG,
    RENDER_ADD_KEYFRAME,
    RENDER_KEYFRAMES,
};


enum class BG_MODE {
    SOLID = 0,
    STREACHED,
    REPEATED
};

enum class SHADE_MODE {
    NONE = 0,
    SOLID,
    GOUROUD,
    PHONG
};
//const uint32_t RENDER_SHAPE = 1 << 0;
//const uint32_t RENDER_POLYGONS_CALC_NORMALS = 1 << 1;
//const uint32_t RENDER_POLYGONS_NORMALS_FROM_DATA = 1 << 2;
//const uint32_t RENDER_CALC_VETICES_NORMALS =  1 << 3;
//const uint32_t RENDER_DATA_VETICES_NORMALS = 16;
//const uint32_t RENDER_OBJ_BBOX = 32;
//const uint32_t RENDER_POLYGONS_BBOX = 64;
//const uint32_t RENDER_OVERRIDER_WIRE_COLOR = 128;
//const uint32_t RENDER_OVERRIDER_NORMAL_COLOR = 256;
//const uint32_t REBDER_SILHOUTTE_COLOR = 512;

//  m_bgInfo.color = ColorGC(230,230,230);
//  m_bgInfo.mode = bgMode::SOLID;
//  m_bgInfo.pngPath[0] = '\0';
class RenderMode {
private:
    uint32_t flags;
    char BG_pngPath[1024];
    ColorGC BG_color;
    ColorGC WIRE_color;
    ColorGC NORMAL_color;
    bool getFlagValue(RENDER_FLAG flag) const { return (1 << flag) & flags; }
    void setFlagValue(RENDER_FLAG flag) { flags ^= (1 << flag); }
    void clearFlagValue(RENDER_FLAG flag) { flags &= ~(1 << flag); }
    BG_MODE bgMode;
    SHADE_MODE shadeMode;
public:
    int m_toPngRenderWidth, m_toPngRenderHeight;

    RenderMode():BG_color(ColorGC(240,240,240)/*Grey*/), WIRE_color(ColorGC(0, 0, 0)/*black*/),NORMAL_color(ColorGC(255, 192, 203)/*Pink*/ ){
        flags = 0;
        setRenderDynemic();        
        setWireframeFlag();
        setRenderBGSolidFlag();
        setRenderShadeSolidFlag();
        setBGPngPath("");
        m_toPngRenderWidth = m_toPngRenderHeight = 0;
    }
    bool getWireFrameFlag() const { return getFlagValue(RENDER_WIREFRAME); }
    bool getSilohetteFlag() const { return getFlagValue(RENDER_SILOHETTE); }
    bool getPolygonsShowCNormalFlag() const { return getFlagValue(RENDER_SHOW_P_C_NORMALS); }
    bool getPolygonsShowDNormalFlag() const { return getFlagValue(RENDER_SHOW_P_D_NORMALS); }
    bool getVertexShowCNormalFlag() const { return getFlagValue(RENDER_SHOW_V_C_NORMALS); }
    bool getVertexShowDNormalFlag() const { return getFlagValue(RENDER_SHOW_V_D_NORMALS); }
    bool getPolygonsUseCNormalFlag() const { return getFlagValue(RENDER_USE_P_C_NORMALS); }
    
    bool getVertexUseCNormalFlag() const { return getFlagValue(RENDER_USE_V_C_NORMALS); }
    
    bool getRenderObjBboxFlag() const { return getFlagValue(RENDER_OBJ_BBOX); }
    bool getRenderPolygonsBboxFlag()  const { return getFlagValue(RENDER_POLYGONS_BBOX); }
    bool getRenderOverrideWireColorFlag() const { return getFlagValue(RENDER_OVERRIDER_WIRE_COLOR); }
    bool getRenderOverrideNormalColorFlag() const { return getFlagValue(RENDER_OVERRIDER_NORMAL_COLOR); }
    bool getRenderToPNGFlag() const { return getFlagValue(RENDER_TO_PNG); }
    bool getRenderBGSolidFlag()  const { return bgMode == BG_MODE::SOLID; }
    bool getRenderBGStreachedFlag() const { return bgMode == BG_MODE::STREACHED; }
    bool getRenderBGRepeatFlag() const { return bgMode == BG_MODE::REPEATED; }
    bool getRenderShadeSolidFlag() const { return shadeMode == SHADE_MODE::SOLID; }
    bool getRenderShadeGouroudFlag() const { return shadeMode == SHADE_MODE::GOUROUD; }
    bool getRenderShadePhongFlag() const { return shadeMode == SHADE_MODE::PHONG; }
    bool getRenderShadeNoneFlag() const { return shadeMode == SHADE_MODE::NONE; }
    bool getRenderCulledFlag() const { return getFlagValue(RENDER_BACKFACE_CULLED); }
    bool getRenderWithFlipedNormalsFlag() const { return getFlagValue(RENDER_FLIPED_NORMALS); }
    bool getRenderDynemic() const { return getFlagValue(RENDER_DYNEMIC); }
    bool getRenderAddKeyFrame() const { return getFlagValue(RENDER_ADD_KEYFRAME); }
    bool getRenderKeyFrames() const { return getFlagValue(RENDER_KEYFRAMES); }
    SHADE_MODE getRenderShadeFlag() const { return shadeMode; }
    BG_MODE getRenderBGFlag() const { return bgMode; }

    void setWireframeFlag() { setFlagValue(RENDER_WIREFRAME); }
    void setSilohetteFlag() { setFlagValue(RENDER_SILOHETTE); }
    void setRenderPolygonsCalcNormalFlag() { setFlagValue(RENDER_SHOW_P_C_NORMALS); }
    void setRenderPolygonsNormalFromDataFlag() { setFlagValue(RENDER_SHOW_P_D_NORMALS); }
    void setRenderCalcVertivesNormalFlag() { setFlagValue(RENDER_SHOW_V_C_NORMALS); }
    void setRenderDataVertivesNormalFlag() { setFlagValue(RENDER_SHOW_V_D_NORMALS); }
    void setRenderObjBboxFlag() { setFlagValue(RENDER_OBJ_BBOX); }
    void setRenderPolygonsBboxFlag() { setFlagValue(RENDER_POLYGONS_BBOX); }
    void setRenderOverrideWireColorFlag() { setFlagValue(RENDER_OVERRIDER_WIRE_COLOR); }
    void setRenderOverrideNormalColorFlag() { setFlagValue(RENDER_OVERRIDER_NORMAL_COLOR); }
    void setRenderBGSolidFlag() { bgMode = BG_MODE::SOLID; }
    void setRenderBGStreachedFlag() { bgMode = BG_MODE::STREACHED; }
    void setRenderBGRepeatFlag() { bgMode = BG_MODE::REPEATED; }
    void setRenderShadeSolidFlag() { shadeMode = SHADE_MODE::SOLID; }
    void setRenderShadeGouroudFlag() { shadeMode = SHADE_MODE::GOUROUD; }
    void setRenderShadePhongFlag() { shadeMode = SHADE_MODE::PHONG; }
    void setRenderShadeNoneFlag() { shadeMode = SHADE_MODE::NONE; }
    void setRenderCulledFlag() { setFlagValue(RENDER_BACKFACE_CULLED); }
    void setRenderWithFlipedNormalsFlag(){ setFlagValue(RENDER_FLIPED_NORMALS); }
    void setPolygonsUseCNormalFlag(){ setFlagValue(RENDER_USE_P_C_NORMALS); }
    void setVertexUseCNormalFlag(){setFlagValue(RENDER_USE_V_C_NORMALS); }
    void setRenderDynemic() { setFlagValue(RENDER_DYNEMIC); }
    void setRenderToPNGFlag() { setFlagValue(RENDER_TO_PNG); }
    void setRenderAddKeyFrame() { setFlagValue(RENDER_ADD_KEYFRAME); }
    void setRenderKeyFrames() { setFlagValue(RENDER_KEYFRAMES); }

    const char* getBGPngPath() const{ return BG_pngPath; }
    void setBGPngPath(const char* path) { strcpy(BG_pngPath, path); }
    bool getHasBGPngPath() const{ return strcmp(BG_pngPath, ""); }
    ColorGC getBGColor() const{ return BG_color; }
    ColorGC getWireColor() const{ return WIRE_color; }
    ColorGC getNormalColor() const{ return NORMAL_color; }
    void setBGColor(const ColorGC& color) { BG_color = color; }
    void setWireColor(const ColorGC& color) { WIRE_color = color; }
    void setNormalColor(const ColorGC& color) { NORMAL_color = color; }
    void unSetAll() {flags = 0; strcpy(BG_pngPath, "");}
};


enum LineVectorIndex {
    SHAPES = 0,
    POLY_CALC_NORNAL = 1,
    POLY_DATA_NORNAL = 2,
    VERTICES_CALC_NORMAL = 3,
    VERTICES_DATA_NORMAL = 4,
    OBJ_BBOX = 5,
    POLY_BBOX = 6,
    LAST = 7
};

struct bgInfo {
    BG_MODE mode;
    char pngPath[1024];
    ColorGC color;
};

enum CAMERA_TYPE
{
    ORTHOGONAL = 0,
    PRESPECTIVE = 1,
    COUNT = 2
};