# Introduction

Having experimented with small problems, such as Matrix Multiplication, Fast 
Fourier Transforms, using OpenMP and comparing results with a conventional 
C++ code versus multi-threaded C++ code, this small project considers a 
complex Engineering problem of stress analysis by implementing Finite 
Element Analysis (FEA).

The objective of this project is to calculate stress from scratch, rather 
than using an FEA software.

# Steps of implementation

## Step 1: Model development
The model was developed using Blender. The resulting file is exported as `.obj` 
file.

## Step 2: Parsing obj file
The resulting `.obj` file is parsed in a C++ code `parsing.cpp`.

## Step 3: Volumetric Mesh Generation
This is a complex task. The aim to accomplish this is to integrate an 
existing open-source meshing library like TetGen or CGAL 
(Computational Geometry Algorithms Library).

## Step 4: Defining material properties
This step defines crucial material properties, such as Young's modulus (E) 
and Poisson's ratio (v) for each material region in my structure. The multiple 
materials and their properties could be stored in a `struct` data structure.

## Step 5: Applying loads and Boundary Conditions (BCs)
Thi step defines where the loads are applied and where the structure is 
constrained. This is implemented by storing loads as a vector of forces `F` 
(one for each degree of freedom) and BCs as a list of constrained degrees 
of freedom.

## Step 6: Solver using C++
### Step 6(A): Element Stiffness Matrix Calculator
This is where I would use OpenMP, as used in the simpler Matrix Multiplication 
C++ code.
### Step 6(B): Global Stiffness Matrix Assembly
This step is to combine all the local element stiffness matrices into a 
single, lrge global stiffness matrix (K). This matrix relates all nodal 
displacements to all applied forces (Kc dot U = F). As the global stiffness 
matrix typically very large and sparse (i.e., most entries are zero), 
specialised data structures, such as Compressed Sparse Row (CSR) or 
Coordinate-COO, could be used. The approach planned is to have multiple 
threads, which assembles its own partial global stiffness matrix.

## Step 7: Applying BCs
This step involves modifying the global stiffness matrix and force vector to 
incorporate the fixed displacements.

## Step 8: Solving the System of Linear Equations
For this step, there are two approaches:
1. Direct solvers - For example, LU decomposition
2. Iterative solvers - For example, Conjugate Gradient, GMRES
While direct solvers are very robust but memory-intensive, iterative solvers 
are more memoer-efficient but require more iterations.

## Step 9: Calculating stress and strain
Once nodal replacements are known, stress and strain could be calculated 
within each element using constituitive equations and element shape 
functions.

## Step 10: Results and visualisations
Calculated displacements, stresses and strains could be exported as 
`.vtu` (unstructured grid) or `.vtk` files. This could then be extracted 
in open-source tool such as ParaView or Visit for visualisation.
