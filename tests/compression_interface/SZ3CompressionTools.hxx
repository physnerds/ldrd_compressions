#include "CompressionTools.hxx"
#include "SZ3/api/sz.hpp"
#include "SZ3/utils/Config.hpp"

//RNTUPLE headers
#include <ROOT/RNTuple.hxx>
#include <ROOT/RNTupleModel.hxx>

//class for the SZ3 Compressor
class SZ3Compressor : public Compressor {
public:
    SZ3Compressor(const std::string& paramFile) : Compressor(paramFile) {
    }
template<typename T> 
    char* compress(const std::vector<T>& input_data) {
        // Implement compression using SZ3 based on parameters in params
        std::vector<T> compressedData;
        //get access to the json file to retrieve the parameters...
        //remember all these parameter names later.
        SZ3::ALGO param_algo = static_cast<SZ3::ALGO>(params.at("ALGO_OPTIONS"));
        SZ3::EB param_eb = static_cast<SZ3::EB>(params.at("EB_OPTIONS"));
        SZ3::INTERP_ALGO param_inter_algo = static_cast<SZ3::INTERP_ALGO>(params.at("INTERP_ALGO"));
        double abs_error_bound = static_cast<double>(params.at("ABS_ERROR_BOUND"));
        double rel_error_bound = static_cast<double>(params.at("REL_ERROR_BOUND"));
        double psnr_error_bound = static_cast<double>(params.at("PSNR_ERROR_BOUND"));
        double l2norm_error_bound = static_cast<double>(params.at("L2NORM_ERROR_BOUND"));

        SZ3::Config conf(input_data.size());

        //Now the compression...
        conf.cmprAlgo = param_algo;
        conf.errorBoundMode = param_eb;
        conf.interpAlgo = param_inter_algo;
        //Put all the assigned error_bound values...
        //compression alo
        conf.absErrorBound = abs_error_bound;
        conf.relErrorBound = rel_error_bound;
        conf.psnrErrorBound = psnr_error_bound;
        conf.l2normErrorBound = l2norm_error_bound;

        char* compressed_data = SZ_compress(conf,input_data.data(),cmpSize);
        params["Compressed_size"] = cmpSize;
        params["Original_size"] = input_data.size();
        return compressed_data;
    }

    //AB: To do....
    //I dont think json param can be casted back into SZ3 type objects...
    //maybe use enum type and string to map parameters...
template<typename T> 
    T* decompress(const char* comp_data) {
        size_t comp_size = static_cast<size_t>(params.at("Compressed_size"));
        size_t orig_size = static_cast<size_t>(params.at("Original_size"));
        SZ3::Config conf(orig_size);
        conf.cmprAlgo = static_cast<SZ3::ALGO>(params.at("ALGO_OPTIONS"));
        conf.errorBoundMode = static_cast<SZ3::EB>(params.at("EB_OPTIONS"));
        conf.interpAlgo = static_cast<SZ3::INTERP_ALGO>(params.at("INTERP_ALGO"));
        conf.absErrorBound = static_cast<double>(params.at("ABS_ERROR_BOUND"));
        conf.relErrorBound = static_cast<double>(params.at("REL_ERROR_BOUND"));
        conf.psnrErrorBound = static_cast<double>(params.at("PSNR_ERROR_BOUND"));
        conf.l2normErrorBound = static_cast<double>(params.at("L2NORM_ERROR_BOUND"));
        T* decdata = new T[orig_size];
	//FIXME Need better way to implement this....
        SZ_decompress(conf,const_cast<char*>(comp_data),comp_size,decdata);
        return decdata;
    }
    
    size_t GetCompressedSize(){
        return cmpSize;
    }

private:
    size_t cmpSize;
    
};

