#include <curses.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <string.h>
#include <fstream>
#include <sstream>
#include <math.h>
#include "render.h"
#include "readGout.h"
#include <menu.h>
#include <panel.h>
// Read the xyz file and store it in a vector
std::vector<std::string> readXYZFile(std::string filename)
{
    std::vector<std::string> xyzline;
    std::string line;
    std::ifstream xyzFile;
    xyzFile.open(filename.c_str(), std::ios::in);
    int i = 0;
    if (xyzFile.is_open())
    {
        while (getline(xyzFile, line))
        {
            i++;
            if (i == 1 || i == 2 || line.empty())
            {
                continue;
            }
            xyzline.push_back(line);
            
        }
        xyzFile.close();
    }
    else
    {
        std::cout << "Unable to open file";
    }
    return xyzline;
}

// Get the number of atoms in the xyz file
int getNumAtoms(std::string filename)
{
    std::string line;
    std::ifstream xyzFile;
    xyzFile.open(filename.c_str(), std::ios::in);
    int i = 0;
    int numAtoms = 0;
    getline(xyzFile, line);
    std::cout << line << std::endl;
    numAtoms = std::stoi(line);

    return numAtoms;
}

// Get the element name
std::vector<std::string> getElementName(std::vector<std::string> line)
{
    std::vector<std::string> elementName;
    int i = 0;
    for (i = 0; i < line.size(); i++)
    {
        std::string temp = line[i];
        std::string delimiter = "  ";
        std::string token = temp.substr(0, temp.find(delimiter));
        //std::cout << temp << std::endl;
        //std::cout << temp.find(delimiter) << std::endl;
        elementName.push_back(token);
    }
    return elementName;

}

// Get the coordinates
std::vector<std::vector<double>> getCoordinates(std::vector<std::string> line)
{
    // extract the coordinates from the line
    // eg. C 0.000000 0.000000 0.000000
    std::vector<std::vector<double>> coordinates;
    for (int i = 0; i < line.size(); i++)
    {
        double x, y, z;
        std::stringstream ss(line[i]);
        std::string symbol;
        ss >> symbol >> x >> y >> z;
        std::vector<double> temp;
        temp.push_back(x);
        temp.push_back(y);
        temp.push_back(z);
        coordinates.push_back(temp);

    }
    return coordinates;
}

// Draw the optimization criteria
WINDOW *drawOptimizationCriteria(int x, int y, int height, std::string type, std::vector<double> value_original)
{
    WINDOW* win = newwin(height, 40, x, y);
    box(win, 0, 0);
    
    mvwprintw(win, 1, 18, type.c_str());

    int n_frames = value_original.size();
    //std::cout << n_frames << std::endl;
    // Copy the value vector to a new vector
    std::vector<double> value;
    for (int i = 0; i < n_frames; i++)
    {
        value.push_back(std::sqrt(value_original[i]));
    }
    //std::cout << value[0] << std::endl;

    double scale;
    double max = 0;
    for (int i = 0; i < n_frames; i++)
    {
        //std::cout << value[i] << std::endl;
        if (value[i] > max)
        {
            max = value[i];
        }
    }
    double min = max;
    for (int i = 0; i < n_frames; i++)
    {
        if (value[i] < min)
        {
            min = value[i];
        }
    }
    scale = (height-3) / (max - min);
    std::vector<std::vector<char>> canvas;
    for (int i = 0; i < height; i++)
    {
        std::vector<char> temp;
        for (int j = 0; j < n_frames; j++)
        {
            temp.push_back(' ');
        }
        canvas.push_back(temp);
    }
    for (int i = 0; i < n_frames; i++)
    {
        int x = i;
        int y = height-2 - value[i] * scale;
        //std::cout << x << std::endl;
        canvas[y][x] = '*';
        for (int j = height-2; j > y; j--)
        {
            if (j == y)
            {
                continue;
            }
            canvas[j][x] = '*';
        }
    }
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < n_frames; j++)
        {
            mvwprintw(win, i + 2, j + 1, "%c", canvas[i][j]);
        }
    }
    wrefresh(win);
    return win;
}

