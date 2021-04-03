#include <iostream>
#include <string>
#include <vector>

#include "ukkonen.hpp"

int main() {
    std::string pattern;
    std::string text;
    std::cin >> pattern >> text;
    std::vector<int> entryPositions = NUkkonen::Find(text, pattern);
    for (int p: entryPositions) {
        std::cout << p << "\n";
    }
    return 0;
}