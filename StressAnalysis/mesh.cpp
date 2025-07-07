#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/IO/OBJ_reader.h>
#include <CGAL/make_mesh_3.h>
#include <CGAL/Implicit_mesh_domain_3.h>
#include <CGAL/Labeled_mesh_domain_3.h>
#include <CGAL/Polyhedral_mesh_domain_3.h>
#include <CGAL/Mesh_triangulation_3.h>
#include <CGAL/Mesh_complex_3_in_triangulation_3.h>
#include <CGAL/Mesh_criteria_3.h>

#include <fstream>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Surface_mesh<K::Point_3> SurfaceMesh;
typedef CGAL::Polyhedron_3<K> Polyhedron;

typedef CGAL::Polyhedral_mesh_domain_3<Polyhedron, K> Mesh_domain;

typedef CGAL::Mesh_triangulation_3<Mesh_domain>::type Tr;
typedef CGAL::Mesh_complex_3_in_triangulation_3<Tr> C3t3;

typedef CGAL::Mesh_criteria_3<Tr> Mesh_criteria;

int main() {
    // 1. Load surface mesh from .obj
    std::ifstream input("bevelled_beam.obj");
    Polyhedron poly;

    if (!input || !(input >> poly) || poly.empty()) {
        std::cerr << "Cannot open or read OBJ file!\n";
        return 1;
    }

    // 2. Create mesh domain
    Mesh_domain domain(poly);

    // 3. Mesh criteria
    Mesh_criteria criteria(
        CGAL::parameters::facet_angle = 30,
        CGAL::parameters::facet_size = 0.1,
        CGAL::parameters::facet_distance = 0.025,
        CGAL::parameters::cell_radius_edge_ratio = 2,
        CGAL::parameters::cell_size = 0.1
    );

    // 4. Generate tetrahedral mesh
    C3t3 c3t3 = CGAL::make_mesh_3<C3t3>(domain, criteria);

    // 5. Export mesh to .mesh file
    std::ofstream medit_file("output.mesh");
    c3t3.output_to_medit(medit_file);

    std::cout << "Done! Mesh saved as output.mesh\n";
    return 0;
}