#include <iostream>
#include <plugin.hpp>

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <plugin>" << std::endl;
        return 1;
    }
    Plugin plugin(argv[1]);
    return 0;
}