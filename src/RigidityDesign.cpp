#include "../include/RigidityDesign.h"
#include <cmath>

constexpr double PI = 3.14159265358979323846;

bool RigidityDesign::calculateRequiredDimensions(double T_max, double L, double G, double theta_max_deg, int hollowMode, double hollowVal, double& out_do, double& out_di) 
{
    double T_max_nmm = T_max * 1000.0;     
    double G_mpa = G * 1000.0;             
    double theta_rad = theta_max_deg * (PI / 180.0); 
    double req_val = (32.0 * T_max_nmm * L) / (PI * G_mpa * theta_rad);

    if (hollowMode == 0) { 
        out_do = std::pow(req_val, 0.25);
        out_di = 0.0;
    } 
    else if (hollowMode == 1) {
        out_di = hollowVal;
        out_do = std::pow(req_val + std::pow(out_di, 4.0), 0.25);
    } 
    else if (hollowMode == 2) {
        out_do = hollowVal;
        if (std::pow(out_do, 4.0) < req_val) return false; 
        out_di = std::pow(std::pow(out_do, 4.0) - req_val, 0.25);
    } 
    else if (hollowMode == 3) {
        double k = hollowVal;
        out_do = std::pow(req_val / (1.0 - std::pow(k, 4.0)), 0.25);
        out_di = k * out_do;
    }
    return true;
}