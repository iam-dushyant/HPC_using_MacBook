cmake_minimum_required(VERSION 3.9)
project(mesh)

find_package(CGAL REQUIRED)

add_executable(mesh mesh.cpp)

target_link_libraries(mesh CGAL::CGAL)
