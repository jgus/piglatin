#include <iostream>

#include "async_converter.h"

int main(int argc, char *argv[]) {
    async_converter converter([](std::string const& original, std::string const& converted){
        std::cout << "'" << original << "' -> '" << converted << "'" << std::endl;
    });
    converter.convert("Hello world !");
}
