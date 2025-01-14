//#include "BezierInterpolator.h"
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
//void BezierInterpolator::setOrder(int n) {
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
//template <typename T> T BezierInterpolator::interpolate(const std::vector<T>& objList, double t) const {
//    int n = objList.size() - 1;
//    T result = T();
//    for (int i = 0; i <= n; ++i)
//    {
//        result += getIthCoef(i, t) * objList[i];
//    }
//    return result;
//}