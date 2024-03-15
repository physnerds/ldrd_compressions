#include "nlohmann/json.hpp"

nlohmann::json jfile;

//make sure that the config parameters are consistent with 
//what is being written in the json file.
std::string config_sz3(){
    return std::string("error_bound_mode, compression_algorim, interp_method");
}

//If adding new parameters, add this in this function as well...
//need to find better way here....
std::string config_mgard(){
    return std::string("smoothness, tolerance");
}


template<typename T>
void WriteJSonParameter(std::pair<const char*, T>parm){
    jfile[parm.first] = parm.second;
}

template<typename T, typename ...Args>
void WriteJSonParameter(std::pair<const char*, T>first_parm, Args...args){
    jfile[first_parm.first] = first_parm.second;
    WriteJSonParameter(args...);
}

template<typename ...Args>
void WriteJSONConfig(std::string alg_name,Args... args){
    //need to make sure that naming scheme is safeguarded.
    assert(alg_name=="MGARD"|| alg_name=="SZ3");
    jfile["Algorithm"] = alg_name;
    if(alg_name=="MGARD")jfile["Par_name"] = config_mgard();
    if(alg_name=="SZ3")jfile["Par_name"] = config_sz3();
    WriteJSonParameter(args...);
}