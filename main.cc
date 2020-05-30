#include <iostream>
#include <set>
#include <optional>

#include "async_converter.h"
#include "tty.h"

// Get a line from the input, or nullopt if we've reached the end of the input stream. Prompt the user if our input in an interactive console.
std::optional<std::string> getline() {
    if (input_is_tty())
        std::cerr << R"(Enter text to convert to pig latin ("q", "quit", or "exit" to quit):)" << std::endl;
    std::string line;
    if (!std::getline(std::cin, line)) return std::nullopt;
    return line;
}

int main(int argc, char* argv[]) {
    static const std::set<std::string> quit_keywords{"q", "quit", "exit"};

    // Set up a converter which, for each completed item, prints the original and its conversion to the output
    async_converter converter([](std::string const& original, std::string const& converted) {
        std::cout << "'" << original << "' -> '" << converted << "'" << std::endl;
    });

    // Get a line of input, and pass it to the converter, until the user quits (in an interactive console) or we reach the end of input
    for (auto line = getline(); line; line = getline()) {
        if (input_is_tty() && quit_keywords.count(*line)) {
            converter.cancel();
            return 0;
        }
        converter.convert(*line);
    }

    return 0;
}
