#ifndef COMMANDS_H
#define COMMANDS_H

#include "Scene.h"


class ScreenCommand {
public:
    ScreenCommand(int width, int height) : screenWidth(width), screenHeigth(height) {}
    virtual ~ScreenCommand(){}
    virtual void execute(Scene& scene) = 0;
protected:
    int screenWidth;
    int screenHeigth;
};

class RenderCommand : public ScreenCommand {
public:
    virtual ~RenderCommand() {}
    RenderCommand(int width, int height, RenderMode& rd_mode) : ScreenCommand(width, height), rd_mode(rd_mode){}
    virtual void execute(Scene& scene) override {
        scene.render(screenWidth, screenHeigth, rd_mode);
    }
protected:
    RenderMode& rd_mode;
};

class TransformationCommand : public ScreenCommand {
public:
    TransformationCommand(int width, int height, int dx, int dy,
        float aspectRatio,int action, int axis,int tSpace, 
        float sensitivity, float depth)
        : ScreenCommand(width,height)
        , dx(dx), dy(dy), aspectRatio(aspectRatio),
        action(action), axis(axis), tSpace(tSpace), sensitivity(sensitivity) , depth(depth) {}

    void execute(Scene& scene) override {
        scene.handleTransformationAction(dx,dy, aspectRatio,action, axis, sensitivity,tSpace, screenWidth, screenHeigth,depth);
    }
protected:
    int dx;
    int dy;
    float aspectRatio;
    int action;
    int axis;
    int tSpace;
    float sensitivity;
    float depth;
};
#endif //COMMANDS_H

//
//class ViewCommand : public ICommand {
//public:
//    ViewCommand(float aspectRatio, int axis, float sensitivity)
//        : ref_point(ref_point), movement(movement), aspectRatio(aspectRatio), axis(axis), sensitivity(sensitivity) {}
//
//    void execute(Scene& scene) override {
//        scene.handleViewAction(ref_point, movement, aspectRatio, /*action=*/1, axis, /*perspective=*/false, sensitivity, /*space=*/0);
//    }
//
//private:
//    int flag;
//    int axis;
//    float sensitivity;
//};
//
//class ViewableObjectsCommand : public ICommand {
//public:
//    ViewableObjectsCommand(float aspectRatio, int axis, float sensitivity)
//        : ref_point(ref_point), movement(movement), aspectRatio(aspectRatio), axis(axis), sensitivity(sensitivity) {}
//
//    void execute(Scene& scene) override {
//        scene.handleViewObjectsAction(ref_point, movement, aspectRatio, /*action=*/1, axis, /*perspective=*/false, sensitivity, /*space=*/0);
//    }
//
//private:
//    int flag;
//    int axis;
//    float sensitivity;
//};
//class SetColorsCommand : public ICommand {
//public:
//    SetColorsCommand(const Vector3& ref_point, const Vector3& movement, float aspectRatio, int axis, float sensitivity)
//        : ref_point(ref_point), movement(movement), aspectRatio(aspectRatio), axis(axis), sensitivity(sensitivity) {}
//
//    void execute(Scene& scene) override {
//        scene.handleColorAction(ref_point, movement, aspectRatio, /*action=*/1, axis, /*perspective=*/false, sensitivity, /*space=*/0);
//    }
//
//private:
//    Vector3 ref_point;
//    Vector3 movement;
//    float aspectRatio;
//    int axis;
//    float sensitivity;
//};

