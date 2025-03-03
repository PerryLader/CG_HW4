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
    using SetType = std::set<GData>;
    using BufferType = std::unordered_map<int, SetType>;

    GBuffer(size_t width, size_t height) : width_(width), height_(height) {}

    ~GBuffer() = default;

    void allocateBBox(size_t x1, size_t y1, size_t x2, size_t y2) {
        size_t totalSize = (x2 - x1 + 1) * (y2 - y1 + 1);
        buffer_.reserve(totalSize);
    }

    void push(size_t x, size_t y, const GData& data) {
        if (x < width_ && y < height_)
            buffer_[getKey(x, y)].insert(data);
    }

    SetType& get(size_t x, size_t y) {
        auto it = buffer_.find(getKey(x, y));
        if (it != buffer_.end()) {
            return it->second;
        }
        return emptySet_;
    }

    const SetType& get(size_t x, size_t y) const {
        auto it = buffer_.find(getKey(x, y));
        if (it != buffer_.end()) {
            return it->second;
        }
        return emptySet_;
    }
    std::vector<std::vector<std::reference_wrapper<const std::pair<const int, SetType>>>> getNParts(size_t n) const {
        std::vector<std::vector<std::reference_wrapper<const std::pair<const int, SetType>>>> parts(n);
        size_t i = 0;
        for (const auto& entry : buffer_) {
            parts[i % n].push_back(std::cref(entry));
            ++i;
        }
        return parts;
    }
    size_t getWidth() const { return width_; }
    size_t getHeight() const { return height_; }
private:
    size_t width_;
    size_t height_;
    BufferType buffer_;
    SetType emptySet_;
};