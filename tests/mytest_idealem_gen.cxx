#undef R__HAS_VDT
//C++ headers
#include <stdio.h>
#include <stdlib.h>
//IDEALEM headers
#include "idealem_common.h"
#include "idealem_util.h"
#include "idealem_file_stream.h"
#include "idealem_api.h"
//RNTUPLE headers
#include <ROOT/RNTuple.hxx>
#include <ROOT/RNTupleModel.hxx>
//Compression Test Headers
#include "compression_interface/WriteConfigParameters.h"
#include "compression_interface/Utilities.hxx"

R__LOAD_LIBRARY(ROOTNTUPLE)

using namespace ROOT::Experimental;
//IDEALEM compression tests....
//unlike SZ3 and MGARD, IDEALEM does I/O on csv file....
//Need an interface that connects textfile I/O to in memory array.
const std::string filename = "/home/abashyal/compressiontests/MJD_NPML_0_1D.hdf5";
std::string dsetname = "raw_dset_1";
const char* fname = "test_compression.root";

int main(){
    H5::H5File file(filename, H5F_ACC_RDONLY);
    std::vector<double>out_data = ReturnH5Data<double>(file,dsetname);
    char* csv_fname = "temp.csv";
    WriteCSVFile<double>(out_data,csv_fname);
    char* test = "ks";
    set_input_file(csv_fname);
    use_built_in_test(test);
    set_alpha_threshold(0.01);
    set_precision(2);
    set_residual();
    set_encoding_input_to_text();
    set_encoding();
    set_history_buffer(254);
    //set_encoding_output_to_text();
    run_idealem();

    set_decoding_only();
    set_dimension(1);
    //set_decoding_input_to_text();
    run_idealem();
    return 1;
}