#include <iostream>
#include <set>
#include <optional>

#include "async_converter.h"
#include "tty.h"

std::optional<std::string> getline() {
    if (input_is_tty())
        std::cerr << R"(Enter text to convert to pig latin ("q", "quit", or "exit" to quit):)" << std::endl;
    std::string line;
    if (!std::getline(std::cin, line)) return std::nullopt;
    return line;
}

int main(int argc, char *argv[]) {
    async_converter converter([](std::string const& original, std::string const& converted){
        std::cout << "'" << original << "' -> '" << converted << "'" << std::endl;
    });
    static const std::set<std::string> quit_keywords{"q",  "quit", "exit"};
    for (auto line = getline(); line; line = getline()) {
        if (input_is_tty() && quit_keywords.count(*line)) {
            converter.cancel();
            return 0;
        }
        converter.convert(*line);
    }
    return 0;
}
