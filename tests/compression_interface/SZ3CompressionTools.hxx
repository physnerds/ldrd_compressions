#include "CompressionTools.hxx"
#include "SZ3/api/sz.hpp"
#include "SZ3/utils/Config.hpp"

//class for the SZ3 Compressor
class SZ3Compressor : public Compressor {
public:
    SZ3Compressor(const std::string& paramFile) : Compressor(paramFile) {
        // Initialize maps in the constructor
        algo_map["ALGO_LORENZO_REG"] = SZ3::ALGO_LORENZO_REG;
        algo_map["ALGO_INTERP_LORENZO"] = SZ3::ALGO_INTERP_LORENZO;
        algo_map["ALGO_INTERP"] = SZ3::ALGO_INTERP;

        eb_map["ABS"] = SZ3::EB_ABS;
        eb_map["REL"] = SZ3::EB_REL;
        eb_map["PSNR"] = SZ3::EB_PSNR;
        eb_map["NORM"] = SZ3::EB_L2NORM;
        eb_map["ABS_AND_REL"] = SZ3::EB_ABS_AND_REL;

        interp_map["INTERP_ALGO_LINEAR"] = SZ3::INTERP_ALGO_LINEAR;
        interp_map["INTERP_ALGO_CUBIC"] = SZ3::INTERP_ALGO_CUBIC;

    }
    char* compress(const std::vector<float>& input_data) override {
        // Implement compression using SZ3 based on parameters in params
        std::vector<float> compressedData;
        //get access to the json file to retrieve the parameters...
        //remember all these parameter names later.
        SZ3::ALGO param_algo = static_cast<SZ3::ALGO>(params.at("ALGO_OPTIONS"));
        SZ3::EB param_eb = static_cast<SZ3::EB>(params.at("EB_OPTIONS"));
        SZ3::INTERP_ALGO param_inter_algo = static_cast<SZ3::INTERP_ALGO>(params.at("INTERP_ALGO"));
        double error_bound = static_cast<double>(params.at("ERROR_BOUND"));
        SZ3::Config conf(input_data.size());

        //Now the compression...
        conf.cmprAlgo = param_algo;
        conf.errorBoundMode = param_eb;
        conf.interpAlgo = param_inter_algo;
        conf.absErrorBound = error_bound;
        char* compressed_data = SZ_compress(conf,input_data.data(),cmpSize);
        params["Compressed_size"] = cmpSize;
        params["Original_size"] = input_data.size();
        return compressed_data;
    }
    //AB: To do....
    //I dont think json param can be casted back into SZ3 type objects...
    //maybe use enum type and string to map parameters...

    float* decompress(char* comp_data) override {
        size_t comp_size = static_cast<size_t>(params.at("Compressed_size"));
        size_t orig_size = static_cast<size_t>(params.at("Original_size"));
        SZ3::Config conf(orig_size);
        conf.cmprAlgo = static_cast<SZ3::ALGO>(params.at("ALGO_OPTIONS"));
        conf.errorBoundMode = static_cast<SZ3::EB>(params.at("EB_OPTIONS"));
        conf.interpAlgo = static_cast<SZ3::INTERP_ALGO>(params.at("INTERP_ALGO"));
        conf.absErrorBound = static_cast<double>(params.at("ERROR_BOUND"));
        
        float* decdata = new float[comp_size];
        SZ_decompress(conf,comp_data,comp_size,decdata);
        return decdata;
    }

private:
    size_t cmpSize;

    std::map<std::string, SZ3::ALGO> algo_map;
    std::map<std::string, SZ3::EB> eb_map;    
    std::map<std::string, SZ3::INTERP_ALGO>interp_map;

    
};

