#include "H5Cpp.h"
#include <iostream>
#include "compression_interface/Utilities.hxx"
#include "compression_interface/WriteConfigParameters.h"
#include "compression_interface/SZ3CompressionTools.hxx"

namespace SZ3{
    constexpr EB LOCAL_EB_OPTIONS[] = {EB_ABS};  
    constexpr const ALGO LOCAL_ALGO_OPTIONS[] = {ALGO_INTERP_LORENZO};
    constexpr INTERP_ALGO LOCAL_INTERP_ALGO_OPTIONS[] = {INTERP_ALGO_CUBIC};  
}

const std::string filename ="/home/abashyal/git_compression/compressiontests/new_magnify2-28052-20833_1DAfterNoise.h5"; //"/home/abashyal/compressiontests/waveforms_ver2.h5";
const std::string outfilename = "test_SZ3_compression_err10.root" ; 
const int tot_channels = 10240;
const int tot_ticks = 5860;
bool twoD = false;
bool lorenzo2 = true;
int main(){ 
    H5::H5File file(filename,H5F_ACC_RDONLY);
    std::vector<double> out_data;
    for(int dval = 0; dval<tot_channels; dval++){
        
        std::string dsetname = "waveform_"+std::to_string(dval);
        std::vector<double>temp_out_data = ReturnH5Data<double>(file,dsetname);
        out_data.insert(out_data.end(),temp_out_data.begin(),temp_out_data.end());
    }
    //Verify the total size of data and dimensions...
    std::cout<<out_data.size()<<" "<<tot_channels*tot_ticks<<std::endl;
    //we are creating a json file first...
    //TODO : Maybe use err_bound also as a parameter to do the test?
    double abs_err_bound = 4;
    double rel_err_bound = 4;
    double psnr_err_bound = 4;
    double l2norm_err_bound = 4;
    
    std::string temp_name = "params_sz3";
    std::string jfilename = temp_name+".json";
    WriteJSONConfig("SZ3",
    std::make_pair("ALGO_OPTIONS",SZ3::ALGO_INTERP_LORENZO),
    std::make_pair("EB_OPTIONS",SZ3::EB_ABS),
    std::make_pair("INTERP_ALGO",SZ3::INTERP_ALGO_CUBIC),
    std::make_pair("ABS_ERROR_BOUND",abs_err_bound),
    std::make_pair("REL_ERROR_BOUND",rel_err_bound),
    std::make_pair("PSNR_ERROR_BOUND",psnr_err_bound),
    std::make_pair("L2NORM_ERROR_BOUND",l2norm_err_bound),
    std::make_pair("LORENZO2",lorenzo2),
    std::make_pair("XDIM",tot_channels),
    std::make_pair("YDIM",tot_ticks)
    );
    //now the json file to write the parameters..
    writeJSON(jfile,jfilename);
    SZ3Compressor sz3compress(jfilename);
    //apply compression....
    char* compressed_data = sz3compress.compress<double>(out_data,twoD);
    double* decompressed_data = sz3compress.decompress<double>(compressed_data,twoD);
    
    std::size_t compressed_size = sz3compress.GetCompressedSize();
    double comp_ratio = double(sizeof(double)*out_data.size()/(sizeof(char)*compressed_size));
    jfile["COMPRESSION_RATIO"] = comp_ratio;
    sz3compress.UpdateJSONContent(jfile);
    std::cout<<jfile.dump()<<std::endl;
    //this is to make sure that the json content is updated before writing into rntuple.
    
}