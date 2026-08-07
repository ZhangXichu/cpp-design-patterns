#include <iostream>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <string>" << std::endl;
        return 1;
    }

    std::string input = argv[1];

    int word_count = input.empty() ? 0 : 1;

    for (size_t i = 0; i < input.size(); ++i) {
        if (input[i] == ' ') {
            word_count++;
        }
    }
    std::cout << word_count << std::endl;
    return 0;
}