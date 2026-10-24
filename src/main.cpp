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
        int mode;
        std::cout << "Select Load Data Type:\n";
        std::cout << "1. Fluctuating Loads (Min/Max) -> Modified Goodman Method\n";
        std::cout << "2. Peak Static Loads (Single)  -> ASME Code Method\n";
        while (!(std::cin >> mode) || (mode != 1 && mode != 2)) {
            std::cout << "Invalid choice. Enter 1 or 2: ";
            clearInputBuffer();
        }
        double L = getValidatedInput("Enter Shaft Length (mm): ", 0.0);
        double M_min = 0, M_max = 0, T_min = 0, T_max = 0, P_min = 0, P_max = 0;double peak_M = 0, peak_T = 0, peak_P = 0, Kb = 1.0, Kt = 1.0;
        bool hasKeyway;
        double FoS = 1.0;
        if(mode == 1){
            std::cout << "Enter Min Bending Moment (N-m): "; std::cin >> M_min;
            std::cout << "Enter Max Bending Moment (N-m): "; std::cin >> M_max;
            std::cout << "Enter Min Torque (N-m): "; std::cin >> T_min;
            std::cout << "Enter Max Torque (N-m): "; std::cin >> T_max;
            std::cout << "Enter Min Axial Load (N): "; std::cin >> P_min;
            std::cout << "Enter Max Axial Load (N): "; std::cin >> P_max;
            FoS = getValidatedInput("Enter Required Factor of Safety: ", 0.99);
        }
        else{
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
        matDB.displayMaterials();
        std::cout << "Select Material ID from above: ";
        int mat_id;
        Material selectedMat;
        while (!(std::cin >> mat_id) || !matDB.getMaterialById(mat_id, selectedMat)) {
            std::cout << "Invalid ID. Try again: ";
            clearInputBuffer();
        }
        double d_fatigue, d_yield, d_strength, abs_T_max;
        if (mode == 1) {
            d_strength = FatigueDesign::calculateRequiredDiameter(M_min, M_max, T_min, T_max, P_min, P_max, selectedMat, FoS, d_fatigue, d_yield);
            abs_T_max = std::max(std::abs(T_max), std::abs(T_min));
        } 
        else {
            d_strength = ASMEDesign::calculateRequiredDiameter(peak_M, peak_T, peak_P, Kb, Kt, hasKeyway, selectedMat);
            abs_T_max = std::abs(peak_T);
        }
        double d_rigidity = RigidityDesign::calculateRequiredDiameter(abs_T_max, L, selectedMat.G, theta_max);
        double d_req = std::max(d_strength, d_rigidity);
        double d_std = stdShaft.selectStandardDiameter(d_req, d_max);
        
        DesignReport::generateReport(mode, L, M_min, M_max, T_min, T_max, P_min, P_max, peak_M, peak_T, peak_P, Kb, Kt, hasKeyway, d_max, theta_max, FoS, selectedMat, d_fatigue, d_yield, d_strength, d_rigidity, d_req, d_std);

        std::cout << "Run another design? (y/n): ";
        char choice;
        std::cin >> choice;
        if (choice == 'n' || choice == 'N') {
            running = false;
        }
    }
    return 0;
}