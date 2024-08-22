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

const std::string filename ="/home/abashyal/git_compression/compressiontests/new_magnify2-28052-20833h_orig_1D.h5" ; //"/home/abashyal/compressiontests/waveforms_ver2.h5";
const std::string outfilename = "test_SZ3_compression.root" ; 
const int tot_channels = 10240;
int main(){ 
    H5::H5File file(filename,H5F_ACC_RDONLY);
    //std::string dsetname = "waveform_1";
    //std::vector<double> out_data = ReturnH5Data<double>(file,dsetname);
    //we want to write the compressed data and original data in the RNTuple File...
    //so we create the model schema....
    //ROOT version might cause API issues...check on your own risk..
    auto model = ROOT::Experimental::RNTupleModel::Create();
    auto fieldOutData = model->MakeField<std::vector<double>>("out_data");
    auto fieldCompressedData = model->MakeField<std::vector<char>>("compressed_data");
    auto param = model->MakeField<std::string>("parameters");

    auto ntuple = ROOT::Experimental::RNTupleWriter::Recreate(std::move(model), "Compressions", outfilename.c_str());
    for(int dval = 0; dval<tot_channels; dval++){
        std::string dsetname = "waveform_"+std::to_string(dval);
        std::cout<<"Compressing dataset "<<dsetname<<std::endl;
        std::vector<double>out_data = ReturnH5Data<double>(file,dsetname);
        fieldOutData->clear();
        *fieldOutData = out_data;
        //we are creating a json file first...
        //TODO : Maybe use err_bound also as a parameter to do the test?
        double abs_err_bound = 4 ;
        double rel_err_bound = 4;
        double psnr_err_bound = 4;
        double l2norm_err_bound = 4;
        std::string str_al_val = std::string(SZ3::enum2Str<SZ3::ALGO>(SZ3::ALGO_INTERP_LORENZO));
        std::string str_al_eb = std::string(SZ3::enum2Str<SZ3::EB>(SZ3::EB_ABS));
        std::string str_al_ial = std::string(SZ3::enum2Str<SZ3::INTERP_ALGO>(SZ3::INTERP_ALGO_CUBIC));
        std::string str_abs_err_bound = std::to_string(abs_err_bound);
        std::string str_rel_err_bound = std::to_string(abs_err_bound);
        std::string str_psnr_err_bound = std::to_string(abs_err_bound);
        std::string str_l2norm_err_bound = std::to_string(l2norm_err_bound);
        //std::string temp_name = "params_sz3_"+str_al_val+"_"+
        //    str_al_eb+"_"+str_al_ial+"_"+str_abs_err_bound+"_"+str_rel_err_bound+"_"+str_psnr_err_bound+"_"+str_l2norm_err_bound;
        std::string temp_name = "params_sz3";
        std::string jfilename = temp_name+".json";
        WriteJSONConfig("SZ3",
        std::make_pair("ALGO_OPTIONS",SZ3::ALGO_INTERP_LORENZO),
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
        
        //auto ksval = CalculateKSVal<int>(out_data.size(),out_data.data(),decompressed_data);

        std::size_t compressed_size = sz3compress.GetCompressedSize();
        double comp_ratio = double(sizeof(int)*out_data.size()/(sizeof(char)*compressed_size));
        jfile["COMPRESSION_RATIO"] = comp_ratio;
        jfile["ORIGINAL DSET"] = dval; //this is needed to keep track of original dset.

        //this is to make sure that the json content is updated before writing into rntuple.
        sz3compress.UpdateJSONContent(jfile);
        //double tranformed_error = jfile.at("Transformed_Error");
        //std::string rfilename = temp_name+std::to_string(tranformed_error);

        //DrawGraphs(out_data.data(),decompressed_data,out_data.size(),temp_name+".png",ksval,comp_ratio);
        //dump the compression parameter....
        //sz3compress.dumpJSONContent();                    
        param->clear();
        *param = jfile.dump();

        //now the compressed data...
        std::vector<char>comp_data(compressed_data,compressed_data+compressed_size);
        fieldCompressedData->clear();
        *fieldCompressedData = comp_data;

        ntuple->Fill();
        std::remove(jfilename.c_str());


    }
    
}