#undef R__HAS_VDT

#include <ROOT/RNTuple.hxx>
#include <ROOT/RNTupleModel.hxx>
#include <TFile.h>
#include "Math/GoFTest.h"
#include "TGraph.h"
#include "SZ3/api/sz.hpp"
#include "SZ3/utils/Config.hpp"
#include "compression_interface/WriteConfigParameters.h"
#include "compression_interface/SZ3CompressionTools.hxx"
#include "compression_interface/Utilities.hxx"

//This test is to test the utilities and fix them....
const std::string filename = "/home/abashyal/compressiontests/waveforms_ver2.h5";

R__LOAD_LIBRARY(ROOTNTuple)

using namespace ROOT::Experimental;
std::string dsetname = "waveform_1";
const char* fname = "test_compression.root";
void compressfile(){
    H5::H5File file(filename,H5F_ACC_RDONLY);
    std::vector<double>out_data = ReturnH5Data<double>(file,dsetname);
    size_t cmpSize;
    //set the compression configuration....
    WriteJSONConfig("SZ3",
        std::make_pair("ALGO_OPTIONS",SZ3::ALGO_LORENZO_REG),
        std::make_pair("EB_OPTIONS",SZ3::EB_ABS_OR_REL),
        std::make_pair("INTERP_ALGO",SZ3::INTERP_ALGO_LINEAR),
        std::make_pair("ERROR_BOUND",1E-3),
        std::make_pair("ORIGINAL_SIZE",out_data.size())
        );
    
    //now test if you can write the configuration parameters....
    SZ3::Config conf(out_data.size());

    conf.cmprAlgo = jfile.at("ALGO_OPTIONS");
    conf.errorBoundMode = jfile.at("EB_OPTIONS");
    conf.interpAlgo = jfile.at("INTERP_ALGO");
    conf.absErrorBound = jfile.at("ERROR_BOUND");

    char* compressed_data = SZ_compress(conf,out_data.data(),cmpSize);
    std::vector<char>comp_data(compressed_data,compressed_data+cmpSize);
    std::string jfilename = "params.json";
    writeJSON(jfile,jfilename);

    //now write the file in RNTuple...
    auto model = RNTupleModel::Create();
    auto& orig = *model->MakeField<std::vector<double>>("original_data");
    auto& compr = *model->MakeField<std::vector<char>>("compressed_data");
    auto& param = *model->MakeField<std::string>("parameters");
    
    auto ntuple = RNTupleWriter::Recreate(std::move(model),dsetname.c_str(),fname);

    orig.clear();
    orig = out_data;

    compr.clear();
    compr = comp_data;

    param = jfile.dump();
    ntuple->Fill();

}

void decompressfile(){
    auto ntuple = RNTupleReader::Open(dsetname.c_str(),fname);

    std::string jString;
    auto param = ntuple->GetView<std::string>("parameters");
    jString = param(0);
    nlohmann::json jobj = nlohmann::json::parse(jString);

    auto v_data = ntuple->GetView<std::vector<char>>("compressed_data");
    std::vector<char>comp_data = v_data(0);

    auto vorig_data = ntuple->GetView<std::vector<double>>("original_data");
    std::vector<double>orig_data = vorig_data(0);

    ntuple->PrintInfo(ENTupleInfo::kStorageDetails);
    size_t original_size = jobj.at("ORIGINAL_SIZE");
    double* decomp_data = new double[original_size];

    size_t cmpSize = comp_data.size();

    SZ3::Config conf(original_size);
    conf.cmprAlgo = jobj.at("ALGO_OPTIONS");
    conf.errorBoundMode = jobj.at("EB_OPTIONS");
    conf.interpAlgo = jobj.at("INTERP_ALGO");
    conf.absErrorBound = jobj.at("ERROR_BOUND");

    SZ_decompress(conf,comp_data.data(),cmpSize,decomp_data);

    //now do the KS-Test and comp ratio.
    auto test = ROOT::Math::GoFTest(orig_data.size(),orig_data.data(),orig_data.size(),decomp_data);
    auto ksVal = test.KolmogorovSmirnov2SamplesTest();
    auto comp_ratio = sizeof(double)*orig_data.size()/(sizeof(char)*cmpSize);

    std::cout<<"KS TEST "<<ksVal<<" CR "<<comp_ratio<<std::endl;
}

int main(){
    compressfile();
    decompressfile();

    return 1;
}