#include "CompressionTools.hxx"
#include "SZ3/api/sz.hpp"
#include "SZ3/utils/Config.hpp"

//TTree Headers
#include "TTree.h"
#include "TFile.h"

//class for the SZ3 Compressor
class SZ3Compressor : public Compressor {
public:
    SZ3Compressor(const std::string& paramFile) : Compressor(paramFile) {
    }

    void ManageConfigFile(SZ3::Config& conf){
        SZ3::ALGO param_algo = static_cast<SZ3::ALGO>(params.at("ALGO_OPTIONS"));
        SZ3::EB param_eb = static_cast<SZ3::EB>(params.at("EB_OPTIONS"));
        SZ3::INTERP_ALGO param_inter_algo = static_cast<SZ3::INTERP_ALGO>(params.at("INTERP_ALGO"));
        double abs_error_bound = static_cast<double>(params.at("ABS_ERROR_BOUND"));
        double rel_error_bound = static_cast<double>(params.at("REL_ERROR_BOUND"));
        double psnr_error_bound = static_cast<double>(params.at("PSNR_ERROR_BOUND"));
        double l2norm_error_bound = static_cast<double>(params.at("L2NORM_ERROR_BOUND")); 
        conf.cmprAlgo = param_algo;
        conf.errorBoundMode = param_eb;
        conf.interpAlgo = param_inter_algo;
        //Put all the assigned error_bound values...
        conf.absErrorBound = abs_error_bound;
        conf.relErrorBound = rel_error_bound;
        conf.psnrErrorBound = psnr_error_bound;
        conf.l2normErrorBound = l2norm_error_bound;   
    }

template<typename T> 
    char* compress(const std::vector<T>& input_data, bool twod=false) {
        // Implement compression using SZ3 based on parameters in params
        char* compressed_data=nullptr;
        size_t cmpSize;
        if(!twod){
            SZ3::Config conf = ConfigureSZ3(input_data.size());
            //params is protected member of CompressionTools.hxx
            ManageConfigFile(conf);
            compressed_data = SZ_compress(conf,input_data.data(),cmpSize);
        }
        else{
            size_t xdim = static_cast<size_t>(params.at("XDIM"));
            size_t ydim = static_cast<size_t>(params.at("YDIM"));
            SZ3::Config conf = ConfigureSZ3(xdim,ydim);
            ManageConfigFile(conf);
            compressed_data = SZ_compress(conf,input_data.data(),cmpSize);
        }
        params["Compressed_size"] = cmpSize;
        params["Original_size"] = input_data.size();

        //SZ3::calAbsErrorBound(conf, input_data.data());
        //params["Transformed_Error"] = conf.absErrorBound;
        return compressed_data;
    }


template<typename T> 
    T* decompress(const char* comp_data, bool twod=false) {
        size_t orig_size = static_cast<size_t>(params.at("Original_size"));
        size_t comp_size = static_cast<size_t>(params.at("Compressed_size"));
        T* decdata = new T[orig_size];
        if(!twod){
            SZ3::Config conf(orig_size);
            ManageConfigFile(conf);
            SZ_decompress(conf,const_cast<char*>(comp_data),comp_size,decdata);
        }
        else{
            size_t xdim = static_cast<size_t>(params.at("XDIM"));
            size_t ydim = static_cast<size_t>(params.at("YDIM"));
            SZ3::Config conf(xdim,ydim);
            ManageConfigFile(conf);
            SZ_decompress(conf,const_cast<char*>(comp_data),comp_size,decdata);
            
        }
        return decdata;
    }
    
    size_t GetCompressedSize(){
        return static_cast<size_t>(params.at("Compressed_size"));
    }

template<typename ...Args>
    SZ3::Config ConfigureSZ3(Args...args){
        SZ3::Config conf(args...);
        return conf;
    }

private:
    
};

