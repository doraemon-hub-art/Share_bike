#include "iniconfig.h"
#include <iniparser/iniparser.h>

Iniconfig::Iniconfig():_isloaded(false){

}

Iniconfig::~Iniconfig(){

}

bool Iniconfig::loadfile(const std::string& path){
    dictionary* ini = NULL;
    ini = iniparser_load(path.c_str());
    if(ini == NULL){
        fprintf(stderr,"cannot parse file:%s\n",path.c_str());
        return false;
    }
    
    //拿到信息,字典,——区段:区段中的信息，默认值(如果没找到就取这个默认值)
    const char* ip = iniparser_getstring(ini,"database:ip","127.0.0.1");
    const int   port  = iniparser_getint(ini, "database:port", 3306);
    const char* user  = iniparser_getstring(ini, "database:user", "root");
    const char* pwd   = iniparser_getstring(ini, "database:pwd", "123456");
    const char* db    = iniparser_getstring(ini, "database:db", "dongnaobike");
    const int   sport = iniparser_getint(ini, "server:port", 9090);

    //信息赋值
    _config = st_env_config(std::string(ip), port, std::string(user),
        std::string(pwd), std::string(db), sport);

    //释放字典资源
    iniparser_freedict(ini);
    _isloaded = true;
    return  _isloaded;
}

const st_env_config& Iniconfig::getconfig(){
    return _config;
}