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
    std::mutex& getMutex(int key) {
    size_t index = std::hash<int>{}(key) % mutexes.size();
    return mutexes[index];
    }

public:
    using SetType = std::vector<GData>;

    using BufferType = std::unordered_map<int, SetType>;

    GBuffer(size_t width, size_t height) : width_(width), height_(height), bufferDemo_(height+1, std::vector<SetType*>(width+1, nullptr)), mutexes(64) {
    }

    ~GBuffer() {
        for (auto& elem : freeing_list_)
            delete elem;
    };

    void push(size_t x, size_t y, const GData& data) {
        int key = getKey(x, y);
        std::unique_lock<std::mutex> lock(getMutex(getKey(x,y)));
        if (bufferDemo_[y][x] == nullptr) {
            bufferDemo_[y][x] = new SetType();
        }
        bufferDemo_[y][x]->push_back(data);
    }

    std::vector<std::vector<std::pair<int, SetType*>>> getNParts(size_t n) {
        std::vector<std::vector<std::pair<int, SetType*>>> parts(n);
        size_t k = 0; 
        for (int y = 0; y < height_; y++) {
            for (int x = 0; x < width_; x++) if (bufferDemo_[y][x] != nullptr) {
                int i = bufferDemo_[y][x]->size();
                freeing_list_.push_back(bufferDemo_[y][x]);
                parts[k % n].push_back(std::pair<int, SetType*>(getKey(x, y), bufferDemo_[y][x]));
                k++;
            }
        }
        return parts;
    }
    size_t getWidth() const { return width_; }
    size_t getHeight() const { return height_; }
private:
    size_t width_;
    size_t height_;
    std::vector<std::vector<SetType*>> bufferDemo_;
    std::vector<SetType*> freeing_list_;
    SetType emptySet_;
    std::mutex freeing_list_mutex; // Add a mutex for the freeing_list_
    std::vector<std::mutex> mutexes; // Fixed number of mutexes for striped locking
};