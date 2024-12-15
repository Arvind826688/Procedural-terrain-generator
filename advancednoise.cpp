#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include "lodepng.h" // Include the lodepng library for PNG writing

// Hashing function for pseudo-random gradients
int permute(int x) {
    static const int permutation[256] = {
        151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,
        8,99,37,240,21,10,23,190,6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,
        35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168,68,175,74,165,71,
        134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,
        41,55,46,245,40,244,102,143,54,65,25,63,161,1,216,80,73,209,76,132,187,208,
        89,18,169,200,196,135,130,116,188,159,86,164,100,109,198,173,186,3,64,52,
        217,226,250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,
        16,58,17,182,189,28,42,223,183,170,213,119,248,152,2,44,154,163,70,221,153,
        101,155,167,43,172,9,129,22,39,253,19,98,108,110,79,113,224,232,178,185,112,
        104,218,246,97,228,251,34,242,193,238,210,144,12,191,179,162,241,81,51,145,
        235,249,14,239,107,49,192,214,31,181,199,106,157,184,84,204,176,115,121,50,
        45,127,4,150,254,138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,
        215,61,156,180
    };
    return permutation[x % 256];
}

// Fade function for smooth interpolation
float fade(float t) {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

// Linear interpolation
float lerp(float t, float a, float b) {
    return a + t * (b - a);
}

// Gradient function
float gradient(int hash, float x, float y) {
    int h = hash & 3; // Take the last 2 bits
    float u = h < 2 ? x : y;
    float v = h < 2 ? y : x;
    return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
}

// Perlin noise function
float perlinNoise(float x, float y, int seed) {
    float frequency = 0.01f; // Scale to control noise detail
    x *= frequency;
    y *= frequency;

    int x0 = static_cast<int>(std::floor(x)) & 255;
    int y0 = static_cast<int>(std::floor(y)) & 255;
    int x1 = (x0 + 1) & 255;
    int y1 = (y0 + 1) & 255;

    float dx = x - std::floor(x);
    float dy = y - std::floor(y);

    int aa = permute(permute(x0) + y0 + seed % 256);
    int ab = permute(permute(x0) + y1 + seed % 256);
    int ba = permute(permute(x1) + y0 + seed % 256);
    int bb = permute(permute(x1) + y1 + seed % 256);

    float gradAA = gradient(aa, dx, dy);
    float gradBA = gradient(ba, dx - 1, dy);
    float gradAB = gradient(ab, dx, dy - 1);
    float gradBB = gradient(bb, dx - 1, dy - 1);

    float u = fade(dx);
    float v = fade(dy);

    float lerpX1 = lerp(u, gradAA, gradBA);
    float lerpX2 = lerp(u, gradAB, gradBB);

    return lerp(v, lerpX1, lerpX2);
}


// Generate height map
std::vector<uint16_t> generateHeightMap(int length, int breadth, int height, int seed) {
    std::vector<uint16_t> heightMap(length * breadth);

    for (int y = 0; y < breadth; y++) {
        for (int x = 0; x < length; x++) {
            float noiseValue = perlinNoise(static_cast<float>(x), static_cast<float>(y), seed);
noiseValue = (noiseValue + 1.0f) / 2.0f; // Normalize to [0, 1]
uint16_t pixelValue = static_cast<uint16_t>(noiseValue * (height - 1)); // Avoid overflow

            heightMap[y * length + x] = pixelValue;
        }
    }

    return heightMap;
}

// Save height map as 16-bit grayscale PNG
void saveAsPNG(const std::vector<uint16_t>& heightMap, int length, int breadth, const char* filename) {
    std::vector<unsigned char> image(length * breadth * 2); // 16-bit grayscale

    for (int i = 0; i < heightMap.size(); i++) {
        uint16_t value = heightMap[i];
        image[2 * i + 0] = value >> 8;      // High byte
        image[2 * i + 1] = value & 0xFF;    // Low byte
    }

    unsigned error = lodepng::encode(filename, image, length, breadth, LCT_GREY, 16);
    if (error) {
        std::cerr << "Error encoding PNG: " << lodepng_error_text(error) << std::endl;
    } else {
        std::cout << "Saved terrain as " << filename << std::endl;
    }
}

int main() {
    int length, breadth, height, seed;

    // Input parameters
    std::cout << "Enter seed value: ";
    std::cin >> seed;
    std::cout << "Enter length (width in pixels): ";
    std::cin >> length;
    std::cout << "Enter breadth (height in pixels): ";
    std::cin >> breadth;
    std::cout << "Enter maximum height (brightness, 16-bit max is 65535): ";
    std::cin >> height;

    // Generate and save the height map
    auto heightMap = generateHeightMap(length, breadth, height, seed);
    saveAsPNG(heightMap, length, breadth, "terrain.png");

    return 0;
}

