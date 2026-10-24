#include "../include/ASMEDesign.h"
#include <cmath>
#include <algorithm>

#define PI 3.14159265358979323846

double ASMEDesign::calculateRequiredDiameter(double M, double T, double P, double Kb, double Kt, bool hasKeyway, const Material& mat) 
{
    M *= 1000.0;
    T *= 1000.0;
    P = std::abs(P); // ASME uses magnitude for column action

    // Calculate maximum allowable shear stress
    double tau_allow = std::min(0.30 * mat.Sy, 0.18 * mat.Sut);
    if (hasKeyway) {
        tau_allow *= 0.75;
    }

    // Numerical Bisection Solver
    auto evaluateASME = [&](double d) {
        double bending_term = Kb * M + (P * d) / 8.0;
        double torsion_term = Kt * T;
        double required_d3 = (16.0 / (PI * tau_allow)) * std::sqrt(bending_term * bending_term + torsion_term * torsion_term);
        
        // If current d^3 is less than the required d^3, constraint > 0 (fails)
        return required_d3 - (d * d * d); 
    };

    double low = 1.0, high = 1000.0, tol = 1e-4;
    while ((high - low) > tol) {
        double mid = low + (high - low) / 2.0;
        if (evaluateASME(mid) > 0) {
            low = mid; 
        } else {
            high = mid;
        }
    }
    return high;
}