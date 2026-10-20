#include "../include/FatigueDesign.h"
#include <cmath>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

double FatigueDesign::calculateRequiredDiameter(
    double M_min, double M_max, 
    double T_min, double T_max, 
    const Material& mat, double factorOfSafety,
    double& out_dfatigue, double& out_dyield) 
{
    // Convert moments from N-m to N-mm
    M_min *= 1000.0; M_max *= 1000.0;
    T_min *= 1000.0; T_max *= 1000.0;

    // 1. Load Decomposition
    double M_a = std::abs(M_max - M_min) / 2.0;
    double M_m = (M_max + M_min) / 2.0;
    double T_a = std::abs(T_max - T_min) / 2.0;
    double T_m = (T_max + T_min) / 2.0;

    // 2. Equivalent Stress Formulation (Extracting 1/d^3 as a constant)
    // Equivalent alternating stress (Distortion Energy theory)
    // sigma_ae = (16 / (pi * d^3)) * sqrt(4 * M_a^2 + 3 * T_a^2)
    double K_a = (16.0 / M_PI) * std::sqrt(4.0 * M_a * M_a + 3.0 * T_a * T_a);

    // Equivalent mean stress (Maximum Principal Stress theory)
    // sigma_me = (16 / (pi * d^3)) * (M_m + sqrt(M_m^2 + T_m^2))
    double K_m = (16.0 / M_PI) * (M_m + std::sqrt(M_m * M_m + T_m * T_m));

    // 3. Modified Goodman Criterion: (sigma_ae / Se) + (sigma_me / Sut) = 1 / n
    // Factoring out 1/d^3: (1/d^3) * [K_a / Se + K_m / Sut] = 1 / n
    double fatigue_term = (K_a / mat.Se) + (K_m / mat.Sut);
    out_dfatigue = std::cbrt(factorOfSafety * fatigue_term);

    // 4. Yield / Static Strength Check (Langer Yield Line)
    // sigma_max_eq = sigma_ae + sigma_me <= Sy / n
    double yield_term = (K_a + K_m) / mat.Sy;
    out_dyield = std::cbrt(factorOfSafety * yield_term);

    // Return the governing strength diameter
    return std::max(out_dfatigue, out_dyield);
}