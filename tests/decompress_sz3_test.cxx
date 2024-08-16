/*
/* Example  code that reads compressed data from RNTuple and decompresses it and plots 2D histogram.
*/

#include "H5Cpp.h"
#include <iostream>
#include "compression_interface/Utilities.hxx"
#include "compression_interface/WriteConfigParameters.h"
#include "compression_interface/SZ3CompressionTools.hxx"

const std::string rfilename = "test_SZ3_compression.root";
const std::string ntuplename = "Compressions";
int main(){
    auto reader = ROOT::Experimental::RNTupleReader::Open(ntuplename.c_str(),rfilename.c_str());
    reader->PrintInfo();
    auto nentries = reader->GetNEntries();
    auto fieldOrigData = reader->GetView<std::vector<double>>("out_data");
    auto fieldCompressedData = reader->GetView<std::vector<char>>("compressed_data");
    auto param = reader->GetView<std::string>("parameters");

    for(size_t entry = 0; entry<2;entry++){
        std::string temp_param = param(entry);
        std::string temp_jfilename = "temp_jfilename.json";
        nlohmann::json jobj = readJSONString(temp_param);
        writeJSON(jobj,temp_jfilename);
        std::cout<<jobj.dump()<<std::endl;
        std::vector<char> compressed_data = fieldCompressedData(entry);
        SZ3Compressor sz3compress(temp_jfilename);
        //now do the decompression
        double* decompressed_data = sz3compress.decompress<double>(compressed_data.data());


    }
}