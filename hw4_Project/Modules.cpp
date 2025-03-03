#include "Modules.h"

int convertClipToScreen(float clipCoord, float halfSize){
    return (int)round((clipCoord * halfSize) + halfSize);
}