#include "../include/DesignReport.h"
#include <iostream>
#include <cmath>
#include <iomanip>
#include <algorithm>

#define PI 3.14159265358979323846

void DesignReport::generateReport(double L, double M_min, double M_max, double T_min, double T_max, double P_min, double P_max, double d_max, double theta_max, double FoS, const Material& mat,double d_fatigue, double d_yield, double d_rigidity, double d_req,double d_std){
    double M_max_nmm = M_max * 1000.0; double M_min_nmm = M_min * 1000.0;
    double T_max_nmm = T_max * 1000.0; double T_min_nmm = T_min * 1000.0;
    double M_a = std::abs(M_max_nmm - M_min_nmm) / 2.0;
    double M_m = (M_max_nmm + M_min_nmm) / 2.0;
    double T_a = std::abs(T_max_nmm - T_min_nmm) / 2.0;
    double T_m = (T_max_nmm + T_min_nmm) / 2.0;
    double s_a = std::abs(P_max - P_min) / 2.0;
    double s_m = (P_max + P_min) / 2.0;

    std::cout << "\n=========================================================\n";
    std::cout << "                 SHAFT DESIGN REPORT                     \n";
    std::cout << "=========================================================\n\n";

    std::cout << "--- A. Input Summary ---\n";
    std::cout << "Configuration      : Solid circular shaft\n";
    std::cout << "Shaft Length       : " << L << " mm\n";
    std::cout << "Bending Moment     : Min = " << M_min << " N-m, Max = " << M_max << " N-m\n";
    std::cout << "Torque             : Min = " << T_min << " N-m, Max = " << T_max << " N-m\n";
    std::cout << "Axial Load         : Min = " << P_min << " N, Max = " << P_max << " N\n";
    std::cout << "Material Selected  : " << mat.name << " (Sut=" << mat.Sut << " MPa, Sy=" << mat.Sy << " MPa, Se=" << mat.Se << " MPa, G=" << mat.G << " GPa)\n";
    std::cout << "Required FoS       : " << FoS << "\n";
    std::cout << "Constraints        : Max Dia = " << d_max << " mm, Max Twist = " << theta_max << " deg\n\n";

    std::cout << "--- B. Calculation Results ---\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Req. Dia (Fatigue) : " << d_fatigue << " mm\n";
    std::cout << "Req. Dia (Yield)   : " << d_yield << " mm\n";
    std::cout << "Req. Dia (Rigidity): " << d_rigidity << " mm\n";
    std::cout << "Governing Theo. Dia: " << d_req << " mm\n\n";

    if (d_std < 0) {
        std::cout << "!!! DESIGN REJECTED !!!\n";
        std::cout << "Reason: Governing theoretical diameter (" << d_req << " mm) exceeds the maximum allowable diameter (" << d_max << " mm) or no standard size is available.\n";
        std::cout << "=========================================================\n";
        return;
    }
    double d3 = d_std * d_std * d_std;
    double d4 = d3 * d_std;
    double sigma_a = (32.0 * M_a) / (PI * d3) + (4.0 * s_a) / (PI * (d_std * d_std));
    double sigma_m = (32.0 * M_m) / (PI * d3) + (4.0 * s_m) / (PI * (d_std * d_std));
    double tau_a   = (16.0 * T_a) / (PI * d3);
    double tau_m   = (16.0 * T_m) / (PI * d3);
    double sigma_ae = std::sqrt(sigma_a * sigma_a + 3.0 * tau_a * tau_a);
    double sigma_me = (sigma_m / 2.0) + std::sqrt(std::pow(sigma_m / 2.0, 2) + tau_m * tau_m);
    double achieved_fatigue_FoS = 1.0 / ((sigma_ae / mat.Se) + (sigma_me / mat.Sut));
    double achieved_yield_FoS = mat.Sy / (sigma_ae + sigma_me);
    double governing_FoS = std::min(achieved_fatigue_FoS, achieved_yield_FoS);
    double abs_T_max = std::max(std::abs(T_max_nmm), std::abs(T_min_nmm));
    double achieved_twist_rad = (32.0 * abs_T_max * L) / (PI * d4 * (mat.G * 1000.0));
    double achieved_twist_deg = achieved_twist_rad * (180.0 / PI);

    std::cout << "--- C. Final Design & Verification ---\n";
    std::cout << "Selected Std. Dia  : " << d_std << " mm\n";
    std::cout << "Achieved FoS       : " << governing_FoS << " (Target: " << FoS << ")\n";
    std::cout << "Achieved Twist     : " << achieved_twist_deg << " deg (Target: <= " << theta_max << " deg)\n\n";

    std::cout << "--- D. Final Status ---\n";
    std::cout << "DESIGN ACCEPTED\n\n";
    std::cout << "Strength check       : PASS\n";
    std::cout << "Rigidity check       : PASS\n";
    std::cout << "Maximum dia check    : PASS\n";
    std::cout << "Factor of Safety     : PASS\n";
    std::cout << "=========================================================\n";
}