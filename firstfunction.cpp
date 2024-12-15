#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include "lodepng.h" // Include lodepng header file

// Define M_PI if not defined
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void generateHeightMap(int seed, int length, int breadth, int height) {
    // Initialize random generator
    std::mt19937 generator(seed);
    std::uniform_real_distribution<float> distribution(0.0f, 1.0f);

    // Create and smooth the height map
    std::vector<uint16_t> heightMap(length * breadth);
    for (int i = 0; i < length; ++i) {
        for (int j = 0; j < breadth; ++j) {
            float rawValue = distribution(generator);               // Random value [0, 1]
            float smoothedValue = std::sin(rawValue * M_PI);        // Apply sine smoothing
            heightMap[i * breadth + j] = static_cast<uint16_t>(smoothedValue * height); // Scale to [0, height]
        }
    }

    // Convert to raw grayscale 16-bit PNG data
    std::vector<unsigned char> pngData;
    unsigned error = lodepng::encode(pngData, reinterpret_cast<unsigned char*>(heightMap.data()), breadth, length, LCT_GREY, 16);

    // Save the heightmap as a PNG file
    if (!error) {
        lodepng::save_file(pngData, "heightmap.png");
        std::cout << "Heightmap saved as heightmap.png\n";
    } else {
        std::cerr << "Failed to save the heightmap! Error: " << lodepng_error_text(error) << "\n";
    }
}

int main() {
    // Input parameters
    int seed, length, breadth, height;
    std::cout << "Enter seed, length, breadth, and height: ";
    std::cin >> seed >> length >> breadth >> height;

    // Generate and save heightmap
    generateHeightMap(seed, length, breadth, height);

    return 0;
}
