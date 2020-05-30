#include <iostream>

#include "PigLatinConverter.h"

int main(int argc, char *argv[]) {
    PigLatinConverter converter;
    auto result = converter.convert("Hello world !");
    std::cout << result << std::endl;
}
