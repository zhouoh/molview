#include <cmath>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include "readPDB.h"

using namespace std;



// 计算两个原子之间的最短距离
float minimum_image_distance(const vector<float>& r1, const vector<float>& r2, const vector<float>& box_size) {
    float dx = r1[0] - r2[0];
    float dy = r1[1] - r2[1];
    float dz = r1[2] - r2[2];
    dx -= box_size[0] * round(dx / box_size[0]); // 按照MIC将相邻的周期性图像映射到一个周期性盒子内
    dy -= box_size[1] * round(dy / box_size[1]);
    dz -= box_size[2] * round(dz / box_size[2]);
    return sqrt(dx*dx + dy*dy + dz*dz);
}

// 计算原子之间的距离矩阵
void calculate_distance_matrix(const vector<vector<float>>& positions, vector<vector<float>>& distances, const vector<float>& box) {
    int n_atoms = positions.size();
    distances.resize(n_atoms, vector<float>(n_atoms));
    for (int i = 0; i < n_atoms; i++) {
        for (int j = i+1; j < n_atoms; j++) {
            distances[i][j] = minimum_image_distance(positions[i], positions[j], box);
            distances[j][i] = distances[i][j];
        }
    }
}



vector<vector<int>> GetConnectivy(vector<vector<float>> distances, vector<string> elements) {
    int n_atoms = distances.size();
    vector<vector<int>> connectivity(n_atoms, vector<int>(n_atoms));
    for (int i = 0; i < n_atoms; i++) {
        for (int j = i+1; j < n_atoms; j++) {
            if (distances[i][j] < 1.75) {
                connectivity[i][j] = 1;
                connectivity[j][i] = 1;
            }
        }
    }
    return connectivity;
}





int main() {
    vector<vector<float>> positions = readPDB("MOF.pdb");  
    vector<string> elements = readElements("MOF.pdb");
    vector<float> box = readBox("MOF.pdb"); 
    vector<vector<float>> distances;
    calculate_distance_matrix(positions, distances, box);
    vector<vector<int>> connectivity = GetConnectivy(distances, elements);
    for (int i = 0; i < connectivity.size(); i++) {
        for (int j = 0; j < connectivity[i].size(); j++) {
            cout << connectivity[i][j] << " ";
        }
        cout << endl;
    }

    return 0;
}

