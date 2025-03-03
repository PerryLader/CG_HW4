#include "BezierInterpolator.h"

BSplineInterpolator::BSplineInterpolator(int N, int degree) : degree(degree >= N ? N - 1 : degree) {
    generateKnots(N);
}

void BSplineInterpolator::generateKnots(int N) {
    int numKnots = N + degree + 1;
    knots.resize(numKnots);

    // Clamped uniformly distributed knots
    for (int i = 0; i <= degree; ++i) {
        knots[i] = 0.0;
        knots[numKnots - 1 - i] = 1.0;
    }
    for (int i = degree + 1; i < numKnots - degree - 1; ++i) {
        knots[i] = static_cast<double>(i - degree) / (numKnots - 2 * degree - 1);
    }
}
double BSplineInterpolator::getBSplineBasis(int i, int k, double t) const {
    if (k == 0) {
        // Special handling for the last knot value
        if (t == knots.back()) {
            return (knots[i] <= t && t <= knots[i + 1]) ? 1.0 : 0.0;
        }
        else {
            return (knots[i] <= t && t < knots[i + 1]) ? 1.0 : 0.0;
        }
    }
    else {
        double coef1 = (t - knots[i]) / (knots[i + k] - knots[i]);
        double coef2 = (knots[i + k + 1] - t) / (knots[i + k + 1] - knots[i + 1]);

        double term1 = (knots[i + k] != knots[i]) ? coef1 * getBSplineBasis(i, k - 1, t) : 0.0;
        double term2 = (knots[i + k + 1] != knots[i + 1]) ? coef2 * getBSplineBasis(i + 1, k - 1, t) : 0.0;

        return term1 + term2;
    }
}
