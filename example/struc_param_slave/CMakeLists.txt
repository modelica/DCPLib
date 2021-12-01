cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 11)


project(struc_param_slave)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

add_definitions("-DDEBUG")

if (WIN32)
    add_definitions(-D_WIN32_WINNT=0x0600 -D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS)
endif()

find_package(DCPLib REQUIRED)
find_package(Threads REQUIRED)
find_package(ASIO REQUIRED )
find_package(XercesC REQUIRED)

include_directories("src")
    
file(GLOB_RECURSE struc_param_slave_src src/*.cpp src/*.hpp)
add_executable(struc_param_slave ${struc_param_slave_src})
target_link_libraries(struc_param_slave DCPLib::Ethernet DCPLib::Xml DCPLib::Slave)


install(TARGETS struc_param_slave DESTINATION bin)

