#pragma once
#include <vector>
#include <numeric>
#include <algorithm>

class BSplineInterpolator {
private:
    std::vector<double> knots;
    int degree;

    void generateKnots(int N);

public:
    BSplineInterpolator(int N, int degree);
    double getBSplineBasis(int i, int k, double t) const;

    template <typename T>
    T interpolate(const std::vector<T>& objList, double t) const {
        int n = objList.size() - 1;
        T result = T();
        result = result - result;
        for (int i = 0; i <= n; ++i) {
            result += objList[i] * getBSplineBasis(i, degree, t);
        }
        return result;
    }
};

