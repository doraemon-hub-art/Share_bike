#ifndef SHBK_COMMON_INICONFIG_H_
#define SHBK_COMMON_INICONFIG_H_


#include <string>
#include "configdef.h"

class Iniconfig{

public:
    Iniconfig();
    ~Iniconfig();

    //º”‘ÿ≈‰÷√    
    bool loadfile(const std::string& path);
    //ªÒ»°≈‰÷√
    const st_env_config& getconfig();

private:
    st_env_config _config;
    bool _isloaded;

};


#endif //SHBK_COMMON_INICONFIG_H_