
#ifndef NCK_POLYNOMIAL_SOLVER_H
#define NCK_POLYNOMIAL_SOLVER_H

// Third degree solver from Daniel Wolf 
// post on stackoverflow

class PolynomialSolver {
public:
    // Solves the equation ax³+bx²+cx+d = 0 for x ϵ ℝ
    // and returns the first result in [0, 1] or null.
    static double SolveCubic(double a, double b, double c, double d);

    // Solves the equation ax² + bx + c = 0 for x ϵ ℝ
    // and returns the first result in [0, 1] or null.
    static double SolveQuadratic(double a, double b, double c);
};

#endif