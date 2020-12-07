#include <iostream>

#include "database.h"

int usage() {
    std::cout << "usage: db [options]" << std::endl;
    exit(1);
}

int main(int argc, char** argv) {
    fs::path basePath = ".";
    if (argc == 2) {
        basePath = argv[1];
    }
    try {
        Database db(basePath);
    }
    catch(const char* err) {
        std::cerr << err << std::endl;
    }
}
