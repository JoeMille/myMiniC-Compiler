#include <iostream>

int main(int argc, char** argv) {
    if(argc < 2) {
        std::cerr << "usage: minic <file.c\n";
        return 1;
    }
    std::cout << "Placeholder Compiler. Will read:" <<argv[1] << "\n";
    return 0;
}