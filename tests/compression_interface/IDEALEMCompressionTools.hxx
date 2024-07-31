#ifndef IDEALEMCOMPRESSIONTOOLS_HXX
#define IDEALEMCOMPRESSIONTOOLS_HXX

#include "CompressionTools.hxx"
#include "Utilities.hxx"
#include "idealem_common.h"
#include "idealem_api.h"
#include "nlohmann/json.hpp"
//class for IDEALEM Compressor

class IDEALEMCompressor : public Compressor {
public:
    IDEALEMCompressor(const std::string&paramfile,const nlohmann::json& userParams = nlohmann::json{}) : Compressor(paramfile){
        params["PRECISION"] = 10;
        params["ALPHA_THRESHOLD"] = 0.05;
        params["MAX_INPUT_READ"] = 1024;
        params["RESIDUAL"] = false; //delta and residual are exclusive 
        params["DELTA"] = false;
        //params["RANGE"] = false; //range-->true for rangemin and rangemax
        //params["RANGEMIN"] = 0;
        //params["RANGEMAX"] = 0;
        params["HISTORY_BUFFER"] = 1;
        //default is the ks
        //params["TEST"] = "ks"; //ks, mjc, dtw

            // Override default parameters with user-provided parameters
        for (const auto& [key, value] : userParams.items()) {
            if (value.is_number_integer()) {
                params[key] = value.get<int>();
            } else if (value.is_boolean()) {
                params[key] = value.get<bool>();
            } else if (value.is_number_float()) {
                params[key] = value.get<double>();
            } else if (value.is_string()) {
                params[key] = value.get<std::string>();
            }
        }    

     }
    char* compress(const std::vector<float>&input_data) {
        //IDEALEM reads the input from csv file...so need to create
        //a csv file and then compress it...
        //not ideal scenario but simpler way for now..
        char* csv_fname = "temp.csv";
        WriteCSVFile<float>(input_data,csv_fname);
        set_input_file(csv_fname);
        std::string test = static_cast<std::string>(params.at("TEST"));
        double precision = static_cast<int>(params.at("PRECISION"));
        double threshold = static_cast<double>(params.at("ALPHA_THRESHOLD"));
        bool res = static_cast<bool>(params.at("RESIDUAL"));
        bool delta = static_cast<bool>(params.at("RANGE"));
        int h_buffer  = static_cast<bool>(params.at("HISTORY_BUFFER"));
        set_input_file(csv_fname);

        //const char* to char*....
        std::vector<char> test_cstr(test.begin(), test.end());
        test_cstr.push_back('\0');
        use_built_in_test(test_cstr.data());
        set_alpha_threshold(threshold);
        set_precision(precision);
        if(res)set_residual();
        set_history_buffer(h_buffer);
        run_idealem(); 

    }
template<typename T>
    T* decompress(const char* comp_data) {
        //because of how the API is constructed, dump data into CSV.....
        //TODO: dump data partially for very large data.
        std::string csv_fname = "temp.csv";
        std::string enc_fname = csv_fname+".enc";
        std::string dec_fname = csv_fname+".dec";
        set_input_file(const_cast<char*>(csv_fname.c_str()));
        set_decoding_only();
        run_idealem();

        //now read the content from the CSV file...
        std::vector<T> out_data = ReadCSVFile<T>(dec_fname);
        return out_data.data();
    }

};

#endif


