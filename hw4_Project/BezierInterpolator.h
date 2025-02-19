#include <vector>
#include <cmath>
#include <iostream>
#include <numeric>

class BSplineInterpolator
{
private:
    std::vector<double> knots;
    std::vector<double> cumulativeKeyframeCounts;
    int degree;

    void generateKnotsWithContinuity(const std::vector<int>& keyframeCounts, bool linear);
    std::vector<double> calculateCumulativeKeyframeCounts(const std::vector<int>& keyframeCounts);

public:
    BSplineInterpolator(int n, const std::vector<int>& keyframeCounts, bool linear);
    double getBSplineBasis(int i, int k, double t) const;
    int findSetIndex(double t) const;

    template <typename T>
    T interpolate(const std::vector<T>& objList, double t) const {
        int n = objList.size() - 1;
        T result = T();
        for (int i = 0; i <= n; ++i) {
            result += objList[i] * getBSplineBasis(i, degree, t);
        }
        return result;
    }
};

