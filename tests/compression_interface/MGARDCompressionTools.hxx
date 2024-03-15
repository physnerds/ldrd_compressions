#include "mgard/compress.hpp"
#include <vector>
#include <iostream>

//class for MGARD Compressor
class MGARDCompressor : public Compressor {
public:
    MGARDCompressor(const std::string& paramFile) : Compressor(paramFile) {}

    std::vector<float> compress(const std::vector<float>& data) override {
        // Implement compression using MGARD based on parameters in params
        std::vector<float> compressedData;
        // Example: Populate compressedData based on MGARD algorithm
        return compressedData;
    }

    std::vector<float> decompress(const std::vector<float>& data) override {
        // Implement decompression using MGARD
        std::vector<float> decompressedData;
        // Example: Populate decompressedData based on MGARD algorithm
        return decompressedData;
    }
};
