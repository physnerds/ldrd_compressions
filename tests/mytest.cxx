#include "SZ3/api/sz.hpp"
//#include "TROOT.h"
#include "H5Cpp.h"

#include <iostream>

const std::string filename = "/home/abashyal/compressiontests/waveforms.h5";

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

int main(){ 
    H5::H5File file(filename,H5F_ACC_RDONLY);
    std::string dsetname = "waveform_0";
    std::vector<double> out_data = ReturnH5Data<double>(file,dsetname);
    SZ3::Config conf(out_data.size()); //AB: Getting the config as it is from example
    conf.cmprAlgo = SZ3::ALGO_INTERP_LORENZO;
    conf.errorBoundMode = SZ3::EB_ABS; //Other error control modes also available.
    conf.absErrorBound = 1E-3; 
    size_t cmpSize;
    char* compressed_data = compress_data<double>(conf, out_data.data(),cmpSize);
    //now decompress the data
    double* decompressed_data = decompress_data<double>(conf,compressed_data,cmpSize,out_data.size());

    std::cout<<cmpSize<<" "<<out_data.size()<<std::endl;

    double sum_original=0.0;
    double sum_decompressed = 0.0;
    for(int i = 0;i<out_data.size();i++){
        sum_original += out_data[i];
        sum_decompressed += decompressed_data[i];
       // if(out_data[i]<abs(1E-3)) std::cout<<out_data[i]<<" "<<decompressed_data[i]<<std::endl;
    }

    std::cout<<"Sum original/decompressed data "<<sum_original<<" "<<sum_decompressed<<std::endl;
}