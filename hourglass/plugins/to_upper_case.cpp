#include <iostream>
#include <cctype>

class ToUpperCase
{
public:
    std::string execute(std::string input) const
    {
        for (char& c : input) {
            c = std::toupper(c);
        }
    }
};

struct plugin_handle {
    ToUpperCase impl;
};

extern "C" {
    plugin_handle* plugin_create()
    {
        return new plugin_handle;
    }
}