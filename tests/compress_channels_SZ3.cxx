#include "H5Cpp.h"
#include <iostream>
#include "compression_interface/Utilities.hxx"
#include "compression_interface/WriteConfigParameters.h"
#include "compression_interface/SZ3CompressionTools.hxx"

#include "TFile.h"
#include "TTree.h"

namespace SZ3{
    constexpr EB LOCAL_EB_OPTIONS[] = {EB_ABS};  
    constexpr const ALGO LOCAL_ALGO_OPTIONS[] = {ALGO_INTERP_LORENZO};
    constexpr INTERP_ALGO LOCAL_INTERP_ALGO_OPTIONS[] = {INTERP_ALGO_CUBIC};  
}

const std::string filename ="/home/abashyal/git_compression/compressiontests/new_magnify2-28052-20833_1DAfterNoise.h5" ; //"/home/abashyal/compressiontests/waveforms_ver2.h5";
const std::string outfilename = "test_SZ3_compression_err50_AfterNoise.root" ; 
const int tot_channels = 10240;
double err_val = 20.00;
int main(){ 
    H5::H5File file(filename,H5F_ACC_RDONLY);

    TFile *f  = new TFile(outfilename.c_str(),"RECREATE");
    TTree *t = new TTree("Compressions","Compressions with SZ3");

    std::vector<double>fieldOutData;
    std::vector<char>fieldCompressedData;
    std::string param;

    for(int dval = 0; dval<tot_channels+1; dval++){
        std::string dsetname = "waveform_"+std::to_string(dval);
        std::cout<<"Compressing dataset "<<dsetname<<std::endl;
        std::vector<double>out_data = ReturnH5Data<double>(file,dsetname);
        fieldOutData->clear();
        *fieldOutData = out_data;
        //we are creating a json file first...
        //TODO : Maybe use err_bound also as a parameter to do the test?
        double abs_err_bound = err_val;
        double rel_err_bound = err_val;
        double psnr_err_bound = err_val;
        double l2norm_err_bound = err_val;

        std::string temp_name = "params_sz3";
        std::string jfilename = temp_name+".json";
        WriteJSONConfig("SZ3",
        std::make_pair("ALGO_OPTIONS",SZ3::ALGO_INTERP_LORENZO), //interpolation instead of lorenzo
        std::make_pair("EB_OPTIONS",SZ3::EB_ABS),
        std::make_pair("INTERP_ALGO",SZ3::INTERP_ALGO_CUBIC),
        std::make_pair("ABS_ERROR_BOUND",abs_err_bound),
        std::make_pair("REL_ERROR_BOUND",rel_err_bound),
        std::make_pair("PSNR_ERROR_BOUND",psnr_err_bound),
        std::make_pair("L2NORM_ERROR_BOUND",l2norm_err_bound)
        );
        //now the json file to write the parameters..
        writeJSON(jfile,jfilename);
        SZ3Compressor sz3compress(jfilename);
        //apply compression....
        char* compressed_data = sz3compress.compress<double>(out_data);
        double* decompressed_data = sz3compress.decompress<double>(compressed_data);
        
        std::size_t compressed_size = sz3compress.GetCompressedSize();
        double comp_ratio = double(sizeof(double)*out_data.size()/(sizeof(char)*compressed_size));
        jfile["COMPRESSION_RATIO"] = comp_ratio;
        jfile["ORIGINAL DSET"] = dval; //this is needed to keep track of original dset.

        //this is to make sure that the json content is updated before writing into rntuple.
        sz3compress.UpdateJSONContent(jfile);

        param->clear();
        *param = jfile.dump();

        //now the compressed data...
        std::vector<char>comp_data(compressed_data,compressed_data+compressed_size);
        fieldCompressedData->clear();
        *fieldCompressedData = comp_data;

        t->Fill();
        std::remove(jfilename.c_str());


    }

    f->Write();
    f->Close();
}