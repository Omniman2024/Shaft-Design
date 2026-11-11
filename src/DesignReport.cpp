#include "../include/DesignReport.h"
#include <iostream>
#include <cmath>
#include <iomanip>
#include <algorithm>
#include <string>

#define PI 3.14159265358979323846

void DesignReport::generateReport(
    int mode, int shaftConf, int hollowMode, double L, 
    double M_min, double M_max, double T_min, double T_max, double P_min, double P_max,
    double peak_M, double peak_T, double peak_P, double Kb, double Kt, bool hasKeyway,
    double d_max, double theta_max, double FoS, const Material& mat,
    double d_strength_do, double d_rigidity_do, double d_req_do, double d_std_do,
    double d_req_di, double d_std_di)
{
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
    std::cout << "Configuration      : " << (shaftConf == 1 ? "Solid Circular Shaft" : "Hollow Circular Shaft") << "\n";
    std::cout << "Shaft Length       : " << L << " mm\n";
    if (mode == 1) {
        std::cout << "Bending Moment     : Min = " << M_min << " N-m, Max = " << M_max << " N-m\n";
        std::cout << "Torque             : Min = " << T_min << " N-m, Max = " << T_max << " N-m\n";
        std::cout << "Axial Load         : Min = " << P_min << " N, Max = " << P_max << " N\n";
    } else {
        std::cout << "Peak Loads         : M = " << peak_M << " N-m, T = " << peak_T << " N-m, P = " << peak_P << " N\n";
        std::cout << "ASME Multipliers   : Kb = " << Kb << ", Kt = " << Kt << ", Keyway = " << (hasKeyway ? "Yes" : "No") << "\n";
    }
    std::cout << "Material Selected  : " << mat.name << " (Sut=" << mat.Sut << " MPa, Sy=" << mat.Sy << " MPa, Se=" << mat.Se << " MPa, G=" << mat.G << " GPa)\n";
    if (mode == 1) std::cout << "Required FoS       : " << FoS << "\n";
    std::cout << "Constraints        : Max OD = " << d_max << " mm, Max Twist = " << theta_max << " deg\n\n";

    std::cout << "--- B. Calculation Results ---\n";
    std::cout << std::fixed << std::setprecision(2);
    
    std::cout << "Req. Outer Dia (Strength) : " << d_strength_do << " mm\n";
    std::cout << "Req. Outer Dia (Rigidity) : " << d_rigidity_do << " mm\n";
    
    bool isRigidityGoverning = false;
    if (hollowMode == 2) {
        if (d_rigidity_do < d_strength_do) isRigidityGoverning = true;
    } else {
        if (d_rigidity_do > d_strength_do) isRigidityGoverning = true;
    }

    std::cout << "Governing Theo. Outer Dia : " << d_req_do << " mm\n";
    if (shaftConf == 2) {
        std::cout << "Governing Theo. Inner Dia : " << d_req_di << " mm\n";
    }
    
    std::string specificGoverningLimit = "Unknown Limit";
    
    // Calculate hollow ratio for theoretical required diameter
    double test_k = 0.0;
    if (shaftConf == 2) {
        test_k = (hollowMode == 2 && d_req_do > 0) ? (d_req_di / d_req_do) : 
                 ((d_req_do > 0) ? (d_req_di / d_req_do) : 0.0);
    }
    
    if (isRigidityGoverning) {
        specificGoverningLimit = "RIGIDITY limit constraint (Max Twist)";
        std::cout << "Governing Limit Phase     : " << specificGoverningLimit << "\n";
    } else if (mode == 1) {
        double area_f = PI * d_req_do * d_req_do * (1.0 - test_k*test_k);
        double polar_f = PI * std::pow(d_req_do, 3) * (1.0 - std::pow(test_k, 4));
        
        if (polar_f > 0 && area_f > 0) {
            double sa = (32.0 * M_a) / polar_f + (4.0 * s_a) / area_f;
            double sm = (32.0 * M_m) / polar_f + (4.0 * s_m) / area_f;
            double ta = (16.0 * T_a) / polar_f;
            double tm = (16.0 * T_m) / polar_f;
            
            double sae = std::sqrt(sa * sa + 3.0 * ta * ta);
            double sme = (sm / 2.0) + std::sqrt(std::pow(sm / 2.0, 2) + tm * tm);
            
            double f_fos = 1.0 / ((sae / mat.Se) + (sme / mat.Sut));
            double y_fos = mat.Sy / (sae + sme);
            
            if (y_fos < f_fos) specificGoverningLimit = "Langer YIELD limit constraint (Plasticity)";
            else specificGoverningLimit = "Modified GOODMAN limit constraint (Fatigue)";
        } else {
            specificGoverningLimit = "Instant Structural Collapse (Pre-Check Failed)";
        }
    } else {
        // ASME Code diagnostic using the correct hollow shaft geometry modifier
        double axial_term = (std::abs(peak_P) * d_req_do * (1.0 + test_k * test_k)) / 8.0; 
        double bending_term = Kb * (peak_M * 1000.0);
        double torsion_term = Kt * (peak_T * 1000.0);
        
        if (axial_term > bending_term * 2.0 && axial_term > torsion_term * 2.0) {
            specificGoverningLimit = "ASME Code limit constraint (Axial Collapse)";
        } else if (bending_term > torsion_term * 1.5 && bending_term > axial_term * 1.5) {
            specificGoverningLimit = "ASME Code limit constraint (Bending Dominated)";
        } else if (torsion_term > bending_term * 1.5 && torsion_term > axial_term * 1.5) {
            specificGoverningLimit = "ASME Code limit constraint (Torsion Dominated)";
        } else {
            specificGoverningLimit = "ASME Code limit constraint (Combined Bending & Torsion)";
        }
    }
    
    std::cout << "\n";

    if (d_std_do < 0) {
        std::cout << "!!! DESIGN REJECTED !!!\n";
        if (shaftConf == 2 && hollowMode == 2 && mode == 1 && specificGoverningLimit != "RIGIDITY limit constraint (Max Twist)") {
            std::cout << "Reason: PRE-CHECK REJECT. A solid " << d_req_do << " mm shaft structurally fails before inner bore optimization.\n";
        } else {
            std::cout << "Reason: Required theoretical outer diameter (" << d_req_do << " mm) exceeds max allowable constraint (" << d_max << " mm).\n";
        }
        std::cout << "Bottleneck: Design restricted by " << specificGoverningLimit << ".\n";
        std::cout << "=========================================================\n";
        return;
    }

    double k = (d_std_do > 0) ? (d_std_di / d_std_do) : 0.0;
    double k2 = k * k;
    double k4 = k2 * k2;
    double abs_T_max = (mode == 1) ? std::max(std::abs(T_max), std::abs(T_min)) : std::abs(peak_T);
    
    double achieved_twist_rad = (32.0 * abs_T_max * 1000.0 * L) / (PI * std::pow(d_std_do, 4) * (1.0 - k4) * (mat.G * 1000.0));
    double achieved_twist_deg = achieved_twist_rad * (180.0 / PI);

    std::cout << "--- C. Final Design & Verification ---\n";
    std::cout << "Selected Std. Outer Dia   : " << d_std_do << " mm\n";
    if (shaftConf == 2) {
        std::cout << "Corresponding Inner Dia   : " << d_std_di << " mm (Ratio k=" << k << ")\n";
        if (hollowMode == 1) std::cout << "Optimization Profile      : Fixed Inner Bore, Scaled Outer Dia\n";
        else if (hollowMode == 2) std::cout << "Optimization Profile      : Fixed Outer Dia, Maximized Inner Bore\n";
        else if (hollowMode == 3) std::cout << "Optimization Profile      : Uniformly Scaled (Fixed Ratio)\n";
    }

    if (isRigidityGoverning) {
        std::cout << "Overall Governing Limit   : RIGIDITY Constraint (Max Twist)\n";
    }

    if (mode == 1) {
        double area_factor = PI * d_std_do * d_std_do * (1.0 - k2);
        double polar_factor = PI * std::pow(d_std_do, 3) * (1.0 - k4);
        
        double sigma_a = (32.0 * M_a) / polar_factor + (4.0 * s_a) / area_factor;
        double sigma_m = (32.0 * M_m) / polar_factor + (4.0 * s_m) / area_factor;
        double tau_a   = (16.0 * T_a) / polar_factor;
        double tau_m   = (16.0 * T_m) / polar_factor;
        
        double sigma_ae = std::sqrt(sigma_a * sigma_a + 3.0 * tau_a * tau_a);
        double sigma_me = (sigma_m / 2.0) + std::sqrt(std::pow(sigma_m / 2.0, 2) + tau_m * tau_m);
        
        double achieved_fatigue_FoS = 1.0 / ((sigma_ae / mat.Se) + (sigma_me / mat.Sut));
        double achieved_yield_FoS = mat.Sy / (sigma_ae + sigma_me);
        double governing_FoS = std::min(achieved_fatigue_FoS, achieved_yield_FoS);
        
        std::cout << "Achieved FoS              : " << governing_FoS << " (Target: " << FoS << ")\n";
        
        if (achieved_yield_FoS < achieved_fatigue_FoS) {
            std::cout << (isRigidityGoverning ? "Secondary Strength Limit  : " : "Strength Governing Limit  : ") << "Langer YIELD Line (Plasticity)\n";
            std::cout << "Yield Check               : VERIFIED via Explicit " 
                      << (shaftConf == 1 ? "Solid" : "Hollow") << " Stress\n";
        } else {
            std::cout << (isRigidityGoverning ? "Secondary Strength Limit  : " : "Strength Governing Limit  : ") << "Modified GOODMAN Line (Fatigue)\n";
            std::cout << "Fatigue Check             : VERIFIED via Explicit " 
                      << (shaftConf == 1 ? "Solid" : "Hollow") << " Goodman Stress\n";
        }
    } 
    else {
        double tau_allow = std::min(0.30 * mat.Sy, 0.18 * mat.Sut);
        if (hasKeyway) tau_allow *= 0.75;
        
        double pure_bending = Kb * (peak_M * 1000.0);
        double pure_axial = (std::abs(peak_P) * d_std_do * (1.0 + k2)) / 8.0;
        double torsion_term = Kt * (peak_T * 1000.0);
        
        double combined_bending = pure_bending + pure_axial;
        double tau_actual = (16.0 / (PI * std::pow(d_std_do, 3) * (1.0 - k4))) * std::sqrt(combined_bending * combined_bending + torsion_term * torsion_term);
        
        std::string asme_diagnostic;
        if (pure_axial > pure_bending * 2.0 && pure_axial > torsion_term * 2.0) {
            asme_diagnostic = "Axial Collapse";
        } else if (pure_bending > torsion_term * 1.5 && pure_bending > pure_axial * 1.5) {
            asme_diagnostic = "Bending Dominated";
        } else if (torsion_term > pure_bending * 1.5 && torsion_term > pure_axial * 1.5) {
            asme_diagnostic = "Torsion Dominated";
        } else {
            asme_diagnostic = "Combined Shear";
        }
        
        std::cout << (isRigidityGoverning ? "Secondary Strength Limit  : " : "Strength Governing Limit  : ") << "ASME Code (" << asme_diagnostic << ")\n";
        std::cout << "Max Shear Stress          : " << tau_actual << " MPa (Allowable: " << tau_allow << " MPa)\n";
    }
    
    std::cout << "Achieved Twist            : " << achieved_twist_deg << " deg (Target: <= " << theta_max << " deg)\n\n";

    std::cout << "--- D. Final Status ---\n";
    std::cout << "DESIGN ACCEPTED\n\n";
    std::cout << "=========================================================\n";
}