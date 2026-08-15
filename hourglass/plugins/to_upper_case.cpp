#include <cctype>
#include <cstring>
#include <memory>
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

    void set_result(std::string result) {
        _result = std::make_unique<std::string>(result);
    }

    std::string &get_result() const { return *_result; }

  private:
    std::unique_ptr<std::string> _result;
};

struct plugin_handle {
    ToUpperCase impl;
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

        h->impl.set_result(h->impl.execute(input));

        out->type = PLUGIN_TYPE_STRING;
        out->value.val_str.data = h->impl.get_result().c_str();

        const auto size = h->impl.get_result().size();
        if (size > INT32_MAX) {
            return status::OTHER_ERR;
        }

        out->value.val_str.len = static_cast<int32_t>(size);

        return status::OK;
    } catch (...) {
        return status::OTHER_ERR;
    }
}
