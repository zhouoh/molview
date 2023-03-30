std::vector<std::vector<std::vector<double>>> readGoutFile(std::string filename);
std::vector<std::string> getElementName_GOUT(std::string filename);
std::vector<std::vector<double>> getOptCriteria(std::string filename);
void writeGJF(std::string filename, std::vector<std::string> elementName, std::vector<std::vector<double>> coordinates, std::string keyword, std::string charge, std::string multiplicity);
std::vector<std::string> getChargeMult(std::string filename);
std::string getKeyword(std::string filename);
