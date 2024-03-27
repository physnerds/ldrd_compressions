#include "mgard/compress.hpp"
#include "mgard/compress_x.hpp"
#include <vector>
#include <iostream>

//class for MGARD Compressor
class MGARDCompressor : public Compressor {
public:
    MGARDCompressor(const std::string& paramFile) : Compressor(paramFile) {}

    char* compress(const std::vector<float>& out_data) override {
        //we will use the mgar_x for compression and decompression...
        mgard_x::DIM num_dims = static_cast<mgard_x::DIM>(params.at("NUM_DIMS"));
        mgard_x::SIZE n1 = static_cast<mgard_x::SIZE>(params.at("SIZE"));
        std::vector<mgard_x::SIZE>shape{n1};
        mgard_x::SIZE in_byte = static_cast<mgard_x::SIZE>(params.at("IN_BYTE"));
        double tolerance = static_cast<double>(params.at("TOLERANCE"));
        double s = static_cast<double>(params.at("SMOOTHNESS"));
        mgard_x::SIZE out_byte;
        void* compressed_array = NULL;
        mgard_x::Config config;

        mgard_x::compress("num_dims",mgard_x::data_type::Double,shape,tol,s,
            mgard_x::error_bound_type::REL,out_data.data(),compressed_array,out_byte,config,false);
        
        params.at("Compressed_size") = out_byte;
        params.at("Original_size") = in_byte;
        return static_cast<char*>(compressed_array);
    }

    float* decompress(const char* comp_data) override {
        // Implement decompression using MGARD
        mgard_x::SIZE out_byte = static_cast<mgard_x::SIZE>(params.at("Compressed_size"));
        mgard_x::Config config;
        mgard_x::decompress(comp_data,out_byte,
            decompressed_array,config,false);
        return decompressedData;
    }
};
