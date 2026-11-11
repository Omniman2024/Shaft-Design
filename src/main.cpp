#include <iostream>
#include <limits>
#include <cmath>
#include <algorithm>
#include "../include/MaterialDatabase.h"
#include "../include/StandardShaft.h"
#include "../include/FatigueDesign.h"
#include "../include/ASMEDesign.h"
#include "../include/RigidityDesign.h"
#include "../include/DesignReport.h"

void clearInputBuffer() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

double getValidatedInput(const std::string& prompt, double minValue) {
    double value;
    std::cout << prompt;
    while (!(std::cin >> value) || value <= minValue) {
        std::cout << "Invalid input. Must be > " << minValue << ": ";
        clearInputBuffer();
    }
    return value;
}

int main() {
    MaterialDatabase matDB;
    StandardShaft stdShaft;
    std::cout << "Loading databases...\n";
    if (!matDB.loadDatabase("data/materials.csv") || !stdShaft.loadDiameters("data/standard_diameters.csv")) {
        std::cerr << "Failed to load required databases. Check 'data' folder.\n";
        return 1;
    }
    bool running = true;
    while (running) {
        std::cout << "\n=== Power Transmission Shaft Designer ===\n";
        
        int shaftConf = 1;
        int hollowMode = 0;
        double hollowVal = 0.0;
        
        std::cout << "Shaft Configuration:\n1. Solid Shaft\n2. Hollow Shaft\nChoice (1 or 2): ";
        while (!(std::cin >> shaftConf) || (shaftConf != 1 && shaftConf != 2)) {
            std::cout << "Invalid choice. Enter 1 or 2: ";
            clearInputBuffer();
        }
        
        if (shaftConf == 2) {
            std::cout << "\nHollow Shaft Reference Mode:\n";
            std::cout << "1. Specify Inner Diameter (Solve for Outer)\n";
            std::cout << "2. Specify Outer Diameter (Solve for max safe Inner)\n";
            std::cout << "3. Specify Ratio (di/do, Solve for both)\n";
            std::cout << "Choice (1-3): ";
            while (!(std::cin >> hollowMode) || hollowMode < 1 || hollowMode > 3) {
                std::cout << "Invalid choice. Enter 1, 2, or 3: ";
                clearInputBuffer();
            }
            if (hollowMode == 1) hollowVal = getValidatedInput("Enter Inner Diameter (mm): ", 0.0);
            if (hollowMode == 2) hollowVal = getValidatedInput("Enter Outer Diameter (mm): ", 0.0);
            if (hollowMode == 3) hollowVal = getValidatedInput("Enter Ratio (di/do): ", 0.0);
        }

        int mode;
        std::cout << "\nSelect Load Data Type:\n";
        std::cout << "1. Fluctuating Loads (Min/Max) -> Modified Goodman Method\n";
        std::cout << "2. Peak Static Loads (Single)  -> ASME Code Method\n";
        std::cout << "Choice (1 or 2): ";
        while (!(std::cin >> mode) || (mode != 1 && mode != 2)) {
            std::cout << "Invalid choice. Enter 1 or 2: ";
            clearInputBuffer();
        }
        
        double L = getValidatedInput("Enter Shaft Length (mm): ", 0.0);
        double M_min = 0, M_max = 0, T_min = 0, T_max = 0, P_min = 0, P_max = 0;
        double peak_M = 0, peak_T = 0, peak_P = 0, Kb = 1.0, Kt = 1.0;
        bool hasKeyway = false;
        double FoS = 1.0;
        
        if (mode == 1) {
            std::cout << "Enter Min Bending Moment (N-m): "; std::cin >> M_min;
            std::cout << "Enter Max Bending Moment (N-m): "; std::cin >> M_max;
            std::cout << "Enter Min Torque (N-m): "; std::cin >> T_min;
            std::cout << "Enter Max Torque (N-m): "; std::cin >> T_max;
            std::cout << "Enter Min Axial Load (N): "; std::cin >> P_min;
            std::cout << "Enter Max Axial Load (N): "; std::cin >> P_max;
            FoS = getValidatedInput("Enter Required Factor of Safety: ", 0.99);
        } else {
            std::cout << "Enter Peak Bending Moment (N-m): "; std::cin >> peak_M;
            std::cout << "Enter Peak Torque (N-m): "; std::cin >> peak_T;
            std::cout << "Enter Peak Axial Load (N): "; std::cin >> peak_P;
            
            std::cout << "Does the shaft have a keyway? (y/n): ";
            char kw;
            std::cin >> kw;
            hasKeyway = (kw == 'y' || kw == 'Y');

            std::cout << "\nSelect Application Type for Shock Factors:\n";
            std::cout << "1. Stationary - Gradual Load\n";
            std::cout << "2. Stationary - Sudden Load\n";
            std::cout << "3. Revolving - Gradual Load\n";
            std::cout << "4. Revolving - Minor Shock\n";
            std::cout << "5. Revolving - Heavy Shock\n";
            int appChoice;
            std::cout << "Choice (1-5): ";
            std::cin >> appChoice;
            switch(appChoice) {
                case 1: Kb = 1.0; Kt = 1.0; break;
                case 2: Kb = 2.0; Kt = 2.0; break;
                case 3: Kb = 1.5; Kt = 1.0; break;
                case 4: Kb = 2.0; Kt = 1.5; break;
                case 5: Kb = 3.0; Kt = 3.0; break;
                default: Kb = 2.0; Kt = 1.5; break; 
            }
        }
        
        double d_max = getValidatedInput("Enter Max Allowable Diameter (mm): ", 0.0);
        double theta_max = getValidatedInput("Enter Max Allowable Twist (degrees): ", 0.0);
        if(hollowMode == 3 && hollowVal >= 1){
            std::cout << "\n!!! DESIGN REJECTED : MATH REJECT (instant structural collapse) !!!\n";
            std::cout << "Reason: A hollow ratio (k) >= 1 requires an inner bore larger than the outer diameter. The shaft mathematically ceases to exist.\n";
            std::cout << "Run another design? (y/n): ";
            char choice;
            std::cin >> choice;
            if (choice == 'n' || choice == 'N') running = false;
            continue;
        }
        matDB.displayMaterials();
        std::cout << "Select Material ID from above: ";
        int mat_id;
        Material selectedMat;
        while (!(std::cin >> mat_id) || !matDB.getMaterialById(mat_id, selectedMat)) {
            std::cout << "Invalid ID. Try again: ";
            clearInputBuffer();
        }
        
        double d_strength_do = 0, d_strength_di = 0;
        double d_rigidity_do = 0, d_rigidity_di = 0;
        bool pass_strength = true, pass_rigidity = true;
        double abs_T_max = 0;
        
        if (mode == 1) {
            pass_strength = FatigueDesign::calculateRequiredDimensions(M_min, M_max, T_min, T_max, P_min, P_max, selectedMat, FoS, hollowMode, hollowVal, d_strength_do, d_strength_di);
            abs_T_max = std::max(std::abs(T_max), std::abs(T_min));
        } else {
            pass_strength = ASMEDesign::calculateRequiredDimensions(peak_M, peak_T, peak_P, Kb, Kt, hasKeyway, selectedMat, hollowMode, hollowVal, d_strength_do, d_strength_di);
            abs_T_max = std::abs(peak_T);
        }
        
        pass_rigidity = RigidityDesign::calculateRequiredDimensions(abs_T_max, L, selectedMat.G, theta_max, hollowMode, hollowVal, d_rigidity_do, d_rigidity_di);
        
        if (!pass_strength || !pass_rigidity) {
            std::cout << "\n!!! DESIGN REJECTED: User's Outer Diameter constraint is too small to support loads even as a solid shaft !!!\n";
            std::cout << "Run another design? (y/n): ";
            char choice;
            std::cin >> choice;
            if (choice == 'n' || choice == 'N') running = false;
            continue; 
        }

        double d_req_do = (hollowMode == 2) ? hollowVal : std::max(d_strength_do, d_rigidity_do);
        double d_req_di = (hollowMode == 2) ? std::min(d_strength_di, d_rigidity_di) : (hollowMode == 1 ? hollowVal : (hollowMode == 3 ? d_req_do * hollowVal : 0.0));
        
        double d_std_do = (hollowMode == 2) ? hollowVal : stdShaft.selectStandardDiameter(d_req_do, d_max);
        double d_std_di = (hollowMode == 2) ? d_req_di : (hollowMode == 1 ? hollowVal : (hollowMode == 3 ? d_std_do * hollowVal : 0.0));
        
        DesignReport::generateReport(
            mode, shaftConf, hollowMode, L, 
            M_min, M_max, T_min, T_max, P_min, P_max, 
            peak_M, peak_T, peak_P, Kb, Kt, hasKeyway, 
            d_max, theta_max, FoS, selectedMat, 
            d_strength_do, d_rigidity_do, d_req_do, d_std_do,
            d_req_di, d_std_di);

        std::cout << "Run another design? (y/n): ";
        char choice;
        std::cin >> choice;
        if (choice == 'n' || choice == 'N') {
            running = false;
        }
    }
    return 0;
}