// Draw the optimization criteria sections
std::vector<WINDOW*> drawOptimizationCriteriaSections(int x, int y, int height, std::vector<std::vector<double>> value)
{
    std::vector<WINDOW*> wins;
    std::vector<std::string> type = { "MF", "RF", "MD", "RD" };
    std::vector<double> MF;
    int n_frames = value.size();
    for (int i = 0; i < n_frames; i++)
    {
        MF.push_back(value[i][0]);
    }
    std::vector<double> RF;
    for (int i = 0; i < n_frames; i++)
    {
        RF.push_back(value[i][1]);
    }
    std::vector<double> MD;
    for (int i = 0; i < n_frames; i++)
    {
        MD.push_back(value[i][2]);
    }
    std::vector<double> RD;
    for (int i = 0; i < n_frames; i++)
    {
        RD.push_back(value[i][3]);
    }
    std::vector<std::vector<double>> value2 = { MF, RF, MD, RD };

    for (int i = 0; i < type.size(); i++)
    {
        WINDOW* temp = drawOptimizationCriteria(x+(height+1)*i, y, height, type[i], value2[i]);
        //wbkgd(temp, COLOR_PAIR(2));
        wins.push_back(temp);
    }
    return wins;
}

// Creat a panel for saving the files
WINDOW *createPanel(int x, int y, int height, int width)
{
    WINDOW* win = newwin(height, width, x, y);
    box(win, 0, 0);
    wprintw(win, "Input the file name:");
    wmove(win, height/2, 1);
    echo();
    //wgetstr(win, fileName);

    wrefresh(win);
    return win;
}





