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
//const std::string filename = "/home/abashyal/compressiontests/MJD_NPML_0_1D.hdf5";
const std::string filename = "/home/abashyal/compressiontests/waveforms_ver2.h5";
std::string dsetname = "waveform_1";
const char* fname = "test_compression.root";

std::vector<char>ReadBinary(const std::string& fname){
    std::ifstream file(fname,std::ios::binary);
    if(!file){
        throw std::runtime_error("Cannot Open File");
    }
    file.seekg(0,std::ios::end);
    std::streamsize sz = file.tellg();
    file.seekg(0,std::ios::beg);

    //reading the content of the file into vector.
    std::vector<char>buffer(sz);
    if(!file.read(buffer.data(),sz)){
        throw std::runtime_error("Cannot read file");
    }
    file.close();
    return buffer;
}


void compress(){
    H5::H5File file(filename, H5F_ACC_RDONLY);
    std::vector<double>out_data = ReturnH5Data<double>(file,dsetname);
    char* csv_fname = "temp.csv";
    WriteCSVFile<double>(out_data,csv_fname);
    char* test = "ks";
    set_input_file(csv_fname);
    use_built_in_test(test);
    set_alpha_threshold(0.01); //parameter
    set_precision(2); //parameter
    set_residual();
    set_encoding_input_to_text();
    set_encoding();
    set_history_buffer(254); //parameter
    set_block_length(64); //parameter
    //set_encoding_output_to_text();
    run_idealem();
    std::string enc_fname = "temp.csv.enc";
    auto buff_vec = ReadBinary(enc_fname);
    //now we just write the data into RNTuple..
    auto model = RNTupleModel::Create();
    auto& compressed_data = *model->MakeField<std::vector<char>>("compressed_data");
    auto& original_data = *model->MakeField<std::vector<double>>("original_data");
    auto ntuple = RNTupleWriter::Recreate(std::move(model),dsetname.c_str(),fname);
    compressed_data.clear();
    original_data.clear();
    original_data = out_data;
    compressed_data = buff_vec;
    ntuple->Fill();
}

void decompress(){
    auto ntuple = RNTupleReader::Open(dsetname.c_str(),fname);
    auto v_data = ntuple->GetView<std::vector<char>>("compressed_data");
    auto orig_data = ntuple->GetView<std::vector<double>>("original_data");
    std::vector<char>comp_vec = v_data(0);
    std::vector<double>orig_vec = orig_data(0);
    std::string enc_fname = "temp_1.csv.enc";
    std::ofstream outfile(enc_fname,std::ios::out | std::ios::binary);
    if(!outfile){
        throw std::runtime_error("Cannot open outfile");
    }
    for(auto entryID: *ntuple){
        if(!comp_vec.empty()){
            outfile.write(comp_vec.data(),comp_vec.size());
        }
    }
    outfile.close();
    // Now we can test with the decompressed data stored in RNTuple
    std::string csv_fname = "temp_1.csv";
    
    set_input_file(const_cast<char*>(csv_fname.c_str()));
    set_decoding_only();
    run_idealem();

    std::string csv_dec_fname = csv_fname+".dec";

    auto out_data = ReadCSVFile(csv_dec_fname);
    //now read back the decompressed content...
    auto comp_ratio = sizeof(double)*orig_vec.size()/(sizeof(char)*comp_vec.size());
    auto test = ROOT::Math::GoFTest(orig_vec.size(),orig_vec.data(),out_data.size(),out_data.data());
    auto ksVal = test.KolmogorovSmirnov2SamplesTest();

    std::cout<<"KS TEST "<<ksVal<<" CR "<<comp_ratio<<std::endl;
    std::cout<<orig_vec.size()<<" "<<out_data.size()<<std::endl;
    DrawGraphs(orig_vec.data(),out_data.data(),orig_vec.size(),"IDEALEM_.png",ksVal,comp_ratio);
}   

int main(){
    compress();
    decompress();

    return 1;
}