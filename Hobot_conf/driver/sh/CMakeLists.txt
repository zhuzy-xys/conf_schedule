#CMakelist sh

project(hconf_sh)
cmake_minimum_required(VERSION 2.6)
include_directories(
    ${PROJECT_SOURCE_DIR}/../c++/include
    )

aux_source_directory(. DIR_SRCS)

#add_definitions(${AGENT_COMPILE_FLAGS})
add_executable(hconf_client ${DIR_SRCS})

target_link_libraries (hconf_client hconf_static)

set_target_properties(hconf_client PROPERTIES OUTPUT_NAME hconf)
install(TARGETS hconf_client DESTINATION bin)
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    install(CODE "EXECUTE_PROCESS (COMMAND ln -sf
    ${CMAKE_INSTALL_PREFIX}/bin/hconf /usr/local/bin/hconf)")
endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
