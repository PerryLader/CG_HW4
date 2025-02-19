#include "BezierInterpolator.h"
//
//// Function to compute combinations using precomputed factorials
//static long long combination(int n, int k, const std::vector<long long>& factorials) {
//    return factorials[n] / (factorials[k] * factorials[n - k]);
//}
//
//// Function to precompute factorials up to n
//void BezierInterpolator::precomputeFactorials(int n) {
//    factorials.clear();
//    factorials.resize(n + 1);
//    factorials[0] = 1;
//    for (int i = 1; i <= n; ++i) {
//        factorials[i] = factorials[i - 1] * i;
//    }
//}
//
//// Function to precompute coefficients up to order n
//void BezierInterpolator::precomputeCoefs(int n) {
//    coefs.clear();
//    coefs.resize(n + 1);
//    for (int i = 0; i <= n; ++i) {
//        coefs[i] = combination(n, i, factorials);
//    }
//}
//
//// Method to set the order of the Bezier curve
//BezierInterpolator::BezierInterpolator(int n) {
//    precomputeFactorials(n);
//    precomputeCoefs(n);
//}
//
//// Method to get the ith coefficient at time t
//double BezierInterpolator::getIthCoef(int i, double t) const {
//    if (i >= 0 && i < coefs.size()) {
//        return coefs[i] * std::pow(1 - t, coefs.size() - 1 - i) * std::pow(t, i);
//    }
//    return 0.0; // Return 0 if index is out of bounds
//}
//


BSplineInterpolator::BSplineInterpolator(int n, const std::vector<int>& keyframeCounts, bool linear) {
    int maxSegmentLength = *std::max_element(keyframeCounts.begin(), keyframeCounts.end());
    degree = linear ? 1 : (maxSegmentLength - 1); // Set degree to 1 for linear, otherwise to max segment length - 1 for Bezier behavior
    generateKnotsWithContinuity(keyframeCounts, linear);
    cumulativeKeyframeCounts = calculateCumulativeKeyframeCounts(keyframeCounts);
}

void BSplineInterpolator::generateKnotsWithContinuity(const std::vector<int>& keyframeCounts, bool linear) {
    int totalFrames = std::accumulate(keyframeCounts.begin(), keyframeCounts.end(), 0);
    int numKnots = totalFrames + degree + 1;

    knots.resize(numKnots);

    double cumulativeTime = 0.0;
    int index = 0;

    for (int setLength : keyframeCounts) {
        double setTimeIncrement = static_cast<double>(setLength) / totalFrames;
        for (int j = 0; j < setLength; ++j) {
            knots[index++] = cumulativeTime;
        }
        cumulativeTime += setTimeIncrement;
    }

    // Ensure uniform distribution of knots at the boundaries
    for (int i = 0; i <= degree; ++i) {
        knots[numKnots - 1 - i] = 1.0;
        knots[i] = 0.0;
    }
}

std::vector<double> BSplineInterpolator::calculateCumulativeKeyframeCounts(const std::vector<int>& keyframeCounts) {
    std::vector<double> cumulativeKeyframeCounts(keyframeCounts.size() + 1, 0.0);
    std::partial_sum(keyframeCounts.begin(), keyframeCounts.end(), cumulativeKeyframeCounts.begin() + 1);
    return cumulativeKeyframeCounts;
}

int BSplineInterpolator::findSetIndex(double t) const {
    for (int i = 1; i < cumulativeKeyframeCounts.size(); ++i) {
        if (t < cumulativeKeyframeCounts[i] / cumulativeKeyframeCounts.back()) {
            return i - 1;
        }
    }
    return cumulativeKeyframeCounts.size() - 2;
}

double BSplineInterpolator::getBSplineBasis(int i, int k, double t) const {
    if (k == 0) {
        return (knots[i] <= t && t < knots[i + 1]) ? 1.0 : 0.0;
    }
    else {
        double coef1 = (t - knots[i]) / (knots[i + k] - knots[i]);
        double coef2 = (knots[i + k + 1] - t) / (knots[i + k + 1] - knots[i + 1]);

        double term1 = (knots[i + k] != knots[i]) ? coef1 * getBSplineBasis(i, k - 1, t) : 0.0;
        double term2 = (knots[i + k + 1] != knots[i + 1]) ? coef2 * getBSplineBasis(i + 1, k - 1, t) : 0.0;

        return term1 + term2;
    }
}
