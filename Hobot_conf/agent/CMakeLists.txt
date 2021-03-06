#CMakelist agent
if (${HAS_PREFIX})
    set(AGENT_COMPILE_FLAGS "-c -D__STDC_FORMAT_MACROS -DHCONF_INTERNAL
    -DHCONF_CURL_ENABLE
    -DHCONF_AGENT_DIR=${CMAKE_INSTALL_PREFIX} -fPIC -g -pthread -Wall")
else()
    set(AGENT_COMPILE_FLAGS "-c -D__STDC_FORMAT_MACROS -DHCONF_CURL_ENABLE
    -DHCONF_AGENT_DIR=${CMAKE_INSTALL_PREFIX} -fPIC -g -pthread -Wall")
endif()
#set(LINK_FLAGS "-lpthread -lrt")

project(hconf-agent)
cmake_minimum_required(VERSION 2.6)
include_directories(
    ${PROJECT_SOURCE_DIR}/../base
    ${PROJECT_SOURCE_DIR}/../base/qlibc
    ${PROJECT_SOURCE_DIR}/../deps/curl/_install/include
    ${PROJECT_SOURCE_DIR}/../deps/zookeeper/_install/include/zookeeper
    ${PROJECT_SOURCE_DIR}/../deps/gdbm/_install/include
    )

aux_source_directory(. DIR_SRCS)
aux_source_directory(../base DIR_SRCS)
aux_source_directory(../base/qlibc DIR_SRCS)

set_source_files_properties(../base/qlibc/md5.c PROPERTIES LANGUAGE CXX )
set_source_files_properties(../base/qlibc/qhash.c PROPERTIES LANGUAGE CXX )
set_source_files_properties(../base/qlibc/qhasharr.c PROPERTIES LANGUAGE CXX )

add_definitions(${AGENT_COMPILE_FLAGS})
add_executable(hconf_agent ${DIR_SRCS})

# link libpthread
find_package (Threads)
target_link_libraries (hconf_agent ${CMAKE_THREAD_LIBS_INIT})

IF(WIN32)
    MESSAGE(STATUS "This is windows")
ELSEIF(APPLE)
    MESSAGE(STATUS "This is apple")
ELSEIF(UNIX)
    MESSAGE(STATUS "This is unix, and add the librt.so")
    target_link_libraries (hconf_agent rt)
ENDIF(WIN32)

#build libcurl
execute_process(WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/../deps/curl COMMAND ./configure
    --prefix=${PROJECT_SOURCE_DIR}/../deps/curl/_install   --disable-ldap
    --disable-ldaps --without-ssl --without-zlib --without-libidn --without-librtmp)
execute_process(WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/../deps/curl COMMAND
    make)
execute_process(WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/../deps/curl COMMAND
    make install)


#build libgdbm
execute_process(WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/../deps/gdbm COMMAND ./configure
    --prefix=${PROJECT_SOURCE_DIR}/../deps/gdbm/_install)
execute_process(WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/../deps/gdbm COMMAND
    make)
execute_process(WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/../deps/gdbm COMMAND
    make install)

# link libcurl
target_link_libraries(hconf_agent
    ${PROJECT_SOURCE_DIR}/../deps/curl/_install/lib/libcurl.a)
# link static libzookeeper_mt
target_link_libraries(hconf_agent
    ${PROJECT_SOURCE_DIR}/../deps/zookeeper/_install/lib/libzookeeper_mt.a)
# link static libgdbm
target_link_libraries(hconf_agent ${PROJECT_SOURCE_DIR}/../deps/gdbm/_install/lib/libgdbm.a)
file(WRITE lockfile "")

install(DIRECTORY bin DESTINATION .)
install(DIRECTORY conf DESTINATION .)
install(DIRECTORY script DESTINATION .)
install(DIRECTORY DESTINATION cmd)
install(DIRECTORY DESTINATION dumps)
install(DIRECTORY DESTINATION monitor)
install(DIRECTORY DESTINATION result)
install(DIRECTORY DESTINATION lock)
install(FILES lockfile DESTINATION lock)
install(DIRECTORY DESTINATION doc)
install(DIRECTORY DESTINATION logs)
install(TARGETS hconf_agent DESTINATION bin)
install(FILES version DESTINATION .)
