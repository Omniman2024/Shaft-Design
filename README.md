# Power Transmission Shaft Design & Verification Solver

A robust C++17 automated engineering solver and diagnostic tool for power transmission shafts subjected to complex multi-axial loading combinations. This software calculates theoretical required outer and inner diameters, evaluates standardized sizes against physical constraints, and outputs high-fidelity terminal reports detailing the exact mechanical governing failure mode.

##  Directory Structure

```text
├── README.md
├── build.bat
├── demo_cases.txt
├── individual_terminal_commands.txt
├── Shaft_Design_10_Master_Test_Cases.xlsx
├── Shaft_Design_Master_Test_Cases_Detailed_v2.xlsx
├── data/
│   ├── materials.csv
│   └── standard_diameters.csv
├── include/
│   ├── ASMEDesign.h
│   ├── DesignReport.h
│   ├── FatigueDesign.h
│   ├── MaterialDatabase.h
│   ├── RigidityDesign.h
│   └── StandardShaft.h
└── src/
    ├── ASMEDesign.cpp
    ├── DesignReport.cpp
    ├── FatigueDesign.cpp
    ├── main.cpp
    ├── MaterialDatabase.cpp
    ├── RigidityDesign.cpp
    └── StandardShaft.cpp
```

##  Instructions to Run

1. **Prerequisites:** Ensure you have a C++17 compatible compiler (e.g., GCC/MinGW via MSYS2 or Visual Studio) installed and accessible in your system `PATH`.
2. **Compilation:** Execute the automated compilation batch script from the root directory:
   ```cmd
   .\build.bat
   ```
3. **Execution:** Run the compiled binary executable from your terminal:
   ```cmd
   .\shaft_solver.exe
   ```
4. **Reference Files:** You can inspect `demo_cases.txt` or `individual_terminal_commands.txt` for preset execution commands and workflow setups.

##  Logic Flowchart

```mermaid
flowchart TD
    classDef reject fill:#ffe6e6,stroke:#cc0000,stroke-width:2px,color:#900000;
    classDef accept fill:#e6ffe6,stroke:#006600,stroke-width:2px,color:#004d00;
    classDef diagnostic fill:#e6f2ff,stroke:#005ce6,stroke-width:2px,color:#330033;
    classDef input fill:#fff9e6,stroke:#b38600,stroke-width:1px,color:#330033;
    classDef governor fill:#f9e6ff,stroke:#660066,stroke-width:2px,color:#330033;

    A([Start New Design Iteration]) --> B{Shaft Configuration}

    %% -- INPUT PHASE --
    B -- Solid --> D{Load Methodology}
    B -- Hollow --> C["Select Optimization: <br/>Fixed Inner, Fixed Outer, or Scaled Ratio"]:::input
    C --> D

    %% -- SOLVER PHASE --
    D -- Fluctuating Loads --> E["Calculate Required Diameter: <br/>Goodman Fatigue & Langer Yield"]:::diagnostic
    D -- Peak Loads --> F["Calculate Required Diameter: <br/>ASME Bending, Torsion, & Axial"]:::diagnostic
    
    E & F --> G["Calculate Required Diameter: <br/>Rigidity / Max Twist"]:::diagnostic

    %% -- DIAGNOSTIC CLASSIFICATION (THE STRENGTH STATES) --
    G --> H{"Classify the Internal Strength State"}
    
    H -- Fluctuating Loads --> S1{"Fatigue vs. Yield Check"}
    S1 -- Fatigue Fails First --> S1A["State: MODIFIED GOODMAN"]:::governor
    S1 -- Yield Fails First --> S1B["State: LANGER YIELD"]:::governor
    S1 -- Area Math Fails --> S1C["State: INSTANT COLLAPSE"]:::reject
    
    H -- Peak Static Loads --> S2{"Check ASME Shear"}
    S2 -- Axial Dominates --> S2A["State: ASME Axial Collapse"]:::governor
    S2 -- Bending Dominates --> S2B["State: ASME Bending"]:::governor
    S2 -- Torsion Dominates --> S2C["State: ASME Torsion"]:::governor
    S2 -- Mixed Loads --> S2D["State: ASME Combined Shear"]:::governor

    %% -- THE RIGIDITY OVERRIDE (THE MATRIX) --
    S1A & S1B & S2A & S2B & S2C & S2D --> R_CHECK{"Is d_rigidity > d_strength?"}
    
    R_CHECK -- Yes --> R_GOV["Overall Governor = RIGIDITY <br/>(Strength State becomes Secondary Limit)"]:::governor
    R_CHECK -- No --> S_GOV["Overall Governor = STRENGTH STATE <br/>(Rigidity is satisfied implicitly)"]:::governor

    %% -- EVALUATION PHASE --
    R_GOV & S_GOV --> EVAL{"Does the required theoretical diameter <br/>exceed max physical limit (d_max)?"}

    %% -- REJECTION BRANCH --
    S1C --> REJ1
    EVAL -- Yes --> REJ1["!!! DESIGN REJECTED !!!"]:::reject
    REJ1 --> REJ2{"Identify Failure Mode"}
    
    REJ2 -- Hollow Pre-Check --> REJ3["PRE-CHECK REJECT <br/>(Solid shaft structurally collapses <br/>before bore optimization)"]:::reject
    REJ2 -- Size Limit Exceeded --> REJ4["CONSTRAINT REJECT <br/>(Exceeded d_max)"]:::reject
    REJ2 -- Math Failure --> REJ5["MATH REJECT <br/>(Instant Structural Collapse)"]:::reject
    
    REJ3 & REJ4 & REJ5 --> REJ_END["Terminal Outputs Exact Primary/Secondary <br/>Bottleneck Causing the Failure"]:::reject

    %% -- ACCEPTANCE BRANCH --
    EVAL -- No --> ACC1["Map to Standard Commercial Outer & Inner Diameters"]
    ACC1 --> ACC2["Generate Final Verification Report <br/>(Explicitly stating Primary & Secondary Limits)"]
    ACC2 --> ACC_END(["DESIGN ACCEPTED"]):::accept
```

##  Mechanical Concepts Used

The solver processes mechanical loads through dual engineering frameworks, isolating failures across multiple physical boundaries:

* **Modified Goodman Line (Fatigue):** Evaluates infinite life under fluctuating bending, torsion, and axial stresses. Governs when alternating load components dominate.
* **Langer Yield Line (Plasticity):** Protects against catastrophic plastic deformation under high steady/mean loads.
* **ASME Code (Static Shear):** Utilizes shock/fatigue multipliers ($K_b, K_t$) and von Mises/Maximum Shear principles for peak-load applications. The diagnostic engine classifies ASME failures into:
    * *Axial Collapse*
    * *Bending Dominated*
    * *Torsion Dominated*
    * *Combined Shear*
* **Rigidity Constraints (Max Twist):** Calculates angular deflection using the shear modulus ($G$). Overrides strength requirements whenever strict rotational tolerances ($\theta \le \theta_{max}$) demand a thicker cross-section.
* **Hollow Shaft Geometric Optimization:** Dynamically computes the hollow ratio ($k = d_i / d_o$) under three distinct constraint profiles: Fixed Inner Bore, Fixed Outer Diameter (Bore Maximization), or Uniform Scaling (Fixed Ratio).

