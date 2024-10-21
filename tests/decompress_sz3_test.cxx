/*
/* Example  code that reads compressed data from TTree and decompresses it and plots 2D histogram.
*/

#include "H5Cpp.h"
#include <iostream>
#include "compression_interface/Utilities.hxx"
#include "compression_interface/WriteConfigParameters.h"
#include "compression_interface/SZ3CompressionTools.hxx"
#include "TFile.h"
#include "TTree.h"
#include "TH2D.h"
#include "TH1D.h"

const std::string rfilename = "test_SZ3_compression_err50.root";
const std::string ntuplename = "Compressions";
const int n_channels = 10240;
const int n_ticks = 5859;
int main(){
    std::vector<char>* fieldCompressedData = nullptr;
    std::vector<double>* fieldOutData = nullptr;
    std::string*param = nullptr;
    TFile *infile = new TFile(rfilename.c_str(),"READ");
    TTree *tree = infile->Get<TTree>(treename.c_str());
    tree->SetBranchAddress("compressed_data",&fieldCompressedData);
    tree->SetBranchAddress("out_data",&fieldOutData);
    tree->SetBranchAddress("parameters",&param);

    auto nentries = tree->GetNEntries();
    TH2D* decompressed_hist = new TH2D("decompressed_value","decompressed_value",n_channels,-0.5,10239.5,n_ticks,0,n_ticks);
    TH2D* original_hist = (TH2D*)decompressed_hist->Clone("original_value");
    TH1D* comp_ratio = new TH1D("compression_ratio","compression_ratio",n_channels,-0.5,10239.5);

    std::cout<<"Total Entries "<<nentries<<std::endl;
    auto fieldCompressedData = reader->GetView<std::vector<char>>("compressed_data");
    auto fieldOutData = reader->GetView<std::vector<double>>("out_data");
    auto param = reader->GetView<std::string>("parameters");

    for(size_t entry = 0; entry<nentries;entry++){
        std::string temp_param = param(entry);
        std::string temp_jfilename = "temp_jfilename.json";
        nlohmann::json jobj = readJSONString(temp_param);
        int channel_number =  static_cast<int>(jobj.at("ORIGINAL DSET"));
        double ratio = static_cast<double>(jobj.at("COMPRESSION_RATIO"));
        writeJSON(jobj,temp_jfilename);
        //std::cout<<temp_param<<std::endl;
        std::vector<char> compressed_data = *fieldCompressedData;
        std::vector<double>orig_data = *fieldOutData;
        SZ3Compressor sz3compress(temp_jfilename);
        //now do the decompression
        double* decompressed_data = sz3compress.decompress<double>(compressed_data.data());
        for(int j=0;j<n_ticks;j++){ //y axis is the number of ticks
            decompressed_hist->SetBinContent(channel_number,j,decompressed_data[j]);
            original_hist->SetBinContent(channel_number,j,orig_data[j]);
        }
        comp_ratio->SetBinContent(channel_number,ratio);
        
    }
    TFile* rfile = new TFile("decompression_info_err50.root","RECREATE");
    rfile->cd();
    decompressed_hist->Write();
    comp_ratio->Write();
    original_hist->Write();
    rfile->Close();
}