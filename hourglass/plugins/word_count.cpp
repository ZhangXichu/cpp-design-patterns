#include <iostream>

#include <plugin_api.h>

class WordCount {

  public:
    int execute(std::string_view input) const {
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

plugin_handle *plugin_create() {
    try {
        return new plugin_handle{};
    } catch (...) {
        return nullptr;
    }
}

void plugin_destroy(plugin_handle *plugin) { delete plugin; }

status plugin_execute(plugin_handle *h, const char *input, plugin_result *out) {
    if (!h || !input || !out) {
        return status::INVALID_INPUT;
    }

    try {
        int result = h->impl.execute(input);

        out->type = PLUGIN_TYPE_INTEGER;
        out->value.val_int = result;

        return status::OK;
    } catch (...) {
        return status::OTHER_ERR;
    }
}