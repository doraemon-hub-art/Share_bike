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
    
    //�õ���Ϣ,�ֵ�,��������:�����е���Ϣ��Ĭ��ֵ(���û�ҵ���ȡ���Ĭ��ֵ)
    const char* ip = iniparser_getstring(ini,"database:ip","127.0.0.1");
    const int   port  = iniparser_getint(ini, "database:port", 3306);
    const char* user  = iniparser_getstring(ini, "database:user", "root");
    const char* pwd   = iniparser_getstring(ini, "database:pwd", "123456");
    const char* db    = iniparser_getstring(ini, "database:db", "dongnaobike");
    const int   sport = iniparser_getint(ini, "server:port", 9090);

    //��Ϣ��ֵ
    _config = st_env_config(std::string(ip), port, std::string(user),
        std::string(pwd), std::string(db), sport);

    //�ͷ��ֵ���Դ
    iniparser_freedict(ini);
    _isloaded = true;
    return  _isloaded;
}

const st_env_config& Iniconfig::getconfig(){
    return _config;
}