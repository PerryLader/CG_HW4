#pragma once
#include <set>
#include <unordered_map>
#include <iostream>
#include <memory>
#include <cmath>
#include <mutex>
#include "ColorGC.h"
#include "Vector3.h"
// Define gData structure
#include <utility>
#include <functional>

enum pixType {
    FROM_LINE,
    FROM_POLYGON,
    FROM_BACKGROUND
};

class PolygonGC;
class ColorGC;

struct GData {
    const PolygonGC* polygon;
    ColorGC pixColor;
    Vector3 pixPos;
    Vector3 pixNorm;
    static constexpr double epsilon = 5e-4;

    GData(const PolygonGC* p, ColorGC c, Vector3 pos, Vector3 norm)
        : polygon(p), pixColor(c), pixPos(pos), pixNorm(norm){}

    bool operator<(const GData& other) const {
        return (std::abs(pixPos.z - other.pixPos.z) > GData::epsilon) && pixPos.z < other.pixPos.z;
    }
};

class GBuffer {
private:
    int getKey(int x, int y) const {
        return y * width_ + x;
    }

public:
    using SetType = std::vector<GData>;

    using BufferType = std::unordered_map<int, SetType>;

    GBuffer(size_t width, size_t height) : width_(width), height_(height), bufferDemo_(height, std::vector<SetType*>(width, nullptr)) {
    }

    ~GBuffer() {
        for (auto& elem : freeing_list_)
            delete elem.second;
    };

    void push(size_t x, size_t y, const GData& data) {

        if (bufferDemo_[y][x] == nullptr) {
            bufferDemo_[y][x] = new SetType();
            freeing_list_.push_back(std::pair<int , SetType*>(getKey(x,y), bufferDemo_[y][x]));
        }
        bufferDemo_[y][x]->push_back(data);
    }

    std::vector<std::vector<std::pair<int, SetType*>*>> getNParts(size_t n) {
        std::vector<std::vector<std::pair<int, SetType*>*>> parts(n);
        size_t k = 0;
        for (auto& elem : freeing_list_) {
            parts[k % n].push_back(&elem);
            k++;
        }
        return parts;
    }
    size_t getWidth() const { return width_; }
    size_t getHeight() const { return height_; }
private:
    size_t width_;
    size_t height_;
    std::vector<std::vector<SetType*>> bufferDemo_;
    std::vector<std::pair<int ,SetType*>> freeing_list_;
    SetType emptySet_;
};