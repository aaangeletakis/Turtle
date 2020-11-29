#include "io.hpp"

void getfile(const char *filename, std::string &data) {
    std::ifstream file(filename);
    file >> data;
    file.close();
}

void writefile(const char *filename, std::string &data) {
    std::ofstream file(filename);
    file << data;
    file.close();
}
