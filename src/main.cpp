#include <iostream>
#include "application.h"


int main() {

    try {
        Application app("Test", 400, 80);
        app.run();
    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
