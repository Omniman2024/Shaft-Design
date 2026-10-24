#include "../include/FatigueDesign.h"
#include <cmath>
#include <algorithm>

#define PI 3.14159265358979323846

double FatigueDesign::calculateRequiredDiameter(double M_min, double M_max, double T_min, double T_max, double P_min, double P_max, const Material& mat, double factorOfSafety,double& out_dfatigue, double& out_dyield){ 
    // unit conversions : N-m to N-mm
    M_min *= 1000.0; M_max *= 1000.0;
    T_min *= 1000.0; T_max *= 1000.0;
    // mean and alternating components
    double M_a = std::abs(M_max - M_min) / 2.0;
    double M_m = (M_max + M_min) / 2.0;
    double T_a = std::abs(T_max - T_min) / 2.0;
    double T_m = (T_max + T_min) / 2.0;
    double P_a = std::abs(P_max - P_min) / 2.0;
    double P_m = std::abs(P_max + P_min) / 2.0;
    auto solveForDiameter = [](auto constraintFunc) {
        double low = 1.0;     
        double high = 1000.0; 
        double tol = 1e-4;   
        while ((high - low) > tol) {
            double mid = low + (high - low) / 2.0;
            if (constraintFunc(mid) > 0) {
                low = mid;
            } else {
                high = mid;
            }
        }
        return high; 
    };
    auto evaluateGoodman = [&](double d) {
        double sigma_a = (32.0 * M_a) / (PI * std::pow(d, 3)) + (4.0 * P_a) / (PI * std::pow(d, 2));
        double sigma_m = (32.0 * M_m) / (PI * std::pow(d, 3)) + (4.0 * P_m) / (PI * std::pow(d, 2));
        double tau_a   = (16.0 * T_a) / (PI * std::pow(d, 3));
        double tau_m   = (16.0 * T_m) / (PI * std::pow(d, 3));

        double sigma_ae = std::sqrt(sigma_a * sigma_a + 3.0 * tau_a * tau_a);
        double sigma_me = (sigma_m / 2.0) + std::sqrt(std::pow(sigma_m / 2.0, 2) + tau_m * tau_m);

        return (sigma_ae / mat.Se) + (sigma_me / mat.Sut) - (1.0 / factorOfSafety);
    };
    auto evaluateYield = [&](double d) {
        double sigma_a = (32.0 * M_a) / (PI * std::pow(d, 3)) + (4.0 * P_a) / (PI * std::pow(d, 2));
        double sigma_m = (32.0 * M_m) / (PI * std::pow(d, 3)) + (4.0 * P_m) / (PI * std::pow(d, 2));
        double tau_a   = (16.0 * T_a) / (PI * std::pow(d, 3));
        double tau_m   = (16.0 * T_m) / (PI * std::pow(d, 3));

        double sigma_ae = std::sqrt(sigma_a * sigma_a + 3.0 * tau_a * tau_a);
        double sigma_me = (sigma_m / 2.0) + std::sqrt(std::pow(sigma_m / 2.0, 2) + tau_m * tau_m);

        return ((sigma_ae + sigma_me) / mat.Sy) - (1.0 / factorOfSafety);
    };
    out_dfatigue = solveForDiameter(evaluateGoodman);
    out_dyield = solveForDiameter(evaluateYield);

    return std::max(out_dfatigue, out_dyield);


    // // equivalent alternating stress (Distortion Energy theory)
    // double sigma_a = (16.0 / PI) * std::sqrt(4.0 * M_a * M_a + 3.0 * T_a * T_a);
    // // equivalent mean stress (Maximum Principal Stress theory)
    // double sigma_m = (16.0 / PI) * (M_m + std::sqrt(M_m * M_m + T_m * T_m));
    // // modified Goodman Criterion
    // double fatigue_term = (sigma_a / mat.Se) + (sigma_m / mat.Sut);
    // out_dfatigue = std::cbrt(factorOfSafety * fatigue_term);
    // // Langer yield check
    // double yield_term = (sigma_a + sigma_m) / mat.Sy;
    // out_dyield = std::cbrt(factorOfSafety * yield_term);
    // return std::max(out_dfatigue, out_dyield);
}