int main(int argc, char *argv[])
{
    double scale_ratio = 0.6;
    int x_offset = 0;
    int y_offset = 0;
    #ifdef XYZ
    std::vector<std::string> xyzFile = readXYZFile("CH4.xyz");

    int numAtoms = getNumAtoms("CH4.xyz");

    std::vector<std::string> elementName = getElementName(xyzFile);

    std::vector<std::vector<double>> coordinates = getCoordinates(xyzFile);

    std::vector<std::vector<int>> bondConnectivity = getBondConnectivity(coordinates);

    std::vector<std::vector<char>> renderResult = render(elementName, coordinates, bondConnectivity, 100, 100,scale_ratio,x_offset,y_offset);
   
    initscr();
    noecho();
    int x, y;
    x = LINES;
    y = COLS;
    renderResult = render(elementName, coordinates, bondConnectivity, x, y, scale_ratio, x_offset, y_offset);
    printCanvas(renderResult);
    refresh();
    while (int key = getch())
    {
        if (key == 'q')
        {
            endwin();
            return 0;
        }
        else if (key == 'w')
        {
            coordinates = rotate(coordinates, 0, 2.0*3.1415/180);
            //clear();
            renderResult = render(elementName, coordinates, bondConnectivity, x, y, scale_ratio, x_offset, y_offset);
            printCanvas(renderResult);
            refresh();
        }
        else if (key == 's')
        {
            coordinates = rotate(coordinates, 0, -2.0*3.1415/180);
            //clear();
            renderResult = render(elementName, coordinates, bondConnectivity, x, y, scale_ratio, x_offset, y_offset);
            printCanvas(renderResult);
            refresh();
        }
        else if (key == 'a')
        {
            coordinates = rotate(coordinates, 2.0*3.1415/180, 0);
            //clear();
            renderResult = render(elementName, coordinates, bondConnectivity, x, y, scale_ratio, x_offset, y_offset);
            printCanvas(renderResult);
            refresh();
        }
        else if (key == 'd')
        {
            coordinates = rotate(coordinates, -2.0*3.1415/180, 0);
            //clear();
            renderResult = render(elementName, coordinates, bondConnectivity, x, y, scale_ratio, x_offset, y_offset);
            printCanvas(renderResult);
            refresh();
        }
        else if (key == 'z')
        {
            scale_ratio += 0.05;
            //clear();
            renderResult = render(elementName, coordinates, bondConnectivity, x, y, scale_ratio, x_offset, y_offset);
            printCanvas(renderResult);
            refresh();
        }
        else if (key == 'x')
        {
            scale_ratio -= 0.05;
            //clear();
            renderResult = render(elementName, coordinates, bondConnectivity, x, y, scale_ratio, x_offset, y_offset);
            printCanvas(renderResult);
            refresh();
        }
        else if (key == 'i')
        {
            y_offset -= 1;
            //clear();
            renderResult = render(elementName, coordinates, bondConnectivity, x, y, scale_ratio, x_offset, y_offset);
            printCanvas(renderResult);
            refresh();
        }
        else if (key == 'k')
        {
            y_offset += 1;
            //clear();
            renderResult = render(elementName, coordinates, bondConnectivity, x, y, scale_ratio, x_offset, y_offset);
            printCanvas(renderResult);
            refresh();
        }
        else if (key == 'j')
        {
            x_offset -= 1;
            //clear();
            renderResult = render(elementName, coordinates, bondConnectivity, x, y, scale_ratio, x_offset, y_offset);
            printCanvas(renderResult);
            refresh();

        }
        else if (key == 'l')
        {
            x_offset += 1;
            //clear();
            renderResult = render(elementName, coordinates, bondConnectivity, x, y, scale_ratio, x_offset, y_offset);
            printCanvas(renderResult);
            refresh();
        }
        else if (key == 'r')
        {
            scale_ratio = 0.6;
            x_offset = 0;
            y_offset = 0;
            //clear();
            coordinates = getCoordinates(xyzFile);
            renderResult = render(elementName, coordinates, bondConnectivity, x, y, scale_ratio, x_offset, y_offset);
            printCanvas(renderResult);
            refresh();
        }

    }
    #endif

    //#ifdef GOUT
    std::string filename;
    try{
    filename = std::string(argv[1]);
    }catch (const std::exception& e) {
        std::cout << "Error: Please input the file name" << std::endl;
        return 0;
    }
    std::vector<std::vector<std::vector<double>>> frames = readGoutFile(filename);
    int nFrames = frames.size();
    int currentFrame = 0;
    std::vector<std::vector<double>> coordinates = frames[0];
    std::vector<std::vector<int>> bondConnectivity = getBondConnectivity(coordinates);
    std::vector<std::string> elementName = getElementName_GOUT(filename);
    bool ifOPT = false;
    std::vector<std::vector<double>> optCriteria;
    std::string keyword =  getKeyword(filename);
    std::vector<std::string> chargeMult = getChargeMult(filename);
    if (nFrames > 1) {
        optCriteria = getOptCriteria(filename);
        int nFrames_opt = optCriteria.size();
        if (nFrames_opt < nFrames){
            nFrames = nFrames_opt;
        }
        ifOPT = true;
    }


    std::vector<std::vector<char>> renderResult = render(elementName, coordinates, bondConnectivity, 100, 100, scale_ratio, x_offset, y_offset);
    initscr();
    noecho();
    start_color();

    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_RED);
    init_pair(3, COLOR_GREEN, COLOR_GREEN);
    init_pair(4, COLOR_YELLOW, COLOR_GREEN);

    bkgd(COLOR_PAIR(1));
    keypad(stdscr, TRUE);
    int x, y;
    x = LINES;
    y = COLS;

    int win_height = x/4.5;

    renderResult = render(elementName, coordinates, bondConnectivity, x, y, scale_ratio, x_offset, y_offset);
    printCanvas(renderResult);
    mvprintw(0, 0, "Frame: %d/%d", currentFrame, nFrames);
    mvprintw(x-1, 0, "Press 'q' to quit, 'r' to reset, 'w' to rotate up, 's' to rotate down, 'a' to rotate left, 'd' to rotate right, 'z' to zoom in, 'x' to zoom out, 'i' to move up, 'k' to move down, 'j' to move left, 'l' to move right, 'n' to go to next frame, 'p' to go to previous frame");
    refresh();
    

    if (ifOPT)
    {
        mvprintw(x-2, 0, "Optimization criteria: MF %.2f RF %.2f MD %.2f RD %.2f", optCriteria[currentFrame][0], optCriteria[currentFrame][1], optCriteria[currentFrame][2], optCriteria[currentFrame][3]);

    }
    //WINDOW* win = drawOptimizationCriteria(1, y-40,"RF", optCriteria[currentFrame]);
    std::vector<WINDOW*> wins = drawOptimizationCriteriaSections(1, y-40, win_height, optCriteria);
    refresh();
    while (int key = getch())
    {
        if (key == 'q')
        {
            endwin();
            return 0;
        }
        else if (key == 'w')
        {
            coordinates = rotate(coordinates, 0, 2.0*3.1415/180);
            //clear();
            renderResult = render(elementName, coordinates, bondConnectivity, x, y, scale_ratio, x_offset, y_offset);
            printCanvas(renderResult);
            mvprintw(0, 0, "Frame: %d/%d", currentFrame, nFrames);
            mvprintw(x-1, 0, "Press 'q' to quit, 'r' to reset, 'w' to rotate up, 's' to rotate down, 'a' to rotate left, 'd' to rotate right, 'z' to zoom in, 'x' to zoom out, 'i' to move up, 'k' to move down, 'j' to move left, 'l' to move right, 'n' to go to next frame, 'p' to go to previous frame");
            if (ifOPT)
            {
                mvprintw(x-2, 0, "Optimization criteria: MF %.2f RF %.2f MD %.2f RD %.2f", optCriteria[currentFrame][0], optCriteria[currentFrame][1], optCriteria[currentFrame][2], optCriteria[currentFrame][3]);
            }
            refresh();
            std::vector<WINDOW*> wins = drawOptimizationCriteriaSections(1, y-40, win_height, optCriteria);
            
        }
        else if (key == 's')
        {
            coordinates = rotate(coordinates, 0, -2.0*3.1415/180);
            //clear();
            renderResult = render(elementName, coordinates, bondConnectivity, x, y, scale_ratio, x_offset, y_offset);
            printCanvas(renderResult);
            mvprintw(0, 0, "Frame: %d/%d", currentFrame, nFrames);
            mvprintw(x-1, 0, "Press 'q' to quit, 'r' to reset, 'w' to rotate up, 's' to rotate down, 'a' to rotate left, 'd' to rotate right, 'z' to zoom in, 'x' to zoom out, 'i' to move up, 'k' to move down, 'j' to move left, 'l' to move right, 'n' to go to next frame, 'p' to go to previous frame");
            if (ifOPT)
            {
                mvprintw(x-2, 0, "Optimization criteria: MF %.2f RF %.2f MD %.2f RD %.2f", optCriteria[currentFrame][0], optCriteria[currentFrame][1], optCriteria[currentFrame][2], optCriteria[currentFrame][3]);
            }
            refresh();
            std::vector<WINDOW*> wins = drawOptimizationCriteriaSections(1, y-40, win_height, optCriteria);
        }
        else if (key == 'a')
        {
            coordinates = rotate(coordinates, 2.0*3.1415/180, 0);
            //clear();
            renderResult = render(elementName, coordinates, bondConnectivity, x, y, scale_ratio, x_offset, y_offset);
            printCanvas(renderResult);
            mvprintw(0, 0, "Frame: %d/%d", currentFrame, nFrames);
            mvprintw(x-1, 0, "Press 'q' to quit, 'r' to reset, 'w' to rotate up, 's' to rotate down, 'a' to rotate left, 'd' to rotate right, 'z' to zoom in, 'x' to zoom out, 'i' to move up, 'k' to move down, 'j' to move left, 'l' to move right, 'n' to go to next frame, 'p' to go to previous frame");
            if (ifOPT)
            {
                mvprintw(x-2, 0, "Optimization criteria: MF %.2f RF %.2f MD %.2f RD %.2f", optCriteria[currentFrame][0], optCriteria[currentFrame][1], optCriteria[currentFrame][2], optCriteria[currentFrame][3]);
            }
            refresh();
            std::vector<WINDOW*> wins = drawOptimizationCriteriaSections(1, y-40, win_height, optCriteria);
        }
        else if (key == 'd')
        {
            coordinates = rotate(coordinates, -2.0*3.1415/180, 0);
            //clear();
            renderResult = render(elementName, coordinates, bondConnectivity, x, y, scale_ratio, x_offset, y_offset);
            printCanvas(renderResult);
            mvprintw(0, 0, "Frame: %d/%d", currentFrame, nFrames);
            mvprintw(x-1, 0, "Press 'q' to quit, 'r' to reset, 'w' to rotate up, 's' to rotate down, 'a' to rotate left, 'd' to rotate right, 'z' to zoom in, 'x' to zoom out, 'i' to move up, 'k' to move down, 'j' to move left, 'l' to move right, 'n' to go to next frame, 'p' to go to previous frame");
            if (ifOPT)
            {
                mvprintw(x-2, 0, "Optimization criteria: MF %.2f RF %.2f MD %.2f RD %.2f", optCriteria[currentFrame][0], optCriteria[currentFrame][1], optCriteria[currentFrame][2], optCriteria[currentFrame][3]);
            }
            refresh();
            std::vector<WINDOW*> wins = drawOptimizationCriteriaSections(1, y-40, win_height, optCriteria);
        }
        else if (key == 'z')
        {
            scale_ratio += 0.05;
            //clear();
            renderResult = render(elementName, coordinates, bondConnectivity, x, y, scale_ratio, x_offset, y_offset);
            printCanvas(renderResult);
            mvprintw(0, 0, "Frame: %d/%d", currentFrame, nFrames);
            mvprintw(x-1, 0, "Press 'q' to quit, 'r' to reset, 'w' to rotate up, 's' to rotate down, 'a' to rotate left, 'd' to rotate right, 'z' to zoom in, 'x' to zoom out, 'i' to move up, 'k' to move down, 'j' to move left, 'l' to move right, 'n' to go to next frame, 'p' to go to previous frame");
            if (ifOPT)
            {
                mvprintw(x-2, 0, "Optimization criteria: MF %.2f RF %.2f MD %.2f RD %.2f", optCriteria[currentFrame][0], optCriteria[currentFrame][1], optCriteria[currentFrame][2], optCriteria[currentFrame][3]);
            }
            refresh();
            std::vector<WINDOW*> wins = drawOptimizationCriteriaSections(1, y-40, win_height, optCriteria);
        }
        else if (key == 'x')
        {
            scale_ratio -= 0.05;
            //clear();
            renderResult = render(elementName, coordinates, bondConnectivity, x, y, scale_ratio, x_offset, y_offset);
            printCanvas(renderResult);
            mvprintw(0, 0, "Frame: %d/%d", currentFrame, nFrames);
            mvprintw(x-1, 0, "Press 'q' to quit, 'r' to reset, 'w' to rotate up, 's' to rotate down, 'a' to rotate left, 'd' to rotate right, 'z' to zoom in, 'x' to zoom out, 'i' to move up, 'k' to move down, 'j' to move left, 'l' to move right, 'n' to go to next frame, 'p' to go to previous frame");
            if (ifOPT)
            {
                mvprintw(x-2, 0, "Optimization criteria: MF %.2f RF %.2f MD %.2f RD %.2f", optCriteria[currentFrame][0], optCriteria[currentFrame][1], optCriteria[currentFrame][2], optCriteria[currentFrame][3]);
            }
            refresh();
            std::vector<WINDOW*> wins = drawOptimizationCriteriaSections(1, y-40, win_height, optCriteria);
        }
        else if (key == 'i')
        {
            y_offset -= 1;
            //clear();
            renderResult = render(elementName, coordinates, bondConnectivity, x, y, scale_ratio, x_offset, y_offset);
            printCanvas(renderResult);
            mvprintw(0, 0, "Frame: %d/%d", currentFrame, nFrames);
            mvprintw(x-1, 0, "Press 'q' to quit, 'r' to reset, 'w' to rotate up, 's' to rotate down, 'a' to rotate left, 'd' to rotate right, 'z' to zoom in, 'x' to zoom out, 'i' to move up, 'k' to move down, 'j' to move left, 'l' to move right, 'n' to go to next frame, 'p' to go to previous frame");
            if (ifOPT)
            {
                mvprintw(x-2, 0, "Optimization criteria: MF %.2f RF %.2f MD %.2f RD %.2f", optCriteria[currentFrame][0], optCriteria[currentFrame][1], optCriteria[currentFrame][2], optCriteria[currentFrame][3]);
            }
            refresh();
            std::vector<WINDOW*> wins = drawOptimizationCriteriaSections(1, y-40, win_height, optCriteria);
        }
        else if (key == 'k')
        {
            y_offset += 1;
            //clear();
            renderResult = render(elementName, coordinates, bondConnectivity, x, y, scale_ratio, x_offset, y_offset);
            printCanvas(renderResult);
            mvprintw(0, 0, "Frame: %d/%d", currentFrame, nFrames);
            mvprintw(x-1, 0, "Press 'q' to quit, 'r' to reset, 'w' to rotate up, 's' to rotate down, 'a' to rotate left, 'd' to rotate right, 'z' to zoom in, 'x' to zoom out, 'i' to move up, 'k' to move down, 'j' to move left, 'l' to move right, 'n' to go to next frame, 'p' to go to previous frame");
            if (ifOPT)
            {
                mvprintw(x-2, 0, "Optimization criteria: MF %.2f RF %.2f MD %.2f RD %.2f", optCriteria[currentFrame][0], optCriteria[currentFrame][1], optCriteria[currentFrame][2], optCriteria[currentFrame][3]);
            }
            refresh();
            std::vector<WINDOW*> wins = drawOptimizationCriteriaSections(1, y-40, win_height, optCriteria);
        }

        else if (key == 'j')
        {
            x_offset -= 1;
            //clear();
            renderResult = render(elementName, coordinates, bondConnectivity, x, y, scale_ratio, x_offset, y_offset);
            printCanvas(renderResult);
            mvprintw(0, 0, "Frame: %d/%d", currentFrame, nFrames);
            mvprintw(x-1, 0, "Press 'q' to quit, 'r' to reset, 'w' to rotate up, 's' to rotate down, 'a' to rotate left, 'd' to rotate right, 'z' to zoom in, 'x' to zoom out, 'i' to move up, 'k' to move down, 'j' to move left, 'l' to move right, 'n' to go to next frame, 'p' to go to previous frame");
            if (ifOPT)
            {
                mvprintw(x-2, 0, "Optimization criteria: MF %.2f RF %.2f MD %.2f RD %.2f", optCriteria[currentFrame][0], optCriteria[currentFrame][1], optCriteria[currentFrame][2], optCriteria[currentFrame][3]);
            }
            refresh();
            std::vector<WINDOW*> wins = drawOptimizationCriteriaSections(1, y-40, win_height, optCriteria);

        }
        else if (key == 'l')
        {
            x_offset += 1;
            //clear();
            renderResult = render(elementName, coordinates, bondConnectivity, x, y, scale_ratio, x_offset, y_offset);
            printCanvas(renderResult);
            mvprintw(0, 0, "Frame: %d/%d", currentFrame, nFrames);
            mvprintw(x-1, 0, "Press 'q' to quit, 'r' to reset, 'w' to rotate up, 's' to rotate down, 'a' to rotate left, 'd' to rotate right, 'z' to zoom in, 'x' to zoom out, 'i' to move up, 'k' to move down, 'j' to move left, 'l' to move right, 'n' to go to next frame, 'p' to go to previous frame");
            if (ifOPT)
            {
                mvprintw(x-2, 0, "Optimization criteria: MF %.2f RF %.2f MD %.2f RD %.2f", optCriteria[currentFrame][0], optCriteria[currentFrame][1], optCriteria[currentFrame][2], optCriteria[currentFrame][3]);
            }
            refresh();
            std::vector<WINDOW*> wins = drawOptimizationCriteriaSections(1, y-40, win_height, optCriteria);
        }
        else if (key == KEY_LEFT)
        {
            if (currentFrame == 0)
            {
                currentFrame = 0;
            }
            else
            {
                currentFrame -= 1;
            }
            coordinates = frames[currentFrame];
            bondConnectivity = getBondConnectivity(coordinates);
            renderResult = render(elementName, coordinates, bondConnectivity, x, y, scale_ratio, x_offset, y_offset);
            printCanvas(renderResult);
            mvprintw(0, 0, "Frame: %d/%d", currentFrame, nFrames);
            mvprintw(x-1, 0, "Press 'q' to quit, 'r' to reset, 'w' to rotate up, 's' to rotate down, 'a' to rotate left, 'd' to rotate right, 'z' to zoom in, 'x' to zoom out, 'i' to move up, 'k' to move down, 'j' to move left, 'l' to move right, 'n' to go to next frame, 'p' to go to previous frame");
            if (ifOPT)
            {
                mvprintw(x-2, 0, "Optimization criteria: MF %.2f RF %.2f MD %.2f RD %.2f", optCriteria[currentFrame][0], optCriteria[currentFrame][1], optCriteria[currentFrame][2], optCriteria[currentFrame][3]);
            }
            refresh();
            std::vector<WINDOW*> wins = drawOptimizationCriteriaSections(1, y-40, win_height, optCriteria);
        }
        else if (key == KEY_RIGHT)
        {
            if (currentFrame == nFrames - 1)
            {
                currentFrame = nFrames - 1;
            }
            else
            {
                currentFrame += 1;
            }
            coordinates = frames[currentFrame];
            bondConnectivity = getBondConnectivity(coordinates);
            renderResult = render(elementName, coordinates, bondConnectivity, x, y, scale_ratio, x_offset, y_offset);
            printCanvas(renderResult);
            mvprintw(0, 0, "Frame: %d/%d", currentFrame, nFrames);
            mvprintw(x-1, 0, "Press 'q' to quit, 'r' to reset, 'w' to rotate up, 's' to rotate down, 'a' to rotate left, 'd' to rotate right, 'z' to zoom in, 'x' to zoom out, 'i' to move up, 'k' to move down, 'j' to move left, 'l' to move right, 'n' to go to next frame, 'p' to go to previous frame");
            if (ifOPT)
            {
                mvprintw(x-2, 0, "Optimization criteria: MF %.2f RF %.2f MD %.2f RD %.2f", optCriteria[currentFrame][0], optCriteria[currentFrame][1], optCriteria[currentFrame][2], optCriteria[currentFrame][3]);
            }
            refresh();
            std::vector<WINDOW*> wins = drawOptimizationCriteriaSections(1, y-40, win_height, optCriteria);
        }
        else if (key == 'r')
        {
            scale_ratio = 0.6;
            x_offset = 0;
            y_offset = 0;
            //clear();
            coordinates = frames[0];
            renderResult = render(elementName, coordinates, bondConnectivity, x, y, scale_ratio, x_offset, y_offset);
            printCanvas(renderResult);
            mvprintw(0, 0, "Frame: %d/%d", currentFrame, nFrames);
            mvprintw(x-1, 0, "Press 'q' to quit, 'r' to reset, 'w' to rotate up, 's' to rotate down, 'a' to rotate left, 'd' to rotate right, 'z' to zoom in, 'x' to zoom out, 'i' to move up, 'k' to move down, 'j' to move left, 'l' to move right, 'n' to go to next frame, 'p' to go to previous frame");
            if (ifOPT)
            {
                mvprintw(x-2, 0, "Optimization criteria: MF %.2f RF %.2f MD %.2f RD %.2f", optCriteria[currentFrame][0], optCriteria[currentFrame][1], optCriteria[currentFrame][2], optCriteria[currentFrame][3]);
            }
            refresh();
            std::vector<WINDOW*> wins = drawOptimizationCriteriaSections(1, y-40, win_height, optCriteria);
        }
        else if (key == KEY_F(1))
        {
            WINDOW* save_win = createPanel(x/2, y/2, win_height, 30);
            char saveName[100];
            wgetstr(save_win, saveName);
            wprintw(save_win, "Saving to %s", saveName);
            
            wclear(save_win);
            delwin(save_win);
            writeGJF(std::string(saveName), elementName, frames[currentFrame] , keyword, chargeMult[0], chargeMult[1]);
            printCanvas(renderResult);
            mvprintw(0, 0, "Frame: %d/%d", currentFrame, nFrames);
            mvprintw(x-1, 0, "Press 'q' to quit, 'r' to reset, 'w' to rotate up, 's' to rotate down, 'a' to rotate left, 'd' to rotate right, 'z' to zoom in, 'x' to zoom out, 'i' to move up, 'k' to move down, 'j' to move left, 'l' to move right, 'n' to go to next frame, 'p' to go to previous frame");
            if (ifOPT)
            {
                mvprintw(x-2, 0, "Optimization criteria: MF %.2f RF %.2f MD %.2f RD %.2f", optCriteria[currentFrame][0], optCriteria[currentFrame][1], optCriteria[currentFrame][2], optCriteria[currentFrame][3]);
            }
            refresh();
            std::vector<WINDOW*> wins = drawOptimizationCriteriaSections(1, y-40, win_height, optCriteria);
        }
       

    }    
    //#endif

    return 0;
}