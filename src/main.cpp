#include <iostream>
#include "application.cuh"


int main() {

    try {
        Application app("Fluid Simulation", 1500, 900);
        app.run();
    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
