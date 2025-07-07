#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <limits>

using namespace std;

int main() {
    ifstream infile("bevelled_beam.obj");
    if (!infile) {
        cerr << "Could not open OBJ file.\n";
        return 1;
    }

    double min_x = numeric_limits<double>::max();
    double max_x = numeric_limits<double>::lowest();
    double min_y = numeric_limits<double>::max();
    double max_y = numeric_limits<double>::lowest();
    double min_z = numeric_limits<double>::max();
    double max_z = numeric_limits<double>::lowest();

    string line;
    while (getline(infile, line)) {
        if (line.substr(0, 2) == "v ") {
            istringstream ss(line);
            string v;
            double x, y, z;
            ss >> v >> x >> y >> z;

            min_x = min(min_x, x);
            max_x = max(max_x, x);
            min_y = min(min_y, y);
            max_y = max(max_y, y);
            min_z = min(min_z, z);
            max_z = max(max_z, z);
        }
    }

    infile.close();

    cout << "Bounding box:\n";
    cout << "X: " << min_x << " to " << max_x << " (Width: " << (max_x - min_x) << ")\n";
    cout << "Y: " << min_y << " to " << max_y << " (Height: " << (max_y - min_y) << ")\n";
    cout << "Z: " << min_z << " to " << max_z << " (Depth: " << (max_z - min_z) << ")\n";

    return 0;
}