
#include "nckPolynomialSolver.h"
#include "nckMathUtils.h"

static double Squared(double f) { return f * f; }

static double Cubed(double f) { return f * f * f; }

static double CubicRoot(double f) { return pow(f, 1.0 / 3.0); }

double PolynomialSolver::SolveCubic(double a, double b, double c, double d) {
    if (a == 0) return SolveQuadratic(b, c, d);
    if (d == 0) return 0;

    b /= a;
    c /= a;
    d /= a;
    double q = (3.0 * c - Squared(b)) / 9.0;
    double r = (-27.0 * d + b * (9.0 * c - 2.0 * Squared(b))) / 54.0;
    double disc = Cubed(q) + Squared(r);
    double term1 = b / 3.0;

    if (disc > 0) {
        double s = r + sqrt(disc);
        s = (s < 0) ? -CubicRoot(-s) : CubicRoot(s);
        double t = r - sqrt(disc);
        t = (t < 0) ? -CubicRoot(-t) : CubicRoot(t);

        double result = -term1 + s + t;
        if (result >= 0 && result <= 1) return result;
    }
    else if (disc == 0) {
        double r13 = (r < 0) ? -CubicRoot(-r) : CubicRoot(r);

        double result = -term1 + 2.0 * r13;
        if (result >= 0 && result <= 1) return result;

        result = -(r13 + term1);
        if (result >= 0 && result <= 1) return result;
    }
    else {
        q = -q;
        double dum1 = q * q * q;
        dum1 = acos(r / sqrt(dum1));
        double r13 = 2.0 * sqrt(q);

        double result = -term1 + r13 * cos(dum1 / 3.0);
        if (result >= 0 && result <= 1) return result;

        result = -term1 + r13 * cos((dum1 + 2.0 * M_PI) / 3.0);
        if (result >= 0 && result <= 1) return result;

        result = -term1 + r13 * cos((dum1 + 4.0 * M_PI) / 3.0);
        if (result >= 0 && result <= 1) return result;
    }

    return NAN;
}

double PolynomialSolver::SolveQuadratic(double a, double b, double c) {
    double result = (-b + sqrt(Squared(b) - 4 * a * c)) / (2 * a);
    if (result >= 0 && result <= 1) return result;

    result = (-b - sqrt(Squared(b) - 4 * a * c)) / (2 * a);
    if (result >= 0 && result <= 1) return result;

    return NAN;
}
