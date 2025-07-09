#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/IO/Polyhedron_iostream.h>
#include <CGAL/Polyhedron_3.h>

// Core meshing components
#include <CGAL/make_mesh_3.h>
#include <CGAL/Polyhedral_mesh_domain_3.h>
#include <CGAL/Mesh_triangulation_3.h>
#include <CGAL/Mesh_complex_3_in_triangulation_3.h>
#include <CGAL/Mesh_criteria_3.h>

// Standard C++ library
#include <fstream>
#include <iostream>

// Kernel for geometric predicates and constructions
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;

// Polyhedron type using the chosen kernel
typedef CGAL::Polyhedron_3<K> Polyhedron;

// Mesh domain type
typedef CGAL::Polyhedral_mesh_domain_3<Polyhedron, K> Mesh_domain;

// Triangulation type used by the mesher
typedef CGAL::Mesh_triangulation_3<Mesh_domain>::type Tr;

// Mesh complex type which holds the resulting tetrahedral mesh.
// C3t3 (Complex 3-dimensional in Triangulation 3-dimensional) stores
// the cells (tetrahedra) and their classification (inside/outside the domain).
typedef CGAL::Mesh_complex_3_in_triangulation_3<Tr> C3t3;

// Mesh criteria type which specifies the quality constraints
// for the generated tetrahedra and facets.
typedef CGAL::Mesh_criteria_3<Tr> Mesh_criteria;

int main() {
    std::ifstream input("bevelled_beam.obj");
    Polyhedron poly;

    if (!input || !(input >> poly) || poly.empty()) {
        std::cerr << "Error: Cannot open or read OBJ file 'bevelled_beam.obj' or it's empty.\n"
                  << "Please ensure the file exists and is a valid OBJ format.\n";
        return 1;
    }
    std::cout << "Successfully loaded 'bevelled_beam.obj'.\n";

    Mesh_domain domain(poly);
    std::cout << "Mesh domain created.\n";

    Mesh_criteria criteria(
        CGAL::parameters::facet_angle = 30.0,              // Minimum angle of facets (in degrees)
        CGAL::parameters::facet_size = 0.1,                // Maximum size of facets
        CGAL::parameters::facet_distance = 0.025,          // Maximum distance between mesh facet and input surface
        CGAL::parameters::cell_radius_edge_ratio = 2.0,    // Maximum radius-edge ratio of tetrahedra
        CGAL::parameters::cell_size = 0.1                  // Maximum size of tetrahedra
    );
    std::cout << "Meshing criteria defined.\n";

    std::cout << "Generating tetrahedral mesh... This may take some time.\n";
    C3t3 c3t3 = CGAL::make_mesh_3<C3t3>(domain, criteria);
    std::cout << "Tetrahedral mesh generation complete.\n";

    std::ofstream medit_file("output.mesh");
    
    if (!medit_file.is_open()) {
        std::cerr << "Error: Could not open 'output.mesh' for writing. Check permissions or path.\n";
        return 1;
    }
    c3t3.output_to_medit(medit_file);
    medit_file.close();

    std::cout << "Done! Mesh saved as output.mesh\n";
    return 0;
}
