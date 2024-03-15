#include <vector>
#include <fstream>
#include <nlohmann/json.hpp>
#include "H5Cpp.h"
#include "Math/GoFTest.h"
#include "mgard/compress.hpp"
#include "compression_interface/WriteConfigParameters.h"

class Compressor {
protected:
    nlohmann::json params;
public:
    Compressor(const std::string& paramFile) {
        std::ifstream file(paramFile);
        if (file.is_open()) {
            file >> params;
        }
    }

    virtual char* compress(const std::vector<float>& data) = 0;
    virtual std::vector<float> decompress(const std::vector<float>& data) = 0;
    virtual ~Compressor() {}
};
