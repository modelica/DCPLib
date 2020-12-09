cmake_minimum_required(VERSION 3.5)

# Project
project(DCPLib VERSION 0.1)

# use c++11 features, requires cmake > 3.1
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

option(BUILD_ALL "Build all parts of the DCPLib" ON)
option(BUILD_ETHERNET "Build neccessary parts to use UDP driver." OFF)
option(BUILD_BLUETOOTH "Build neccessary parts to use the Bluetooth RFCOMM driver." OFF)
option(BUILD_SLAVE "Build neccessary parts to implement an DCP slave." OFF)
option(BUILD_MASTER "Build neccessary parts to implement an DCP master." OFF)
option(BUILD_XML "Build neccessary parts for handling slave description XML files" OFF)
option(BUILD_ZIP "Build neccessary parts for handling slave zip files" OFF)


add_definitions(-DDEBUG)

##dcplib core
add_library(Core INTERFACE)
add_library(DCPLib::Core ALIAS Core)


target_include_directories(Core INTERFACE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/core>
        $<INSTALL_INTERFACE:include/DCPLib/core>
        )

install(TARGETS Core
        EXPORT DCPLib-targets
        COMPONENT Core
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/core DESTINATION include/DCPLib)

if(BUILD_ALL OR BUILD_ETHERNET)
    find_package(Threads REQUIRED)
	find_package(ASIO REQUIRED )

    ##dcplib ethernet
    add_library(Ethernet INTERFACE)
    add_library(DCPLib::Ethernet ALIAS Ethernet)

    target_link_libraries(Ethernet INTERFACE Threads::Threads ASIO::ASIO)
    target_link_libraries(Ethernet INTERFACE DCPLib::Core)
    if(WIN32)
        target_link_libraries(Ethernet INTERFACE wsock32 ws2_32)
    endif()

    target_include_directories(Ethernet INTERFACE
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/ethernet>
            $<INSTALL_INTERFACE:include/DCPLib/ethernet>
            )

    install(TARGETS Ethernet
            EXPORT DCPLib-targets
            COMPONENT Ethernet
            ARCHIVE DESTINATION lib
            LIBRARY DESTINATION lib
            RUNTIME DESTINATION bin)

    install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ethernet DESTINATION include/DCPLib)
endif(BUILD_ALL OR BUILD_ETHERNET)

if(BUILD_ALL OR BUILD_BLUETOOTH)

    ##dcplib bluetooth
    add_library(Bluetooth INTERFACE)
    add_library(DCPLib::Bluetooth ALIAS Bluetooth)

    target_link_libraries(Bluetooth INTERFACE DCPLib::Core)

    target_include_directories(Bluetooth INTERFACE
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/bluetooth>
            $<INSTALL_INTERFACE:include/DCPLib/bluetooth>
            )

    install(TARGETS Bluetooth
            EXPORT DCPLib-targets
            COMPONENT Bluetooth
            ARCHIVE DESTINATION lib
            LIBRARY DESTINATION lib
            RUNTIME DESTINATION bin)

    install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/bluetooth DESTINATION include/DCPLib)
endif(BUILD_ALL OR BUILD_BLUETOOTH)

if(BUILD_ALL OR BUILD_MASTER)
    ##dcplib master
    add_library(Master INTERFACE)
    add_library(DCPLib::Master ALIAS Master)

    target_link_libraries(Master INTERFACE DCPLib::Core)
    if(WIN32)
        target_link_libraries(Master INTERFACE wsock32 ws2_32)
    endif()

    target_include_directories(Master INTERFACE
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/master>
            $<INSTALL_INTERFACE:include/DCPLib/master>
            )

    install(TARGETS Master
            EXPORT DCPLib-targets
            COMPONENT Master
            ARCHIVE DESTINATION lib
            LIBRARY DESTINATION lib
            RUNTIME DESTINATION bin)

    install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/master DESTINATION include/DCPLib)
endif(BUILD_ALL OR BUILD_MASTER)

if(BUILD_ALL OR BUILD_SLAVE)
    ##dcplib slave
    add_library(Slave INTERFACE)
    add_library(DCPLib::Slave ALIAS Slave)

    target_link_libraries(Slave INTERFACE DCPLib::Core)
    if(WIN32)
        target_link_libraries(Slave INTERFACE wsock32 ws2_32)
    endif()

    target_include_directories(Slave INTERFACE
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/slave>
            $<INSTALL_INTERFACE:include/DCPLib/slave>
            )

    install(TARGETS Slave
            EXPORT DCPLib-targets
            COMPONENT Slave
            ARCHIVE DESTINATION lib
            LIBRARY DESTINATION lib
            RUNTIME DESTINATION bin)

    install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/slave DESTINATION include/DCPLib)
endif(BUILD_ALL OR BUILD_SLAVE)

if(BUILD_ALL OR BUILD_XML)
    find_package(XercesC REQUIRED)

    add_library(Xml INTERFACE)
    add_library(DCPLib::Xml ALIAS Xml)

    target_link_libraries(Xml INTERFACE DCPLib::Core)
    target_link_libraries(Xml INTERFACE XercesC::XercesC)

    target_include_directories(Xml INTERFACE
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/xml>
            $<INSTALL_INTERFACE:include/DCPLib/xml>
            )

    install(TARGETS Xml
            EXPORT DCPLib-targets
            COMPONENT Xml
            ARCHIVE DESTINATION lib
            LIBRARY DESTINATION lib
            RUNTIME DESTINATION bin)

    install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/xml DESTINATION include/DCPLib)
endif(BUILD_ALL OR BUILD_XML)

if(BUILD_ALL OR BUILD_ZIP)
	find_package(Zip REQUIRED)

    add_library(Zip INTERFACE)
    add_library(DCPLib::Zip ALIAS Zip)

    target_link_libraries(Zip INTERFACE DCPLib::Xml ZIP::ZIP)

    target_include_directories(Zip INTERFACE
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/zip>
            $<INSTALL_INTERFACE:include/DCPLib/zip>
            )

    install(TARGETS Zip
            EXPORT DCPLib-targets
            COMPONENT Zip
            ARCHIVE DESTINATION lib
            LIBRARY DESTINATION lib
            RUNTIME DESTINATION bin)

    install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/zip DESTINATION include/DCPLib)
endif(BUILD_ALL OR BUILD_ZIP)


install(EXPORT DCPLib-targets
        FILE
            DCPLibConfig.cmake
        NAMESPACE
            DCPLib::
        DESTINATION
            lib/DCPLib)


add_executable(mytest src/test/BasicChecks.cpp)
target_link_libraries(mytest DCPLib::Ethernet DCPLib::Bluetooth DCPLib::Master DCPLib::Slave DCPLib::Xml DCPLib::Zip)

