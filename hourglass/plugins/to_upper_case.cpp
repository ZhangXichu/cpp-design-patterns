#include <iostream>
#include <cctype>


int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <string>" << std::endl;
        return 1;
    }

    std::string input = argv[1];
    for (char& c : input) {
        c = std::toupper(c);
    }

    std::cout << input << std::endl;
    return 0;
}