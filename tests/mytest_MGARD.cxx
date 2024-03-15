#include <cmath>
#include <cstddef>

#include <algorithm>
#include <array>
#include <iostream>
#include <vector>
#include "H5Cpp.h"
#include "Math/GoFTest.h"
#include "mgard/compress.hpp"
#include "compression_interface/WriteConfigParameters.h"



//More general example available here:
//https://github.com/CODARcode/MGARD/blob/master/examples/compression/src/main.cpp

const std::string filename = "/home/abashyal/compressiontests/waveforms.h5";



template<typename ...Args>
void WriteJSONConfig(std::string alg_name,Args... args){
    //need to make sure that naming scheme is safeguarded.
    assert(alg_name=="MGARD"|| alg_name=="SZ3");
    jfile["Algorithm"] = alg_name;
    if(alg_name=="MGARD")jfile["Par_name"] = config_mgard();
    if(alg_name=="SZ3")jfile["Par_name"] = config_sz3();
    WriteJSonParameter(args...);
}



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

int main(){
    H5::H5File file(filename,H5F_ACC_RDONLY);
    std::string dsetname = "waveform_1";
    std::vector<double> out_data = ReturnH5Data<double>(file,dsetname);  

    //mgard requires to create the mesh hierarchy.
    //however for uniform mess hierarchy, only the 
    //number of data points is required.
    std::cout << "creating mesh hierarchy ...";
    //const mgard::TensorMeshHierarchy<1, float> hierarchy({N}, coordinates);
    const mgard::TensorMeshHierarchy<1,double>hierarchy({out_data.size()});
    std::cout << " done" << std::endl;

    //smoothness parameter (s)
    //s = 0 ->L2 
    //s = ? --> L1
    const double s = 1;
    const double tolerance = 1E-3;
    WriteJSONConfig("MGARD",std::make_pair("tolerance",s),std::make_pair("smoothness",tolerance));
    std::cout<<"Dumping JSON CONTENT \n"<<jfile.dump(4)<<std::endl; 
    std::cout << "compressing ...";
    const mgard::CompressedDataset<1, double> compressed =
    mgard::compress(hierarchy, out_data.data(), s, tolerance);
    std::cout << " done" << std::endl;

    std::cout << "compression ratio: "
        << static_cast<double>(out_data.size()) / compressed.size()
        << std::endl;

    // now the decompression...
    std::cout << "decompressing ...";
    const mgard::DecompressedDataset<1, double> decompressed =
      mgard::decompress(compressed);
    std::cout << " done" << std::endl; 

    double *const out_copy = new double[out_data.size()];
    std::copy(out_data.data(), out_data.data() + out_data.size(),out_copy);

    // Compression error.
    double *const error = new double[out_data.size()];
    // The `data` member function returns a pointer to the decompressed dataset.
    std::transform(out_copy, out_copy + out_data.size(), decompressed.data(), error,
        std::minus<double>());
    delete[] out_copy;

    //mgard has its own error computation method https://github.com/CODARcode/MGARD/blob/master/examples/decompression/src/main.cpp
    //But we will stick the KS test for the sake of uniformity in test.
    /* 
    // To compute the magnitude of the compression error, we use the internal
    // functions `mgard::norm` and `mgard::shuffle`. Important: these functions
    // are not part of the API, so they should not be considered stable.
    double *const shuffled = new double[out_data.size()];
    mgard::shuffle(hierarchy, error, shuffled);
    delete[] error;
    std::cout << "error tolerance: " << tolerance << std::endl
            << "achieved error: " << mgard::norm(hierarchy, shuffled, s)
            << std::endl;
    delete[] shuffled;
    */

    auto test = ROOT::Math::GoFTest(out_data.size(),out_data.data(),out_data.size(),decompressed.data());
    auto ksVal = test.KolmogorovSmirnov2SamplesTest();

    auto comp_ratio = sizeof(double)*out_data.size()/(sizeof(double)*compressed.size());
    std::cout<<"ksVal "<<ksVal<<" "<<comp_ratio<<std::endl;
    

    return 1;
}

