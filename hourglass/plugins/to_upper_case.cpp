#include <cctype>
#include <cstring>
#include <string>

#include <plugin_api.h>

class ToUpperCase {
  public:
    std::string execute(std::string input) const {
        for (char &c : input) {
            c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
        }
        return input;
    }
};

struct plugin_handle {
    ToUpperCase impl;
};

plugin_handle *plugin_create() { return new plugin_handle{}; }

void plugin_destroy(plugin_handle *plugin) { delete plugin; }

int plugin_execute(plugin_handle *h, const char *input, plugin_result *out) {
    if (!h || !input || !out) {
        return 1;
    }

    try {
        const std::string result = h->impl.execute(input);

        out->type = PLUGIN_TYPE_STRING;
        out->value.val_str.data = result.c_str();
        out->value.val_str.len = result.size();

        return 0;
    } catch (...) {
        return 3;
    }
}
