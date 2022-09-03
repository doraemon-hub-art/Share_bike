# CMake要求最低版本
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# 工程名
PROJECT(share_bike)

# 将指定目录添加到编译器的头文件搜索路径之下
INCLUDE_DIRECTORIES(../third/include)
INCLUDE_DIRECTORIES(./common)

# 将指定目录添加到需要链接的库文件目录之下
LINK_DIRECTORIES(../third/lib/iniparser)
LINK_DIRECTORIES(../third/lib/log4cpp)
# 刚才在子文件夹中生成的库
LINK_DIRECTORIES(./common)

# 搜集所有在指定路径下的源文件的文件名，将输出的结果列表存储在指定的变量中
# 内置变量CMAKE_SOURCE_DIR 定义了顶级CMakeLists.txt所在的文件夹
# PROJECT_SOURCE_DIR 定义了包含最近的project()命令的CMakeLists.txt 所在的文件夹
aux_source_directory(${PROJECT_SOURCE_DIR} SOURCE_FILES)

# 使用给定的源文件，为工程引入一个可执行文件
ADD_EXECUTABLE(share_bike ${SOURCE_FILES})

# GCC编译选项
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -rdynamic -Wall -m64 -pipe -std=c++0x -lrt -Wno-reorder -Wdeprecated-declarations")

TARGET_LINK_LIBRARIES(share_bike iniparser)
TARGET_LINK_LIBRARIES(share_bike common)
TARGET_LINK_LIBRARIES(share_bike pthread)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} liblog4cpp.a)


# 增加子目录
ADD_SUBDIRECTORY(common)

SET(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR})
INSTALL(TARGETS share_bike DESTINATION bin)