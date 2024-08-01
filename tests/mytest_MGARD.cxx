#include <cmath>
#include <cstddef>

#include <algorithm>
#include <array>
#include <iostream>
#include <vector>
#include "H5Cpp.h"
#include "Math/GoFTest.h"
//Interface related headers...
#include "compression_interface/Utilities.hxx"
#include "compression_interface/WriteConfigParameters.h"
#include "compression_interface/MGARDCompressionTools.hxx"

//More general example available here:
//https://github.com/CODARcode/MGARD/blob/master/examples/compression/src/main.cpp

const std::string filename = "/home/abashyal/compressiontests/waveforms.h5";

int main(){
    H5::H5File file(filename,H5F_ACC_RDONLY);
    std::string dsetname = "waveform_0";
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
    WriteJSONConfig("MGARD",
    std::make_pair("TOLERANCE",s),
    std::make_pair("SMOOTHNESS",tolerance),
    std::make_pair("SIZE",out_data.size()),
    std::make_pair("NUM_DIMS",1));

    std::cout<<"Dumping JSON CONTENT \n"<<jfile.dump(4)<<std::endl; 
    
    std::string jfilename =  "params_mgard.json";
    //json file to write the parameters...
    writeJSON(jfile,jfilename);
    MGARDCompressor mgard(jfilename);
    char* compressed_data = mgard.compress<double>(out_data);
    double* decompressed_data = mgard.decompress<double>(compressed_data);
    //see if we got the compression parameters....
    auto ksval = CalculateKSVal<double>(out_data.size(),out_data.data(),decompressed_data);
    std::cout<<"KSVAL is "<<ksval<<std::endl;
    mgard.dumpJSONContent();
    
    //Draw the plots of original and decompressed data
    
    return 1;
}

