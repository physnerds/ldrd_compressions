#include "SZ3/api/sz.hpp"
#include "SZ3/utils/Config.hpp"
#include "H5Cpp.h"
#include "Math/GoFTest.h"
#include "TGraph.h"
#include "TMultiGraph.h"
#include "TCanvas.h"
#include <iostream>

const std::string filename = "/home/abashyal/compressiontests/waveforms_ver2.h5";


template<typename T>
std::vector<T> ReturnH5Data(H5::H5File file, std::string dsetname){
    
    H5::DataSet dset = file.openDataSet(dsetname);
    H5::DataType dtype = dset.getDataType();
    H5::DataSpace dspace = dset.getSpace();

    int ndims = dspace.getSimpleExtentNdims();
    hsize_t dims_out[ndims];
    dspace.getSimpleExtentDims(dims_out,NULL);

    std::vector<T>out_data(dims_out[0]);

    dset.read(out_data.data(),H5::PredType::NATIVE_DOUBLE);

    return out_data;
}

////////////////////////////////////////////////////////
//This one just to test the SZ3 functionalities....
template <typename T>
char* compress_data(SZ3::Config &conf, T* input_data, size_t& cmpSize){
    //These tests are basically what are available in SZ3/api/sz.hpp     
    char* compressed_data = SZ_compress(conf,input_data,cmpSize);
    return compressed_data;
    
}
////////////////////////////////////////////////////////
template<typename T>
T* decompress_data(SZ3::Config &conf,char* input_data,size_t& cmpSize, size_t input_size){
    T* decData = new T[input_size];
    SZ_decompress(conf, input_data,cmpSize,decData);
    return decData;
}

void DrawGraphs(double* orig_data, double* decomp_data, size_t dat_size,std::string cfname, double ksVal, int comp_ratio){
    auto gr_orig = new TGraph(dat_size,orig_data);
    auto gr_decomp = new TGraph(dat_size,decomp_data);
    std::vector<double> diff_val;
    for(int i =0;i<dat_size;i++)diff_val.push_back(orig_data[i]-decomp_data[i]);

    auto gr_diff = new TGraph(dat_size,diff_val.data());

    gr_decomp->SetLineColor(kBlue);
    gr_diff->SetLineColor(kRed);
    gr_orig->SetLineWidth(3);
    gr_diff->SetLineWidth(3);
    gr_decomp->SetLineWidth(3);
    gr_orig->SetTitle("Original");
    gr_decomp->SetTitle("Decomposed");
    gr_diff->SetTitle("Difference");

    auto mg = new TMultiGraph();
    std::string mg_title = "KS-VAL: "+std::to_string(ksVal)+", CR: "+std::to_string(comp_ratio);
    //mg->Add(gr_orig,"l");
    mg->Add(gr_diff,"l");
    //mg->Add(gr_decomp,"l");
    mg->SetTitle(mg_title.c_str());

    auto c = new TCanvas("c","c",750,750);
    c->cd();
    mg->Draw("a");
    c->BuildLegend();
    c->SaveAs(cfname.c_str());
}

//function that does compression and decompression test based on Config parameters....
//Parameters of SZ3 Interest:
//1. SZ3::EB
//2. SZ3::ALGO
//3. SZ3::INTERP_ALGO
//4. Error Bound --> Number
void test_compression(SZ3::EB eb, SZ3::ALGO algo, SZ3::INTERP_ALGO ialgo,SZ3::Config &conf, float errBound ,std::vector<double> out_data){
    conf.cmprAlgo = algo;
    conf.errorBoundMode =  eb; 
    conf.interpAlgo = ialgo;
    conf.absErrorBound = errBound;

    size_t cmpSize;
    char* compressed_data = compress_data<double>(conf, out_data.data(),cmpSize);
    //now decompress the data
    double* decompressed_data = decompress_data<double>(conf,compressed_data,cmpSize,out_data.size());

    //do a GOF Test between the original and compressed data....
    auto test = ROOT::Math::GoFTest(out_data.size(),out_data.data(),out_data.size(),decompressed_data);

    auto ksVal = test.KolmogorovSmirnov2SamplesTest();
    auto  comp_ratio = sizeof(double)*out_data.size()/(sizeof(char)*cmpSize);
    std::cout<<SZ3::enum2Str(eb)<<","<<SZ3::enum2Str(algo)<<","<<SZ3::enum2Str(ialgo)<<","<<ksVal<<","<<comp_ratio<<std::endl;

    std::string fname = std::string(SZ3::enum2Str(eb))+"_"+SZ3::enum2Str(algo)+"_"+SZ3::enum2Str(ialgo)+".png";

    DrawGraphs(out_data.data(), decompressed_data,out_data.size(),fname, ksVal, comp_ratio);

}


int main(){ 
    H5::H5File file(filename,H5F_ACC_RDONLY);
    std::string dsetname = "waveform_1";
    std::vector<double> out_data = ReturnH5Data<double>(file,dsetname);
    std::cout<<"Error Bound,Algorithm, Interpolation Algorithm,KSVAL,Compression-Ratio"<<std::endl;
    for(auto al_val: SZ3::ALGO_OPTIONS){
        for(auto al_eb: SZ3::EB_OPTIONS){
            for(auto al_ial: SZ3::INTERP_ALGO_OPTIONS){
                    SZ3::Config conf(out_data.size()); //AB: Getting the config as it is from example
                    test_compression(al_eb, al_val,al_ial,conf,1E-5,out_data);
            }
        }
    }
    
}