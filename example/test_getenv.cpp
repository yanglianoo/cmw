#include <iostream>
#include <cstdlib>

int main() {
    const char* path = std::getenv("CMW_IP");
    if (path) {
        std::cout << "PATH: " << path << std::endl;
    } else {
        std::cout << "PATH not found" << std::endl;
    }
    return 0;
}
