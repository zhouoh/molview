#include <curses.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <string.h>
#include <fstream>
#include <sstream>
#include <math.h>
#include "readGout.h"

// Read the Gaussian output file and store it in a vector
std::vector<std::vector<std::vector<double>>> readGoutFile(std::string filename)
{
    std::ifstream infile(filename);
    if (!infile) {
        std::cout << "Error: Could not open input file" << std::endl;
    }

    std::vector<double> coords;
    std::vector<std::vector<double>> frame;
    std::vector<std::vector<std::vector<double>>> frames;
    std::string line;
    bool start_block = false;
    bool start_parsing = false;
    int flag = 0;
    while (getline(infile, line)) {

        if (line.find("Input orientation:") != std::string::npos) {
            start_block = true;
            continue;
        }
        if (start_block && flag < 3) {
            flag++;            
            continue;
        }
        else if (start_block && flag == 3) {
            start_parsing = true;
            flag = 5;
            continue;
        }
        
        if (start_parsing) {
            //std::cout << "line" << std::endl;
            std::istringstream iss(line);
            int atom_num, atomic_num;
            std::string atom_type;
            double x, y, z;
            if (iss >> atom_num >> atomic_num >> atom_type >> x >> y >> z) {
                coords.push_back(x);
                coords.push_back(y);
                coords.push_back(z);
                frame.push_back(coords);
                coords.clear();
            } else {

                frames.push_back(frame);
                frame.clear();
                start_parsing = false;
                start_block = false;
                flag = 0;
            }
        }

    }

    infile.close();



    return frames;
}

//Get the element name
std::vector<std::string> getElementName_GOUT(std::string filename)
{
    std::vector<std::string> elementName;
    std::ifstream infile(filename);
    if (!infile) {
        std::cout << "Error: Could not open input file" << std::endl;
    }
    bool start_block = false;
    bool start_parsing = false;
    int flag = 0;
    std::string line;
    std::vector<std::string> perodic_table = {"H","He","Li","Be","B","C","N","O","F","Ne","Na","Mg","Al","Si","P","S","Cl","Ar","K","Ca","Sc","Ti","V","Cr","Mn","Fe","Co","Ni","Cu","Zn","Ga","Ge","As","Se","Br","Kr","Rb","Sr","Y","Zr","Nb","Mo","Tc","Ru","Rh","Pd","Ag","Cd","In","Sn","Sb","Te","I","Xe","Cs","Ba","La","Ce","Pr","Nd","Pm","Sm","Eu","Gd","Tb","Dy","Ho","Er","Tm","Yb","Lu","Hf","Ta","W","Re","Os","Ir","Pt","Au","Hg","Tl","Pb","Bi","Po","At","Rn","Fr","Ra","Ac","Th","Pa","U","Np","Pu","Am","Cm","Bk","Cf","Es","Fm","Md","No","Lr","Rf","Db","Sg","Bh","Hs","Mt","Ds","Rg","Cn","Nh","Fl","Mc","Lv","Ts","Og"};
    while (getline(infile, line)) {

        if (line.find("Input orientation:") != std::string::npos) {
            start_block = true;
            continue;
        }
        if (start_block && flag < 3) {
            flag++;            
            continue;
        }
        else if (start_block && flag == 3) {
            start_parsing = true;
            flag = 5;
            continue;
        }
         if (start_parsing) {

            std::istringstream iss(line);
            int atom_num, atomic_num;
            std::string atom_type;
            double x, y, z;
            if (iss >> atom_num >> atomic_num >> atom_type >> x >> y >> z) {
                elementName.push_back(perodic_table[atomic_num-1]);
            } 
             else {
                break;

            }
        }  
    } 
    return elementName;    
        
}

// Get the optimaztion criteria
std::vector<std::vector<double>> getOptCriteria(std::string filename)
{
    std::vector<std::vector<double>> optCriteria;
    std::ifstream infile(filename);
    if (!infile) {
        std::cout << "Error: Could not open input file" << std::endl;
    }
    bool start_parsing = false;
    std::vector<double> temp;
    int flag = 0;
    std::string line;
    while (getline(infile, line)) {

        if (line.find("Converged?") != std::string::npos) {
            start_parsing = true;
            continue;
        }
         if (start_parsing) {
            //std::cout << "line" << std::endl;
            std::istringstream iss(line);
            std::string MR, FD, F;
            double c,thereshold;
            if (iss >> MR >> FD >> c >> thereshold >> F) {
                temp.push_back(c/thereshold);                                
            } 
             else {
                optCriteria.push_back(temp);
                temp.clear();
                start_parsing = false;

            }
        }  
    } 
    return optCriteria;    
        
}


// Get the keyword
std::string getKeyword(std::string filename)
{
    std::string keyword;
    std::ifstream infile(filename);
    if (!infile) {
        std::cout << "Error: Could not open input file" << std::endl;
    }
   
    std::string line;
    while (getline(infile, line)) {

        if (line.find(" #") != std::string::npos) {
            keyword = line;
            keyword = keyword.erase(0,1);
            break;
        }
    } 
    return keyword;    
        
}


// Get the charge and multiplicity
std::vector<std::string> getChargeMult(std::string filename)
{
    std::ifstream input_file(filename);
    std::string line;
    int charge, multiplicity;
    std::vector<std::string> chargeMult;

    while (std::getline(input_file, line)) {
        if (line.find("Charge =") != std::string::npos) {
            // Extract charge and multiplicity from the line
            std::stringstream ss(line.substr(line.find("Charge =") + 9));
            ss >> charge;

            std::stringstream ss2(line.substr(line.find("Multiplicity =") + 15));
            ss2 >> multiplicity;
            break;


        }
    }

    input_file.close();   
    chargeMult.push_back(std::to_string(charge));
    chargeMult.push_back(std::to_string(multiplicity));
    return chargeMult;

        
}



void writeGJF(std::string filename, std::vector<std::string> elementName, std::vector<std::vector<double>> coordinates, std::string keyword, std::string charge, std::string multiplicity)
{
    std::ofstream outfile(filename);
    if (!outfile) {
        std::cout << "Error: Could not open output file" << std::endl;
    }
    std::string prefix = filename.substr(0, filename.find_last_of("."));
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


//int main()
//{
//    std::string filename = "TS4.out";
//    std::vector<std::vector<std::vector<double>>> frames = readGoutFile(filename);
//    std::cout << "Number of frames: " << frames.size() << std::endl;
//    std::cout << "Number of atoms: " << frames[0].size() << std::endl;
//    std::cout << "Number of coordinates: " << frames[0][0].size() << std::endl;
//    std::cout << "Coordinates: " << frames[0][0][0] << std::endl;
//    std::vector<std::string> elementName = getElementName_GOUT(filename);
//    for (std::string element : elementName) {
//        std::cout << element << std::endl;
//    }
//    std::vector<std::vector<double>> optCriteria = getOptCriteria(filename);
//    for (std::vector<double> criteria : optCriteria) {
//        for (double c : criteria) {
//            std::cout << c << " ";
//        }
//        std::cout << std::endl;
//    }
//    std::string keyword = getKeyword(filename);
//    std::cout << keyword << std::endl;
//    std::vector<std::string> chargeMult = getChargeMult(filename);
//    for (std::string c : chargeMult) {
//        std::cout << c << std::endl;
//    }
//    writeGJF("test1.gjf", elementName, frames[0], keyword, chargeMult[0], chargeMult[1]);
//    return 0;
//}

