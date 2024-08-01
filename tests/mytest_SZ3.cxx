#include "H5Cpp.h"
#include <iostream>
#include "compression_interface/Utilities.hxx"
#include "compression_interface/WriteConfigParameters.h"
#include "compression_interface/SZ3CompressionTools.hxx"

const std::string filename ="/home/abashyal/compressiontests/waveforms.h5" ; //"/home/abashyal/compressiontests/waveforms_ver2.h5";


int main(){ 
    H5::H5File file(filename,H5F_ACC_RDONLY);
    std::string dsetname = "waveform_1";
    std::vector<double> out_data = ReturnH5Data<double>(file,dsetname);
    //we want to write the compressed data and original data in the RNTuple File...
    //so we create the model schema....
    //ROOT version might cause API issues...check on your own risk..
    auto model = ROOT::Experimental::RNTupleModel::Create();
    auto fieldOutData = model->MakeField<std::vector<double>>("out_data");
    auto fieldCompressedData = model->MakeField<std::vector<char>>("compressed_data");
    auto param = model->MakeField<std::string>("parameters");

    auto ntuple = ROOT::Experimental::RNTupleWriter::Recreate(std::move(model), "Compressions", "test_SZ3_compression.root");
    fieldOutData->clear();
    *fieldOutData = out_data;

    for(auto al_val: SZ3::ALGO_OPTIONS){
        for(auto al_eb: SZ3::EB_OPTIONS){
            for(auto al_ial: SZ3::INTERP_ALGO_OPTIONS){
                //we are creating a json file first...
                //TODO : Maybe use err_bound also as a parameter to do the test?
                double err_bound = 0.005 ; 
                std::string str_al_val = std::string(SZ3::enum2Str<SZ3::ALGO>(al_val));
                std::string str_al_eb = std::string(SZ3::enum2Str<SZ3::EB>(al_eb));
                std::string str_al_ial = std::string(SZ3::enum2Str<SZ3::INTERP_ALGO>(al_ial));
                std::string str_err_bound = std::to_string(err_bound);
                std::string temp_name = "params_sz3_"+str_al_val+"_"+str_al_eb+"_"+str_al_ial+"_"+str_err_bound;
                std::string jfilename = temp_name+".json";
                WriteJSONConfig("SZ3",
                std::make_pair("ALGO_OPTIONS",al_val),
                std::make_pair("EB_OPTIONS",al_eb),
                std::make_pair("INTERP_ALGO",al_ial),
                std::make_pair("ERROR_BOUND",err_bound)   
                );
                //now the json file to write the parameters..
                writeJSON(jfile,jfilename);
                SZ3Compressor sz3compress(jfilename);
                //apply compression....
                char* compressed_data = sz3compress.compress<double>(out_data);

                double* decompressed_data = sz3compress.decompress<double>(compressed_data);
                
                auto ksval = CalculateKSVal<double>(out_data.size(),out_data.data(),decompressed_data);

                std::size_t compressed_size = sz3compress.GetCompressedSize();
                double comp_ratio = double(sizeof(double)*out_data.size()/(sizeof(char)*compressed_size));

                //Draw the plots...
                DrawGraphs(out_data.data(),decompressed_data,out_data.size(),temp_name+".png",ksval,comp_ratio);
                //dump the compression parameter....
                sz3compress.dumpJSONContent();

                param->clear();
                *param = jfile.dump();

                //now the compressed data...
                std::vector<char>comp_data(compressed_data,compressed_data+compressed_size);
                fieldCompressedData->clear();
                *fieldCompressedData = comp_data;

                ntuple->Fill();

            }
        }
    }
    
}