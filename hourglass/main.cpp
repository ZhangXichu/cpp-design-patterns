#include <iostream>
#include <plugin.hpp>

template <class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

int main() {
    const std::string text = "the quick brown fox";

    for (const char *path : {"libs/to_upper_case.so", "libs/word_count.so"}) {
        Plugin p(path);                       // dlopen + plugin_create

        std::cout << path << " -> ";

        std::visit(overloaded{
            [](std::int32_t n)       { std::cout << "integer " << n; },
            [](const std::string &s) { std::cout << "string \"" << s << '"'; },
        }, p.execute(text.c_str()));

        std::cout << '\n';
    }  
}