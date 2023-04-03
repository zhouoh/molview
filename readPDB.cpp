#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include "readPDB.h"
using namespace std;

vector<vector<float>> readPDB(string filename) {
    ifstream file(filename);
    string line;
    vector<vector<float>> coords;
    while (getline(file, line)) {
        if (line.substr(0, 6) == "HETATM") {
            float x;
            float y;
            float z;

            stringstream ss(line.substr(30, 24));
            ss >> x >> y >> z;

            coords.push_back({x, y, z});
        }
    }
    return coords;
}

vector<string> readElements(string filename) {
    ifstream file(filename);
    string line;
    vector<string> elements;
    while (getline(file, line)) {
        if (line.substr(0, 6) == "HETATM") {
            string element = line.substr(76, 2);
            elements.push_back(element);
        }
    }
    return elements;
}

vector<float> readBox(string filename) {
    ifstream file(filename);
    string line;
    vector<float> box;
    while (getline(file, line)) {
        if (line.substr(0, 5) == "CRYST") {
            float x;
            float y;
            float z;

            stringstream ss(line.substr(6, 54));
            ss >> x >> y >> z;

            box.push_back(x);
            box.push_back(y);
            box.push_back(z);
        }
    }
    return box;
}

#ifdef PDB
int main() {
    vector<vector<float>> coords = readPDB("MOF.pdb");
    for (auto& atom : coords) {
        cout << atom[0] << " " << atom[1] << " " << atom[2] << endl;
    }
    vector<string> elements = readElements("MOF.pdb");
    for (auto& element : elements) {
        cout << element << endl;
    }
    vector<float> box = readBox("MOF.pdb");
    for (auto& dim : box) {
        cout << dim << endl;
    }
}
#endif