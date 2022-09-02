#include "iniconfig.h" //注意路径，会在CMake中进行调整
#include "configdef.h"

#include "Logger.h"

int main(int argc,char** argv){

    if(argc != 3){//缺少参数
        printf("Please input shbk <config file path> <log file config>!\n");
        return -1;
    }

    if(!Logger::instance()->init(std::string(argv[2]))){
        fprintf(stderr,"init log module failed.\n");
        return -2;
    }


    Iniconfig config;
    if(!config.loadfile(std::string(argv[1]))){
        //printf("load %s failed.\n",argv[1]);
        LOG_ERROR("load %s failed.",argv[1]);
        return -3;
    }

    //测试是否读取成功
    st_env_config conf_args = config.getconfig();
    LOG_INFO("[database]ip:%s\n [database]port:%d\n [database]user:%s\n [database]pwd:%s\n [database]db:%s\n [server]port:%d\n ",
                conf_args.db_ip.c_str(),
                conf_args.db_port,
                conf_args.db_user.c_str(),
                conf_args.db_pwd.c_str(),
                conf_args.db_name.c_str(),
                conf_args.svr_port);
    return 0;
}