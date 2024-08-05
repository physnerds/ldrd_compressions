#include "nlohmann/json.hpp"

nlohmann::json jfile;

//make sure that the config parameters are consistent with 
//what is being written in the json file.
std::string config_sz3(){
    return std::string("ALGO_OPTIONS, EB_OPTIONS, INTERP_ALGO,ERROR_BOUND_TYPE,ABS_ERROR_BOUND,REL_ERROR_BOUND,PSNR_ERROR_BOUND,L2NORM_ERROR_BOUND");
}

//If adding new parameters, add this in this function as well...
//need to find better way here....
std::string config_mgard(){
    return std::string("SMOOTHNESS, TOLERANCE,NUM_DIMS,SIZE");
}

std::string config_idealem(){
    return std::string("PRECISION, ALPHA_THRESHOLD, MAX_INPUT_READ, RESIDUAL, DELTA, RANGE, RANGEMIN, RAGEMAX, HISTORY_BUFFER");
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
    assert(alg_name=="MGARD"|| alg_name=="SZ3"|| alg_name=="IDEALEM");
    jfile["Algorithm"] = alg_name;
    if(alg_name=="MGARD")jfile["Par_name"] = config_mgard();
    if(alg_name=="SZ3")jfile["Par_name"] = config_sz3();
    if(alg_name=="IDEALEM")jfile["Par_name"] = config_idealem();
    WriteJSonParameter(args...);
}
