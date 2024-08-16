#ifndef COMPRESSIONTOOLS_HXX
#define COMPRESSIONTOOLS_HXX
#include <vector>
#include <fstream>
#include <nlohmann/json.hpp>
#include "H5Cpp.h"
#include "Math/GoFTest.h"
#include "mgard/compress.hpp"


class Compressor {
protected:
    nlohmann::json params;
public:
    Compressor(const std::string& paramFile,bool storeData= false) {
        std::ifstream file(paramFile);
        if (file.is_open()) {
            file >> params;
        }
    }
    //virtual char* compress(const std::vector<float>& data) = 0;
    //virtual float* decompress(const std::vector<float>& data) = 0;
    virtual ~Compressor() {}

    virtual void dumpJSONContent(){
      std::cout<<"JSON \n"<<params.dump(4)<<std::endl;
    }

    virtual void UpdateJSONContent(nlohmann::json& jobj){
        for(auto it = params.begin(); it != params.end(); ++it){
            if(jobj.find(it.key())==jobj.end()){
                jobj[it.key()] = it.value();
            } else if(it->is_object()){
                UpdateJSONContent(jobj[it.key()]);
            }
        }
    }

};
#endif
