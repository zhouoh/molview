#include <vector>
#include <string>
#include "render.h"
#include <math.h>
#include <ncurses.h>
#include <iostream>
// determin the bond connectivity
std::vector<std::vector<int>> getBondConnectivity(std::vector<std::vector<double>> coordinate)
{
    std::vector<std::vector<int>> bondConnectivity;
    int i = 0;
    int j = 0;
    for (i = 0; i < coordinate.size(); i++)
    {
        std::vector<int> temp;
        for (j = 0; j < coordinate.size(); j++)
        {
            temp.push_back(0);
        }

        bondConnectivity.push_back(temp);
    }
    for (i = 0; i < coordinate.size(); i++)
    {
        double x, y, z;
        x = coordinate[i][0];
        y = coordinate[i][1];
        z = coordinate[i][2];
        for (j = 0; j < coordinate.size(); j++)
        {
            if (i == j)
            {
                continue;
            }

            double x2, y2, z2;
            x2 = coordinate[j][0];
            y2 = coordinate[j][1];
            z2 = coordinate[j][2];
            double distance = sqrt(pow(x - x2, 2) + pow(y - y2, 2) + pow(z - z2, 2));
            if (distance < 1.75)
            {
                bondConnectivity[i][j] = 1;
            }
        }
    }
    return bondConnectivity;
}

// Render the molecule to ASCII
std::vector<std::vector<char>> render(std::vector<std::string> elementName, std::vector<std::vector<double>> coordinates, std::vector<std::vector<int>> bondConnectivity, int H, int W, double scale_ratio, int x_offset, int y_offset)
{
    std::vector<std::vector<char>> canvas;
    int i = 0;
    int j = 0;
    for (i = 0; i < H; i++)
    {
        std::vector<char> temp;
        for (j = 0; j < W; j++)
        {
            temp.push_back(' ');
        }
        canvas.push_back(temp);
    }
    double xMax = 0;
    double yMax = 0;
    double xMin = 0;
    double yMin = 0;
    for (i = 0; i < coordinates.size(); i++)
    {
        if (coordinates[i][0] > xMax)
        {
            xMax = coordinates[i][0];
        }
        if (coordinates[i][0] < xMin)
        {
            xMin = coordinates[i][0];
        }
        if (coordinates[i][1] > yMax)
        {
            yMax = coordinates[i][1];
        }
        if (coordinates[i][1] < yMin)
        {
            yMin = coordinates[i][1];
        }
    }
    double xScale = (W - 2) / (xMax - xMin);
    double yScale = (H - 2) / (yMax - yMin);
    double scale;
    if (xScale < yScale)
    {
        scale = xScale;
    }
    else
    {
        scale = yScale;
    }
    for (i = 0; i < coordinates.size(); i++)
    {
        coordinates[i][0] = coordinates[i][0] * scale * scale_ratio + x_offset;
        coordinates[i][1] = coordinates[i][1] * scale * scale_ratio + y_offset;
    }

    for (i = 0; i < coordinates.size(); i++)
    {
        for (j = 0; j < coordinates.size(); j++)
        {
            if (bondConnectivity[i][j] == 1)
            {
                int x1 = coordinates[i][0] + W / 2;
                int y1 = coordinates[i][1] + H / 2;
                int x2 = coordinates[j][0] + W / 2;
                int y2 = coordinates[j][1] + H / 2;
                double xAngle = std::atan2(y2 - y1, x2 - x1);
                double yAngle = std::atan2(x2 - x1, y2 - y1);
                double x_Step = std::abs(std::cos(xAngle));
                double y_Step = std::abs(std::cos(yAngle));
                //std::cout << x_Step << " " << y_Step << std::endl;

                int x = x1;
                int y = y1;
                double xd = x1;
                double yd = y1;
                while (x != x2 || y != y2)
                {                    
                    if (x < x2)
                    {
                        xd = xd + x_Step;
                        x = (int)std::round(xd);
                    }
                    else if (x > x2)
                    {
                        xd = xd - x_Step;
                        x = (int)std::round(xd);
                    }
                    if (y < y2)
                    {
                        yd = yd + y_Step;
                        y = (int)std::round(yd);
                    }
                    else if (y > y2)
                    {
                        yd = yd - y_Step;
                        y = (int)std::round(yd);
                    }
                    if (x < 0 || x >= W || y < 0 || y >= H)
                    {
                        continue;
                    }
                    canvas[y][x] = '-';
                }
            }
        }

    }
    for (i = 0; i < coordinates.size(); i++)
    {
        int x = coordinates[i][0] + W / 2;
        int y = coordinates[i][1] + H / 2;
        if (x < 0 || x >= W || y < 0 || y >= H)
        {
            continue;
        }
        #ifdef XYZ
        canvas[y][x] = elementName[i][1];
        #endif
        #ifdef GOUT
        canvas[y][x] = elementName[i][0];
        #endif

    }
    return canvas;
}

// Print the canvas using curses
void printCanvas(std::vector<std::vector<char>> canvas)
{
    int i = 0;
    int j = 0;
    for (i = 0; i < canvas.size(); i++)
    {
        for (j = 0; j < canvas[i].size(); j++)
        {
            mvprintw(i, j, "%c", canvas[i][j]);
        }
    }
}

// Rotate the molecule
std::vector<std::vector<double>> rotate(std::vector<std::vector<double>> coordinates, double x_angle, double y_angle)
{
    int i = 0;
    for (i = 0; i < coordinates.size(); i++)
    {
        double x = coordinates[i][0];
        double y = coordinates[i][1];
        double z = coordinates[i][2];
        double x2 = x * std::cos(x_angle) - z * std::sin(x_angle);
        double y2 = y;
        double z2 = x * std::sin(x_angle) + z * std::cos(x_angle);
        double x3 = x2;
        double y3 = y2 * std::cos(y_angle) - z2 * std::sin(y_angle);
        double z3 = y2 * std::sin(y_angle) + z2 * std::cos(y_angle);
        coordinates[i][0] = x3;
        coordinates[i][1] = y3;
        coordinates[i][2] = z3;
    }
    return coordinates;

}
