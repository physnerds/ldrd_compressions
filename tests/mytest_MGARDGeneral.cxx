#include <cmath>
#include <cstddef>

#include <algorithm>
#include <array>
#include <iostream>
#include <vector>
#include "H5Cpp.h"
#include "Math/GoFTest.h"
#include "mgard/compress_x.hpp"

#include "compression_interface/WriteConfigParameters.h"
#include "compression_interface/Utilities.hxx"

const std::string filename = "/home/abashyal/compressiontests/waveforms.h5";
//const std::string filename = "/home/abashyal/compressiontests/MJD_NPML_0_1D.hdf5";
//std::string dsetname = "raw_dset_1";
std::string dsetname = "waveform_1";

int main(){
    H5::H5File file(filename,H5F_ACC_RDONLY);
    //std::string dsetname = "waveform_1";
    std::vector<double> out_data = ReturnH5Data<double>(file,dsetname);  

    //mgard requires to create the mesh hierarchy.
    //however for uniform mess hierarchy, only the 
    //number of data points is required.
    std::cout << "Preparing data buffers"<<std::endl;
    mgard_x::DIM num_dims = 1;
    mgard_x::SIZE n1 = out_data.size();
    std::vector<mgard_x::SIZE> shape{n1};
    mgard_x::SIZE in_byte = n1*sizeof(double);
    mgard_x::SIZE out_byte;

    //....load into the array.
    double *in_array = out_data.data();
    void *compressed_array = NULL;
    void *decompressed_array = NULL;
    //tol: error tolerance
    //s: smoothness parameter

    double tol = 0.5;
    double s = 0;
    mgard_x::Config config;
    //compressing the data....
    mgard_x::compress(num_dims,mgard_x::data_type::Double,shape,tol,s,mgard_x::error_bound_type::REL, in_array,
        compressed_array,out_byte,config,false);

    //now decompress the data....
    mgard_x::decompress(compressed_array, out_byte, 
        decompressed_array,config,false);

    auto test = ROOT::Math::GoFTest(out_data.size(),out_data.data(),out_data.size(),(double*)decompressed_array);
    auto ksVal = test.KolmogorovSmirnov2SamplesTest();

    auto comp_ratio = in_byte/out_byte;
    std::cout<<"ksVal "<<ksVal<<" "<<comp_ratio<<std::endl;  
    DrawGraphs((double*)decompressed_array,out_data.data(),out_data.size(),"MGARD_2.png",ksVal,comp_ratio);
    return 1;
}

/*
Config::Config() {
  dev_type = device_type::AUTO;
  dev_id = 0;
  compressor = compressor_type::MGARD;
  domain_decomposition = domain_decomposition_type::MaxDim;
  decomposition = decomposition_type::MultiDim;
  estimate_outlier_ratio = 1.0;
  huff_dict_size = 8192;
  huff_block_size = 1024 * 20;
  lz4_block_size = 1 << 15;
  zstd_compress_level = 3;
#if MGARD_ENABLE_COORDINATE_NORMALIZATION
  normalize_coordinates = true;
#else
  normalize_coordinates = false;
#endif
  lossless = lossless_type::Huffman;
  reorder = 0;
  log_level = log::ERR;
  max_larget_level = std::numeric_limits<SIZE>::max(); // no limit
  prefetch = true;
  max_memory_footprint = std::numeric_limits<SIZE>::max(); // no limit
  total_num_bitplanes = 32;
  block_size = 256;
  temporal_dim = 0;
  temporal_dim_size = std::numeric_limits<SIZE>::max();
  mdr_adaptive_resolution = false;
  collect_uncertainty = false;
  adjust_shape = false;
  compress_with_dryrun = false;
  num_local_refactoring_level = 1;
  cache_compressor = false;
}

*/