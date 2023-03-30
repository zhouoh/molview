std::vector<std::vector<int>> getBondConnectivity(std::vector<std::vector<double>> coordinate);
std::vector<std::vector<char>> render(std::vector<std::string> elementName, std::vector<std::vector<double>> coordinates, std::vector<std::vector<int>> bondConnectivity, int H, int W, double scale_ratio, int x_offset, int y_offset);
std::vector<std::vector<double>> rotate(std::vector<std::vector<double>> coordinates, double x_angle, double y_angle);
void printCanvas(std::vector<std::vector<char>> canvas);