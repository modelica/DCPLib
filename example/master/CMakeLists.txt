cmake_minimum_required(VERSION 3.5)

# use c++11 features, requires cmake > 3.1
set(CMAKE_CXX_STANDARD 11)

# Project
project(dcpmaster)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

add_definitions("-DDEBUG")

if (WIN32)
    add_definitions(-D_WIN32_WINNT=0x0600 -D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS)
endif()

add_definitions("-DDEBUG")

find_package(DCPLib REQUIRED)
find_package(Threads REQUIRED)
find_package(XercesC REQUIRED)
find_package(ASIO REQUIRED )


include_directories("src")

file(GLOB_RECURSE dcpmaster_src_files src/*.cpp src/*.h )
add_executable(dcpmaster ${dcpmaster_src_files})
target_link_libraries(dcpmaster DCPLib::Ethernet DCPLib::Master DCPLib::Xml)

install(TARGETS dcpmaster DESTINATION bin)