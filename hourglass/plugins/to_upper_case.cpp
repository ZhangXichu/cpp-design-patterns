#include <cctype>
#include <cstring>
#include <plugin_api.h>
#include <iostream>

class ToUpperCase {
  public:
    std::string execute(std::string input) const {
        for (char &c : input) {
            c = std::toupper(c);
        }
    }
};

struct plugin_handle {
    ToUpperCase impl;
};

plugin_handle *plugin_create() { return new plugin_handle{}; }

void destory_handle(plugin_handle *plugin) { delete plugin; }

int plugin_execute(plugin_handle *plugin, const char *input, char *output,
                   std::size_t output_size) {
    if (!plugin || !input || !output) {
        return 1;
    }

    try {
        const std::string result = plugin->impl.execute(input);

        if (result.size() + 1 > output_size) {
            return 2;
        }

        std::memcpy(output, result.c_str(), result.size() + 1);

        return 0;
    } catch (...) {
        return 3;
    }
}
