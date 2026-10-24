#include "../include/ASMEDesign.h"
#include <cmath>
#include <algorithm>

constexpr double PI = 3.14159265358979323846;

bool ASMEDesign::calculateRequiredDimensions(double M, double T, double P, double Kb, double Kt, bool hasKeyway, const Material& mat, int hollowMode, double hollowVal, double& out_do, double& out_di) 
{
    M *= 1000.0; T *= 1000.0; P = std::abs(P);
    double tau_allow = std::min(0.30 * mat.Sy, 0.18 * mat.Sut);
    if (hasKeyway) tau_allow *= 0.75;

    auto evalASME = [&](double d_o, double d_i) {
        double k = (d_o > 0) ? (d_i / d_o) : 0.0;
        double bending_term = Kb * M + (P * d_o * (1.0 + k*k)) / 8.0;
        double torsion_term = Kt * T;
        double required_d3 = (16.0 / (PI * tau_allow)) * std::sqrt(bending_term * bending_term + torsion_term * torsion_term);
        return required_d3 - (std::pow(d_o, 3) * (1.0 - k*k*k*k)); 
    };

    bool solvingForDo = (hollowMode != 2);
    double low = 0.0, high = 1000.0;
    if (solvingForDo && hollowMode == 1) low = hollowVal + 0.1;
    if (!solvingForDo) { high = hollowVal - 0.1; low = 0.0; }
    
    if (!solvingForDo && evalASME(hollowVal, 0.0) > 0) return false; 
    
    while ((high - low) > 1e-4) {
        double mid = low + (high - low) / 2.0;
        double do_test = solvingForDo ? mid : hollowVal;
        double di_test = solvingForDo ? (hollowMode == 1 ? hollowVal : (hollowMode == 3 ? mid * hollowVal : 0.0)) : mid;
        
        if (evalASME(do_test, di_test) > 0) {
            if (solvingForDo) low = mid; else high = mid; 
        } else {
            if (solvingForDo) high = mid; else low = mid;
        }
    }
    out_do = solvingForDo ? high : hollowVal;
    out_di = solvingForDo ? (hollowMode == 1 ? hollowVal : (hollowMode == 3 ? high * hollowVal : 0.0)) : low;
    return true;
}