#CMakelist root
cmake_minimum_required (VERSION 2.6)
project (hconf-monitor)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX
        "/usr/local/hconf/monitor" CACHE PATH "hconf_mointor install prefix" FORCE
        )
endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

include_directories(${PROJECT_SOURCE_DIR}/include)
add_subdirectory(src)

install(DIRECTORY bin DESTINATION .)
install(DIRECTORY conf DESTINATION .)
install(DIRECTORY DESTINATION logs)
install(DIRECTORY DESTINATION tmp)
