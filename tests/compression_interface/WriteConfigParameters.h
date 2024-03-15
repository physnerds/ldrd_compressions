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