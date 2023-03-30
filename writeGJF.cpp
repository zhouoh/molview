#include <curses.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <string.h>
#include <fstream>
#include <sstream>
#include <math.h>
#include "writeGJF.h"



// Write the Gaussian input file
void writeGJF(std::string filename, std::vector<std::string> elementName, std::vector<std::vector<double>> coordinates, std::string keyword, std::string charge, std::string multiplicity)
{
    std::ofstream outfile(filename);
    if (!outfile) {
        std::cout << "Error: Could not open output file" << std::endl;
    }
    prefix = filename.substr(0, filename.find_last_of("."));
    outfile << "%chk=" << prefix << ".chk" << std::endl;
    outfile << "%mem=16GB" << std::endl;
    outfile << "%nprocshared=12" << std::endl;
    outfile << keyword << std::endl;
    outfile << std::endl;
    outfile << "t" << filename << std::endl;
    outfile << std::endl;
    outfile << charge << " " << multiplicity << std::endl;
    for (int i = 0; i < coordinates.size(); i++)
    {
        outfile << elementName[i] << "   " << coordinates[i][0] << " " << coordinates[i][1] << " " << coordinates[i][2] << std::endl;
    }
    outfile << std::endl;
    outfile << std::endl;
    outfile.close();
}