#include "CompressionTools.hxx"
#include "mgard/compress.hpp"
#include "mgard/compress_x.hpp"
#include <vector>
#include <iostream>


template<typename T>
    mgard_x::data_type get_mgard_data_type();

template<>
    mgard_x::data_type get_mgard_data_type<float>() {
        return mgard_x::data_type::Float;
    }

template<>
    mgard_x::data_type get_mgard_data_type<double>() {
        return mgard_x::data_type::Double;
    }

//class for MGARD Compressor
class MGARDCompressor : public Compressor {
public:
    MGARDCompressor(const std::string& paramFile) : Compressor(paramFile) {}
    template<typename T>
    char* compress(const std::vector<T>& out_data) {
        //we will use the mgar_x for compression and decompression...
        mgard_x::DIM num_dims = static_cast<mgard_x::DIM>(params.at("NUM_DIMS"));
        mgard_x::SIZE n1 = static_cast<mgard_x::SIZE>(params.at("SIZE"));
        std::vector<mgard_x::SIZE>shape{n1};
        mgard_x::SIZE in_byte = n1*sizeof(T);
        double t = static_cast<double>(params.at("TOLERANCE"));
        double s = static_cast<double>(params.at("SMOOTHNESS"));
        mgard_x::SIZE out_byte;
        void* compressed_array = NULL;
        mgard_x::Config config;
        mgard_x::data_type dtype = get_mgard_data_type<T>(); 
        mgard_x::compress(num_dims,dtype,shape,t,s,
            mgard_x::error_bound_type::REL,out_data.data(),compressed_array,out_byte,config,false);
        params["Original_size"] = in_byte;
        return static_cast<char*>(compressed_array);
    }
template<typename T>
    T* decompress(const char* comp_data) {
        void* decompressed_array = NULL;
        // Implement decompression using MGARD
        mgard_x::SIZE out_byte;
        mgard_x::Config config;
        mgard_x::decompress(comp_data,out_byte,
            decompressed_array,config,false);
	    params["Compressed_size"] = out_byte;
        return static_cast<T*>(decompressed_array);
    }

};
