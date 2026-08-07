#include <iostream>

class WordCount {

public:

    int execute(std::string_view input) const
    {
        int word_count = input.empty() ? 0 : 1;

        for (size_t i = 0; i < input.size(); ++i) {
            if (input[i] == ' ') {
                word_count++;
            }
        }

        return word_count;
    }

};


struct plugin_handle {
    WordCount impl;
};