#include "../include/FatigueDesign.h"
#include <cmath>
#include <algorithm>

#define PI 3.14159265358979323846

bool FatigueDesign::calculateRequiredDimensions(double M_min, double M_max, double T_min, double T_max, double P_min, double P_max,const Material& mat, double factorOfSafety, int hollowMode, double hollowVal, double& out_do, double& out_di) 
{
    double M_a = std::abs((M_max - M_min) * 1000.0) / 2.0;
    double M_m = std::abs((M_max + M_min) * 1000.0) / 2.0;
    double T_a = std::abs((T_max - T_min) * 1000.0) / 2.0;
    double T_m = std::abs((T_max + T_min) * 1000.0) / 2.0;
    double P_a = std::abs(P_max - P_min) / 2.0;
    double P_m = std::abs(P_max + P_min) / 2.0; 

    auto getStresses = [&](double d_o, double d_i) {
        double k = (d_o > 0) ? (d_i / d_o) : 0.0;
        double k2 = k * k;
        double k4 = k2 * k2;
        double area_factor = PI * d_o * d_o * (1.0 - k2);
        double polar_factor = PI * std::pow(d_o, 3) * (1.0 - k4);
        double sigma_a = (32.0 * M_a) / polar_factor + (4.0 * P_a) / area_factor;
        double sigma_m = (32.0 * M_m) / polar_factor + (4.0 * P_m) / area_factor;
        double tau_a   = (16.0 * T_a) / polar_factor;
        double tau_m   = (16.0 * T_m) / polar_factor;
        double sigma_ae = std::sqrt(sigma_a * sigma_a + 3.0 * tau_a * tau_a);
        double sigma_me = (sigma_m / 2.0) + std::sqrt(std::pow(sigma_m / 2.0, 2) + tau_m * tau_m);
        return std::make_pair(sigma_ae, sigma_me);
    };

    auto evalGoodman = [&](double d_o, double d_i) {
        auto stresses = getStresses(d_o, d_i);
        return (stresses.first / mat.Se) + (stresses.second / mat.Sut) - (1.0 / factorOfSafety);
    };

    auto evalYield = [&](double d_o, double d_i) {
        auto stresses = getStresses(d_o, d_i);
        return ((stresses.first + stresses.second) / mat.Sy) - (1.0 / factorOfSafety);
    };

    auto solve = [&](auto constraintFunc, double& res_do, double& res_di) -> bool {
        bool solvingForDo = (hollowMode != 2);
        double low = 0.0, high = 1000.0;
        if (solvingForDo && hollowMode == 1) low = hollowVal + 0.1;
        if (!solvingForDo) { high = hollowVal - 0.1; low = 0.0; }
        
        if (!solvingForDo && constraintFunc(hollowVal, 0.0) > 0) return false; 
        
        while ((high - low) > 1e-4) {
            double mid = low + (high - low) / 2.0;
            double do_test = solvingForDo ? mid : hollowVal;
            double di_test = solvingForDo ? (hollowMode == 1 ? hollowVal : (hollowMode == 3 ? mid * hollowVal : 0.0)) : mid;
            if (constraintFunc(do_test, di_test) > 0) {
                if (solvingForDo) low = mid; else high = mid; 
            } 
            else {
                if (solvingForDo) high = mid; else low = mid;
            }
        }
        res_do = solvingForDo ? high : hollowVal;
        res_di = solvingForDo ? (hollowMode == 1 ? hollowVal : (hollowMode == 3 ? high * hollowVal : 0.0)) : low;
        return true;
    };

    double do_f, di_f, do_y, di_y;
    if (!solve(evalGoodman, do_f, di_f) || !solve(evalYield, do_y, di_y)) return false;
    if (hollowMode == 2) {
        out_do = hollowVal;
        out_di = std::min(di_f, di_y); 
    } 
    else {
        out_do = std::max(do_f, do_y);
        out_di = (hollowMode == 1) ? hollowVal : (hollowMode == 3 ? out_do * hollowVal : 0.0);
    }
    return true;
}