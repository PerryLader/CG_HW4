#pragma once
#include <set>
#include <unordered_map>
#include <iostream>
#include <memory>
#include <cmath>
#include <mutex>
#include "ColorGC.h"
#include "Vector3.h"

enum pixType {
    FROM_LINE,
    FROM_POLYGON,
    FROM_BACKGROUND
};

class PolygonGC;
class ColorGC;

// Define gData structure
struct GData {
    float z_indx;
    const PolygonGC* polygon;
    ColorGC pixColor;
    Vector3 pixPos;
    Vector3 pixNorm;
    pixType m_pixType;

    GData(float z, const PolygonGC* p, ColorGC c, Vector3 pos, Vector3 norm, pixType type)
        : z_indx(z), polygon(p), pixColor(c), pixPos(pos), pixNorm(norm), m_pixType(type) {}
};

// Custom allocator for gData
template <typename T>
class CustomAllocator {
public:
    using value_type = T;

    CustomAllocator() = default;

    template <typename U>
    constexpr CustomAllocator(const CustomAllocator<U>&) noexcept {}

    [[nodiscard]] T* allocate(std::size_t n) {
        if (auto p = static_cast<T*>(::operator new(n * sizeof(T)))) {
            return p;
        }
        throw std::bad_alloc();
    }

    void deallocate(T* p, std::size_t) noexcept {
        ::operator delete(p);
    }
};

template <typename T, typename U>
bool operator==(const CustomAllocator<T>&, const CustomAllocator<U>&) { return true; }

template <typename T, typename U>
bool operator!=(const CustomAllocator<T>&, const CustomAllocator<U>&) { return false; }

// Comparator with epsilon for gData
struct CompareZIndex {
    double epsilon = 5e-2;
    bool operator()(const GData& lhs, const GData& rhs) const {
        return std::abs(lhs.z_indx - rhs.z_indx) > epsilon && lhs.z_indx < rhs.z_indx;  // Ascending order
    }
};

// Custom hash function for std::pair<size_t, size_t>
struct PairHash {
    template <typename T1, typename T2>
    std::size_t operator()(const std::pair<T1, T2>& p) const {
        auto hash1 = std::hash<T1>{}(p.first);
        auto hash2 = std::hash<T2>{}(p.second);
        return hash1 ^ (hash2 << 1); // Combine the two hash values
    }
};

class GBuffer {
public:
    using SetType = std::set<GData, CompareZIndex, CustomAllocator<GData>>;

    GBuffer(size_t width, size_t height) : width_(width), height_(height), emptySet() {}

    ~GBuffer() = default;

    void allocateBBox(size_t x1, size_t y1, size_t x2, size_t y2) {
     //   std::lock_guard<std::mutex> lock(mutex_);
        for (size_t x = x1; x <= x2; ++x) {
            for (size_t y = y1; y <= y2; ++y) {
                buffer_[std::make_pair(x, y)];
            }
        }
    }

    void push(size_t x, size_t y, const GData& data) {
    //    std::lock_guard<std::mutex> lock(mutex_);
        if (x < width_ && y < height_) {
            buffer_[std::make_pair(x, y)].insert(data);
        }
    }

    SetType& get(size_t x, size_t y) {
     //   std::lock_guard<std::mutex> lock(mutex_);
        auto it = buffer_.find(std::make_pair(x, y));
        if (it != buffer_.end()) {
            return it->second;
        }
        return emptySet;
    }
    std::vector<std::unordered_map<std::pair<size_t, size_t>, std::reference_wrapper<GBuffer::SetType>, PairHash>> getNParts(size_t n) const {
        std::vector<std::unordered_map<std::pair<size_t, size_t>, std::reference_wrapper<GBuffer::SetType>, PairHash>> parts(n);
        size_t i = 0;
        for (auto& entry : buffer_) if(!entry.second.empty()){
            parts[i % n].emplace(entry.first, std::ref(entry.second));
            ++i;
        }
        return parts;
    }

    const SetType& get(size_t x, size_t y) const {
    //    std::lock_guard<std::mutex> lock(mutex_);
        auto it = buffer_.find(std::make_pair(x, y));
        if (it != buffer_.end()) {
            return it->second;
        }
        return emptySet;
    }

    size_t getWidth() const {
        return width_;
    }

    size_t getHeight() const {
        return height_;
    }

private:
    size_t width_;
    size_t height_;
    mutable std::unordered_map<std::pair<size_t, size_t>, SetType, PairHash> buffer_;
    mutable SetType emptySet;
//    mutable std::mutex mutex_;
};

