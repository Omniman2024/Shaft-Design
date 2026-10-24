#include "../include/FatigueDesign.h"
#include <cmath>
#include <algorithm>

#define PI 3.14159265358979323846

double FatigueDesign::calculateRequiredDiameter(double M_min, double M_max, double T_min, double T_max, const Material& mat, double factorOfSafety,double& out_dfatigue, double& out_dyield){ 
    // unit conversions : N-m to N-mm
    M_min *= 1000.0; M_max *= 1000.0;
    T_min *= 1000.0; T_max *= 1000.0;
    // mean and alternating components
    double M_a = std::abs(M_max - M_min) / 2.0;
    double M_m = (M_max + M_min) / 2.0;
    double T_a = std::abs(T_max - T_min) / 2.0;
    double T_m = (T_max + T_min) / 2.0;
    // equivalent alternating stress (Distortion Energy theory)
    double sigma_a = (16.0 / PI) * std::sqrt(4.0 * M_a * M_a + 3.0 * T_a * T_a);
    // equivalent mean stress (Maximum Principal Stress theory)
    double sigma_m = (16.0 / PI) * (M_m + std::sqrt(M_m * M_m + T_m * T_m));
    // modified Goodman Criterion
    double fatigue_term = (sigma_a / mat.Se) + (sigma_m / mat.Sut);
    out_dfatigue = std::cbrt(factorOfSafety * fatigue_term);
    // Langer yield check
    double yield_term = (sigma_a + sigma_m) / mat.Sy;
    out_dyield = std::cbrt(factorOfSafety * yield_term);
    return std::max(out_dfatigue, out_dyield);
}