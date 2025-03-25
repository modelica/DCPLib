cmake_minimum_required(VERSION 3.5)

# use c++11 features, requires cmake > 3.1
set(CMAKE_CXX_STANDARD 11)

# Project
project(dcp_msd_master)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

add_definitions("-DDEBUG")

if (WIN32)
    add_definitions(-D_WIN32_WINNT=0x0600 -D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS)
endif()

find_package(DCPLib REQUIRED)
find_package(Threads REQUIRED)
find_package(XercesC REQUIRED)
find_package(ASIO REQUIRED )


include_directories("src")

file(GLOB_RECURSE dcp_msd_master_src_files src/*.cpp src/*.h )
add_executable(dcp_msd_master ${dcp_msd_master_src_files})
target_link_libraries(dcp_msd_master DCPLib::Ethernet DCPLib::Master DCPLib::Xml)

add_custom_command(TARGET dcp_msd_master POST_BUILD
  COMMAND "${CMAKE_COMMAND}" -E copy
    ${CMAKE_SOURCE_DIR}/MSD1-Slave-Description.xml
    $<TARGET_FILE_DIR:dcp_msd_master>
  COMMAND "${CMAKE_COMMAND}" -E copy
    ${CMAKE_SOURCE_DIR}/MSD2-Slave-Description.xml
    $<TARGET_FILE_DIR:dcp_msd_master>
  COMMENT "Copied description XML file."
)

install(TARGETS dcp_msd_master DESTINATION bin)