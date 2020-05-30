#include <iostream>
#include <set>

#include "async_converter.h"

int main(int argc, char *argv[]) {
    async_converter converter([](std::string const& original, std::string const& converted){
        std::cout << "'" << original << "' -> '" << converted << "'" << std::endl;
    });
    static const std::set<std::string> quit_keywords{"q",  "quit", "exit"};
    for (std::string line; std::getline(std::cin, line);) {
        if (quit_keywords.count(line)) {
            converter.cancel();
            return 0;
        }
        converter.convert(line);
    }
    return 0;
}
