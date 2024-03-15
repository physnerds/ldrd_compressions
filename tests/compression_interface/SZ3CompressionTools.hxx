#include "CompressionTools.hxx"
#include "SZ3/api/sz.hpp"
#include "SZ3/utils/Config.hpp"

//class for the SZ3 Compressor
class SZ3Compressor : public Compressor {
public:
    SZ3Compressor(const std::string& paramFile) : Compressor(paramFile) {}

    char* compress(const std::vector<float>& input_data) override {
        // Implement compression using SZ3 based on parameters in params
        std::vector<float> compressedData;
        //get access to the json file to retrieve the parameters...
        //remember all these parameter names later.
        SZ3::ALGO param_algo = static_cast<SZ3::ALGO>(params.at("ALGO_OPTIONS"));
        SZ3::EB param_eb = static_cast<SZ3::EB>(param.at("EB_OPTIONS"));
        SZ3::INTERP_ALGO param_inter_algo = static_cast<SZ3::INTERP_ALGO>(params.at("INTERP_ALGO"));
        double error_bound = static_cast<double>(param.at("Error_Bound"));
        SZ3::Config conf(input_data.size());

        //Now the compression...
        conf.cmprAlog = param_algo;
        conf.errorBoundMode = param_eb;
        conf.interpAlgo = param_inter_algo;
        conf.absErrorBound = error_bound;
        char* compressed_data = SZ_compress(conf,input_data,cmpSize);
        param["Compressed_size"] = cmpSize;
        param["Original size"] = input_data.size();
        return compressed_data;
    }

    float* decompress(const char* comp_data) override {
        size_t comp_size = static_cast<size_t>(param.at("Compressed_size"));
        size_t orig_size = static_cast<size_t>(param.at("Original size"));
        SZ3::Config conf(orig_size);
        conf.cmprAlgo = static_cast<SZ3::ALGO>(param.at("ALGO_OPTIONS"));
        conf.errorBoundMode = static_cast<SZ3::EB>(param.at("EB_OPTIONS"));
        conf.interpAlgo = static_cast<SZ3::INTERP_ALGO>(param.at("INTERP_ALGO"));
        conf.absErrorBound = static_cast<double>(param.at("Error_Bound"));
        
        T* decdata = new T[comp_size];
        SZ_decompress(conf,comp_data,comp_size,decdata);
        return decdata;
    }

private:
    size_t cmpSize;
};